#include "tcp_client.h"
#include "tcp_listener.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fmt/format.h>

#include <array>
#include <chrono>
#include <cstdint>
#include <span>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace vsl::tcp::test
{

static const auto server_endpoint = std::pair{"0.0.0.0", 8888};
static const auto client_remote_endpoint = std::pair{"127.0.0.1", 8888};

class BaseTcpTest : public ::testing::Test
{
  protected:
    BaseTcpTest(TcpClient::ByteOrder client_byte_order, TcpClient::ByteOrder server_byte_order)
        : CLIENT_BYTE_ORDER{client_byte_order},
          SERVER_BYTE_ORDER{server_byte_order}
    {}

    auto SetUp() -> void override
    {
        auto listener = TcpListener{};
        listener.start(server_endpoint);
        ASSERT_TRUE(listener.is_listening());

        client_ = TcpClient{CLIENT_BYTE_ORDER};
        ASSERT_FALSE(client_.is_active());

        client_.connect(client_remote_endpoint);
        ASSERT_TRUE(client_.is_active());

        server_ = listener.accept_client(SERVER_BYTE_ORDER);

        listener.stop();
        ASSERT_FALSE(listener.is_listening());
    }

    auto TearDown() -> void override
    {
        client_.close();
        server_.close();

        ASSERT_FALSE(client_.is_active());
        ASSERT_FALSE(server_.is_active());
    }

    const TcpClient::ByteOrder CLIENT_BYTE_ORDER;
    const TcpClient::ByteOrder SERVER_BYTE_ORDER;

    TcpClient client_;
    TcpClient server_;
};

class TcpTest : public BaseTcpTest
{
  protected:
    TcpTest()
        : BaseTcpTest{TcpClient::ByteOrder::NATIVE, TcpClient::ByteOrder::NATIVE}
    {}
};

class TcpEndiannessTest : public BaseTcpTest
{
  protected:
    TcpEndiannessTest()
        : BaseTcpTest{TcpClient::ByteOrder::LITTLE_ENDIAN, TcpClient::ByteOrder::BIG_ENDIAN}
    {}
};

static auto test_connect(auto start_listener, auto connect_client) -> void
{
    auto listener = TcpListener{};
    start_listener(listener);
    ASSERT_TRUE(listener.is_listening());

    auto client = TcpClient{};
    ASSERT_FALSE(client.is_active());

    connect_client(client);
    ASSERT_TRUE(client.is_active());

    auto server = listener.accept_client();

    listener.stop();
    ASSERT_FALSE(listener.is_listening());

    client.close();
    server.close();
}

TEST(TcpConnectTest, SuccessfulConnect1)
{
    test_connect(                                                     //
        [](auto& listener) { listener.start(server_endpoint); },      //
        [](auto& client) { client.connect(client_remote_endpoint); }  //
    );                                                                //
}

TEST(TcpConnectTest, SuccessfulConnect2)
{
    test_connect(                                                        //
        [](auto& listener) { listener.start(server_endpoint.second); },  //
        [](auto& client) { client.connect(client_remote_endpoint); }     //
    );                                                                   //
}

TEST(TcpConnectTest, SuccessfulConnect3)
{
    test_connect(                                                                                          //
        [](auto& listener) { listener.start(server_endpoint.first, server_endpoint.second); },             //
        [](auto& client) { client.connect(client_remote_endpoint.first, client_remote_endpoint.second); }  //
    );                                                                                                     //
}

TEST(TcpConnectTest, SuccessfulConnect4)
{
    auto server_endpoint_as_str = fmt::format("{}:{}",                                //
                                              server_endpoint.first,                  //
                                              server_endpoint.second);                //
    auto client_remote_endpoint_as_str = fmt::format("{}:{}",                         //
                                                     client_remote_endpoint.first,    //
                                                     client_remote_endpoint.second);  //
    test_connect(                                                                     //
        [&](auto& listener) { listener.start(server_endpoint_as_str); },              //
        [&](auto& client) { client.connect(client_remote_endpoint_as_str); }          //
    );                                                                                //
}

TEST(TcpConnectTest, UnableToBind)
{
    auto listener = TcpListener{};
    listener.start(server_endpoint);
    EXPECT_THROW(TcpListener{}.start(server_endpoint), TcpListenerError);
}

TEST(TcpConnectTest, UnableToConnect)
{
    EXPECT_THROW(TcpClient{}.connect(client_remote_endpoint), TcpClientError);
}

TEST_F(TcpTest, Options)
{
    EXPECT_TRUE(client_.is_active());
    EXPECT_EQ(client_.get_no_dalay(), true);
    EXPECT_EQ(client_.get_local_endpoint().first, "127.0.0.1");
    EXPECT_EQ(client_.get_remote_endpoint().first, "127.0.0.1");
    EXPECT_EQ(client_.get_remote_endpoint().second, client_remote_endpoint.second);

    EXPECT_TRUE(server_.is_active());
    EXPECT_EQ(server_.get_no_dalay(), true);
    EXPECT_EQ(server_.get_local_endpoint().first, "127.0.0.1");
    EXPECT_EQ(server_.get_local_endpoint().second, server_endpoint.second);
    EXPECT_EQ(server_.get_remote_endpoint().first, "127.0.0.1");
}

TEST_F(TcpTest, SendRecv)
{
    auto v1 = std::vector<int64_t>{101, 102, -103};
    auto v2 = std::vector<int32_t>{201, 202, -203};
    auto v3 = std::vector<int16_t>{301, 302, -303};

    client_.write_vector(v1);
    client_.write_vector<decltype(v2)::value_type, TcpClient::size64_t>(v2);
    client_.write_vector<decltype(v3)::value_type, TcpClient::size32_t>(v3);

    client_.flush();

    ASSERT_EQ((server_.read_vector<decltype(v1)::value_type, TcpClient::size64_t>()), v1);
    ASSERT_EQ((server_.read_vector<decltype(v2)::value_type>()), v2);
    ASSERT_EQ((server_.read_vector<decltype(v3)::value_type, TcpClient::size32_t>()), v3);

    // -------------------------------------------------------------------------------

    auto s1 = std::string{"Hello"};
    auto s2 = std::string{"World"};
    auto s3 = std::string{"Привет"};

    client_.write_string(s1);
    client_.write_string<TcpClient::size64_t>(s2);
    client_.write_string<TcpClient::size32_t>(s3);

    client_.flush();

    ASSERT_EQ(server_.read_string<TcpClient::size64_t>(), s1);
    ASSERT_EQ(server_.read_string(), s2);
    ASSERT_EQ(server_.read_string<TcpClient::size32_t>(), s3);

    // -------------------------------------------------------------------------------

    auto i1 = int64_t{-64};
    auto i2 = uint64_t{64};
    auto i3 = int32_t{-32};
    auto i4 = uint32_t{32};
    auto i5 = int8_t{-8};
    auto i6 = uint8_t{8};

    client_.write(i1);
    client_.write(i2);
    client_.write(i3);
    client_.write(i4);
    client_.write(i5);
    client_.write(i6);

    client_.flush();

    ASSERT_EQ(server_.read<decltype(i1)>(), i1);
    ASSERT_EQ(server_.read<decltype(i2)>(), i2);
    ASSERT_EQ(server_.read<decltype(i3)>(), i3);
    ASSERT_EQ(server_.read<decltype(i4)>(), i4);
    ASSERT_EQ(server_.read<decltype(i5)>(), i5);
    ASSERT_EQ(server_.read<decltype(i6)>(), i6);

    // -------------------------------------------------------------------------------

    client_.write<bool>(true);
    client_.write<bool>(false);

    client_.flush();

    ASSERT_EQ(server_.read<bool>(), true);
    ASSERT_EQ(server_.read<bool>(), false);

    // -------------------------------------------------------------------------------

    auto float_val = 333.5f;
    auto double_val = 444.25;

    client_.write(float_val);
    client_.write(double_val);

    client_.flush();

    ASSERT_EQ(server_.read<decltype(float_val)>(), float_val);
    ASSERT_EQ(server_.read<decltype(double_val)>(), double_val);
}

TEST_F(TcpTest, SendRecvRaw)
{
    auto value_sent = int32_t{123};
    client_.write_raw(std::span{reinterpret_cast<unsigned char*>(&value_sent), sizeof(decltype(value_sent))});
    client_.flush();

    decltype(value_sent) value_received;
    server_.read_raw(std::span{reinterpret_cast<unsigned char*>(&value_received), sizeof(decltype(value_received))});

    ASSERT_EQ(value_sent, value_received);

    // -------------------------------------------------------------------------------

    auto chars_sent = std::vector<char>{'a', 'b', 'c'};
    client_.write_raw(std::span{chars_sent});
    client_.flush();

    std::array<char, 3> chars_received;
    server_.read_raw(std::span{chars_received});

    ASSERT_EQ(chars_sent[0], chars_received[0]);
    ASSERT_EQ(chars_sent[1], chars_received[1]);
    ASSERT_EQ(chars_sent[2], chars_received[2]);
}

TEST_F(TcpTest, DataAvailable)
{
    constexpr auto DELAY = std::chrono::milliseconds(100);

    ASSERT_EQ(server_.data_available(), 0);

    client_.write(int16_t{});
    client_.flush();
    std::this_thread::sleep_for(DELAY);
    ASSERT_EQ(server_.data_available(), 2);

    server_.read<int8_t>();
    ASSERT_EQ(server_.data_available(), 1);

    client_.write(int8_t{});
    client_.flush();
    std::this_thread::sleep_for(DELAY);
    ASSERT_EQ(server_.data_available(), 2);

    server_.read<int16_t>();
    ASSERT_EQ(server_.data_available(), 0);

    client_.write(int16_t{});
    client_.flush();
    client_.shutdown();
    std::this_thread::sleep_for(DELAY);
    ASSERT_EQ(server_.data_available(), 2);

    server_.read<int8_t>();
    server_.read<int8_t>();

    ASSERT_THROW(server_.data_available(), TcpClientGracefulShutdown);
}

TEST_F(TcpTest, WaitForDisconnect)
{
    client_.write(0);
    client_.write(0);
    client_.write(0);
    client_.flush();
    client_.shutdown(TcpClient::ShutdownType::SEND);

    server_.wait_for_disconnect();
}

TEST_F(TcpTest, GracefulDisconnect)
{
    client_.write(int32_t{});
    client_.flush();
    client_.shutdown();

    server_.read<int8_t>();
    server_.read<int8_t>();
    server_.read<int8_t>();
    server_.read<int8_t>();

    ASSERT_THROW(server_.read<int8_t>(), TcpClientGracefulShutdown);
}

TEST_F(TcpTest, ConnectionResetOnRead)
{
    client_.shutdown();
    ASSERT_THROW(client_.read<int8_t>(), TcpClientConnectionReset);
}

TEST_F(TcpTest, ConnectionResetOnFlush)
{
    client_.shutdown();
    client_.write(0);
    ASSERT_THROW(client_.flush(), TcpClientConnectionReset);
}

TEST_F(TcpEndiannessTest, SendRecvDiffEndiannessInt)
{
    auto value = uint16_t{0x0005};
    auto inverted_value = uint16_t{0x0500};

    client_.write(value);
    client_.flush();
    ASSERT_EQ(server_.read<uint16_t>(), inverted_value);

    server_.write(inverted_value);
    server_.flush();
    ASSERT_EQ(client_.read<uint16_t>(), value);
}

}  // namespace vsl::tcp::test
