#include "tcp_client.h"
#include "tcp_listener.h"

#include <fmt/format.h>

#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <span>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace vsl::tcp::test
{

static const auto server_endpoint = std::pair{"0.0.0.0", 7777};
static const auto client_remote_endpoint = std::pair{"127.0.0.1", 7777};

static auto create_connections(TcpClient::ByteOrder client_byte_order = TcpClient::ByteOrder::NATIVE,
                               TcpClient::ByteOrder server_byte_order = TcpClient::ByteOrder::NATIVE)
    -> std::pair<TcpClient, TcpClient>
{
    auto listener = TcpListener{};
    listener.start(server_endpoint);
    assert(listener.is_listening() == true);

    auto client = TcpClient{client_byte_order};
    assert(client.is_active() == false);

    client.connect(client_remote_endpoint);
    assert(client.is_active() == true);

    auto server = listener.accept_client(server_byte_order);

    listener.stop();
    assert(listener.is_listening() == false);

    // testing TcpClient copy/move
    return {client, std::move(server)};
}

static auto close_connections(TcpClient& client, TcpClient& server) -> void
{
    client.close();
    server.close();
}

static auto test_connect(auto start_listener, auto connect_client) -> void
{
    auto listener = TcpListener{};
    start_listener(listener);
    assert(listener.is_listening() == true);

    auto client = TcpClient{};
    assert(client.is_active() == false);

    connect_client(client);
    assert(client.is_active() == true);

    auto server = listener.accept_client();

    listener.stop();
    assert(listener.is_listening() == false);

    client.close();
    server.close();
}

static auto test_connect() -> void
{
    test_connect(                                                     //
        [](auto& listener) { listener.start(server_endpoint); },      //
        [](auto& client) { client.connect(client_remote_endpoint); }  //
    );                                                                //

    test_connect(                                                        //
        [](auto& listener) { listener.start(server_endpoint.second); },  //
        [](auto& client) { client.connect(client_remote_endpoint); }     //
    );                                                                   //

    test_connect(                                                                                          //
        [](auto& listener) { listener.start(server_endpoint.first, server_endpoint.second); },             //
        [](auto& client) { client.connect(client_remote_endpoint.first, client_remote_endpoint.second); }  //
    );                                                                                                     //

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

    try
    {
        // throws TcpClientError
        test_connect(                                                            //
            [](auto& listener) { listener.start(server_endpoint.second + 1); },  //
            [](auto& client) { client.connect(client_remote_endpoint); }         //
        );                                                                       //
        assert(false && "Exception is not thrown");
    }
    catch (const TcpClientError&)
    {}
}

static auto test_options() -> void
{
    auto [client, server] = create_connections();

    // -------------------------------------------------------------------------------

    assert(client.is_active() == true);
    assert(client.get_no_dalay() == true);
    assert(client.get_local_endpoint().first == "127.0.0.1");
    assert(client.get_remote_endpoint().first == "127.0.0.1");
    assert(client.get_remote_endpoint().second == client_remote_endpoint.second);

    assert(server.is_active() == true);
    assert(server.get_no_dalay() == true);
    assert(server.get_local_endpoint().first == "127.0.0.1");
    assert(server.get_local_endpoint().second == server_endpoint.second);
    assert(server.get_remote_endpoint().first == "127.0.0.1");

    // -------------------------------------------------------------------------------

    close_connections(client, server);

    assert(client.is_active() == false);
    assert(server.is_active() == false);
}

static auto test_send_recv() -> void
{
    auto [client, server] = create_connections();

    // -------------------------------------------------------------------------------

    auto v1 = std::vector<int64_t>{101, 102, -103};
    auto v2 = std::vector<int32_t>{201, 202, -203};
    auto v3 = std::vector<int16_t>{301, 302, -303};

    client.write_vector(v1);
    client.write_vector<decltype(v2)::value_type, TcpClient::size64_t>(v2);
    client.write_vector<decltype(v3)::value_type, TcpClient::size32_t>(v3);

    client.flush();

    assert((server.read_vector<decltype(v1)::value_type, TcpClient::size64_t>()) == v1);
    assert((server.read_vector<decltype(v2)::value_type>()) == v2);
    assert((server.read_vector<decltype(v3)::value_type, TcpClient::size32_t>()) == v3);

    // -------------------------------------------------------------------------------

    auto s1 = std::string{"Hello"};
    auto s2 = std::string{"World"};
    auto s3 = std::string{"Привет"};

    client.write_string(s1);
    client.write_string<TcpClient::size64_t>(s2);
    client.write_string<TcpClient::size32_t>(s3);

    client.flush();

    assert(server.read_string<TcpClient::size64_t>() == s1);
    assert(server.read_string() == s2);
    assert(server.read_string<TcpClient::size32_t>() == s3);

    // -------------------------------------------------------------------------------

    auto i1 = int64_t{-64};
    auto i2 = uint64_t{64};
    auto i3 = int32_t{-32};
    auto i4 = uint32_t{32};
    auto i5 = int8_t{-8};
    auto i6 = uint8_t{8};

    client.write(i1);
    client.write(i2);
    client.write(i3);
    client.write(i4);
    client.write(i5);
    client.write(i6);

    client.flush();

    assert(server.read<decltype(i1)>() == i1);
    assert(server.read<decltype(i2)>() == i2);
    assert(server.read<decltype(i3)>() == i3);
    assert(server.read<decltype(i4)>() == i4);
    assert(server.read<decltype(i5)>() == i5);
    assert(server.read<decltype(i6)>() == i6);

    // -------------------------------------------------------------------------------

    client.write<bool>(true);
    client.write<bool>(false);

    client.flush();

    assert(server.read<bool>() == true);
    assert(server.read<bool>() == false);

    // -------------------------------------------------------------------------------

    auto float_val = 333.5f;
    auto double_val = 444.25;

    client.write(float_val);
    client.write(double_val);

    client.flush();

    assert(server.read<decltype(float_val)>() == float_val);
    assert(server.read<decltype(double_val)>() == double_val);

    // -------------------------------------------------------------------------------

    close_connections(client, server);
}

static auto test_send_recv_raw() -> void
{
    auto [client, server] = create_connections();

    // -------------------------------------------------------------------------------

    auto value_sent = int32_t{123};
    client.write_raw(std::span{reinterpret_cast<unsigned char*>(&value_sent), sizeof(decltype(value_sent))});
    client.flush();

    decltype(value_sent) value_received;
    server.read_raw(std::span{reinterpret_cast<unsigned char*>(&value_received), sizeof(decltype(value_received))});

    assert(value_sent == value_received);

    // -------------------------------------------------------------------------------

    auto chars_sent = std::vector<char>{'a', 'b', 'c'};
    client.write_raw(std::span{chars_sent});
    client.flush();

    std::array<char, 3> chars_received;
    server.read_raw(std::span{chars_received});

    assert(chars_sent[0] == chars_received[0]);
    assert(chars_sent[1] == chars_received[1]);
    assert(chars_sent[2] == chars_received[2]);

    // -------------------------------------------------------------------------------

    close_connections(client, server);
}

static auto test_data_available() -> void
{
    constexpr auto DELAY = std::chrono::milliseconds(100);

    auto [client, server] = create_connections();

    // -------------------------------------------------------------------------------

    assert(server.data_available() == 0);

    client.write(int16_t{});
    client.flush();
    std::this_thread::sleep_for(DELAY);
    assert(server.data_available() == 2);

    server.read<int8_t>();
    assert(server.data_available() == 1);

    client.write(int8_t{});
    client.flush();
    std::this_thread::sleep_for(DELAY);
    assert(server.data_available() == 2);

    server.read<int16_t>();
    assert(server.data_available() == 0);

    client.write(int16_t{});
    client.flush();
    client.shutdown();
    std::this_thread::sleep_for(DELAY);
    assert(server.data_available() == 2);

    server.read<int8_t>();
    server.read<int8_t>();

    try
    {
        server.data_available();  // throws TcpClientGracefulShutdown
        assert(false && "Exception is not thrown");
    }
    catch (const TcpClientGracefulShutdown&)
    {}

    // -------------------------------------------------------------------------------

    close_connections(client, server);
}

static auto test_wait_for_disconnect() -> void
{
    auto [client, server] = create_connections();

    // -------------------------------------------------------------------------------

    client.write(0);
    client.write(0);
    client.write(0);
    client.flush();
    client.shutdown(TcpClient::ShutdownType::SEND);

    server.wait_for_disconnect();

    // -------------------------------------------------------------------------------

    close_connections(client, server);
}

static auto test_graceful_disconnect() -> void
{
    auto [client, server] = create_connections();

    // -------------------------------------------------------------------------------

    client.write(int32_t{});
    client.flush();
    client.shutdown();

    server.read<int8_t>();
    server.read<int8_t>();
    server.read<int8_t>();
    server.read<int8_t>();

    try
    {
        server.read<int8_t>();  // throws TcpClientGracefulShutdown
        assert(false && "Exception is not thrown");
    }
    catch (const TcpClientGracefulShutdown&)
    {}

    // -------------------------------------------------------------------------------

    close_connections(client, server);
}

static auto test_connection_reset() -> void
{
    auto [client, server] = create_connections();

    // -------------------------------------------------------------------------------

    client.shutdown();

    try
    {
        client.read<int8_t>();  // throws TcpClientConnectionReset
        assert(false && "Exception is not thrown");
    }
    catch (const TcpClientConnectionReset&)
    {}

    // -------------------------------------------------------------------------------

    close_connections(client, server);
}

static auto test_connection_reset2() -> void
{
    auto [client, server] = create_connections();

    // -------------------------------------------------------------------------------

    client.shutdown();
    client.write(0);

    try
    {
        client.flush();  // throws TcpClientConnectionReset
        assert(false && "Exception is not thrown");
    }
    catch (const TcpClientConnectionReset&)
    {}

    // -------------------------------------------------------------------------------

    close_connections(client, server);
}

static auto test_endianness() -> void
{
    auto [client, server] = create_connections(TcpClient::ByteOrder::LITTLE_ENDIAN, TcpClient::ByteOrder::BIG_ENDIAN);

    // -------------------------------------------------------------------------------

    auto value = uint16_t{0x0005};
    auto inverted_value = uint16_t{0x0500};

    client.write(value);
    client.flush();
    assert(server.read<uint16_t>() == inverted_value);

    server.write(inverted_value);
    server.flush();
    assert(client.read<uint16_t>() == value);

    // -------------------------------------------------------------------------------

    close_connections(client, server);
}

auto test_tcp() -> void
{
    std::cout << "testing \"tcp\": ";

    test_connect();
    test_options();
    test_send_recv();
    test_send_recv_raw();
    test_data_available();
    test_wait_for_disconnect();
    test_graceful_disconnect();
    test_connection_reset();
    test_connection_reset2();
    test_endianness();

    std::cout << "OK" << std::endl;
}

}  // namespace vsl::tcp::test
