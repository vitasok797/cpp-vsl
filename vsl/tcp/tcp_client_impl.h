#ifndef VSL_TCP_TCP_CLIENT_IMPL_H
#define VSL_TCP_TCP_CLIENT_IMPL_H

#include <vsl/concepts.h>
#include <vsl/scope_guard.h>

#include <gsl/narrow>
#include <Poco/BinaryReader.h>
#include <Poco/BinaryWriter.h>
#include <Poco/Exception.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/StreamSocket.h>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace vsl::tcp
{

inline TcpClient::TcpClient(ByteOrder byte_order)
    : TcpClient{Poco::Net::StreamSocket{}, byte_order}
{}

inline TcpClient::TcpClient(Poco::Net::StreamSocket socket, ByteOrder byte_order)
    : socket_{std::move(socket)},
      socket_stream_{std::make_shared<Poco::Net::SocketStream>(socket_)},
      binary_reader_{*socket_stream_, static_cast<Poco::BinaryReader::StreamByteOrder>(byte_order)},
      binary_writer_{*socket_stream_, static_cast<Poco::BinaryWriter::StreamByteOrder>(byte_order)}
{
    if (socket_.impl()->initialized())
    {
        socket_.setNoDelay(true);
    }
}

inline auto TcpClient::connect(const std::pair<std::string, int>& endpoint) -> void
{
    connect(endpoint.first, endpoint.second);
}

inline auto TcpClient::connect(const std::string& host, int port) -> void
{
    try
    {
        const auto socket_addr = Poco::Net::SocketAddress{host, gsl::narrow<uint16_t>(port)};
        connect(socket_addr);
    }
    catch (const Poco::Exception& ex)
    {
        throw_connect_error(ex.displayText());
    }
}

inline auto TcpClient::connect(const std::string& endpoint) -> void
{
    try
    {
        const auto socket_addr = Poco::Net::SocketAddress{endpoint};
        connect(socket_addr);
    }
    catch (const Poco::Exception& ex)
    {
        throw_connect_error(ex.displayText());
    }
}

inline auto TcpClient::connect(Poco::Net::SocketAddress socket_addr) -> void
{
    socket_.connect(socket_addr);
    socket_.setNoDelay(true);
}

inline auto TcpClient::throw_connect_error(std::string_view error_desc) -> void
{
    throw TcpClientError{"Connection failed", error_desc};
}

inline auto TcpClient::shutdown(ShutdownType how) -> void
{
    try
    {
        switch (how)
        {
        case ShutdownType::BOTH:
            socket_.shutdown();
            break;
        case ShutdownType::RECEIVE:
            socket_.shutdownReceive();
            break;
        case ShutdownType::SEND:
            socket_.shutdownSend();
            break;
        }
    }
    catch (const Poco::Exception& ex)
    {
        throw TcpClientError{"Failed to shutdown connection", ex.displayText()};
    }
}

inline auto TcpClient::close() -> void
{
    try
    {
        socket_.close();
    }
    catch (const Poco::Exception& ex)
    {
        throw TcpClientError{"Failed to close connection", ex.displayText()};
    }
}

template<typename T>
    requires vsl::numeric<T>
auto TcpClient::read() -> T
{
    T value;
    binary_reader_ >> value;
    check_stream_status(binary_reader_);
    return value;
}

template<typename T>
    requires vsl::numeric<T>
auto TcpClient::write(T value) -> void
{
    binary_writer_ << value;
    check_stream_status(binary_writer_);
}

template<typename ItemType, typename SizeType>
    requires vsl::one_of<SizeType, TcpClient::size64_t, TcpClient::size32_t>
auto TcpClient::read_vector() -> std::vector<ItemType>
{
    auto size = read<typename SizeType::type>();
    auto vec = std::vector<ItemType>{};
    vec.reserve(size);
    for (size_t i = 0; i < size; ++i)
    {
        vec.push_back(read<ItemType>());
    }
    return vec;
}

template<typename ItemType, typename SizeType>
    requires vsl::one_of<SizeType, TcpClient::size64_t, TcpClient::size32_t>
auto TcpClient::write_vector(const std::vector<ItemType>& vec) -> void
{
    auto size = gsl::narrow<typename SizeType::type>(vec.size());
    write(size);
    for (const auto& el : vec)
    {
        write(el);
    }
}

template<typename SizeType>
    requires vsl::one_of<SizeType, TcpClient::size64_t, TcpClient::size32_t>
auto TcpClient::read_string() -> std::string
{
    auto size = read<typename SizeType::type>();
    auto str = std::string{};
    str.resize(size);
    read_raw(std::span{str});
    return str;
}

template<typename SizeType>
    requires vsl::one_of<SizeType, TcpClient::size64_t, TcpClient::size32_t>
auto TcpClient::write_string(std::string_view str) -> void
{
    auto size = gsl::narrow<typename SizeType::type>(str.size());
    write(size);
    write_raw(std::span{str});
}

template<typename T, std::size_t Extent>
    requires vsl::one_of<T, std::byte, uint8_t, char, unsigned char>
auto TcpClient::read_raw(std::span<T, Extent> buffer) -> void
{
    auto data_ptr = reinterpret_cast<char*>(buffer.data());
    auto data_size = gsl::narrow<std::streamsize>(buffer.size());
    binary_reader_.readRaw(data_ptr, data_size);
    check_stream_status(binary_reader_);
}

template<typename T, std::size_t Extent>
    requires vsl::one_of<T, std::byte, uint8_t, char, unsigned char>
auto TcpClient::write_raw(std::span<T, Extent> buffer) -> void
{
    auto data_ptr = reinterpret_cast<const char*>(buffer.data());
    auto data_size = gsl::narrow<std::streamsize>(buffer.size());
    binary_writer_.writeRaw(data_ptr, data_size);
    check_stream_status(binary_writer_);
}

inline auto TcpClient::flush() -> void
{
    try
    {
        binary_writer_.flush();
    }
    catch (const Poco::Net::NetException&)
    {
        throw TcpClientConnectionReset{};
    }

    check_stream_status(binary_writer_);
}

inline auto TcpClient::check_connection() -> void
{
    auto is_blocking = socket_.getBlocking();
    VSL_SCOPE_GUARD
    {
        socket_.setBlocking(is_blocking);
    };
    socket_.setBlocking(false);

    try
    {
        auto buffer = std::byte{};
        auto received_count = socket_.receiveBytes(&buffer, 1, MSG_PEEK);

        if (received_count == 0)
        {
            throw TcpClientGracefulShutdown{};
        }
    }
    catch (const Poco::Net::ConnectionResetException&)
    {
        throw TcpClientConnectionReset{};
    }
}

template<typename T>
    requires vsl::one_of<T, Poco::BinaryReader, Poco::BinaryWriter>
auto TcpClient::check_stream_status(T& stream) const -> void
{
    if constexpr (requires { stream.eof(); })
    {
        if (stream.eof())
        {
            throw TcpClientGracefulShutdown{};
        }
    }

    if (stream.bad())
    {
        throw TcpClientConnectionReset{};
    }

    if (!stream.good())
    {
        throw TcpClientError{"Unknown stream error"};
    }
}

inline auto TcpClient::wait_for_disconnect() -> void
{
    constexpr auto MAX_STREAM_SIZE = std::numeric_limits<std::streamsize>::max();
    binary_reader_.stream().ignore(MAX_STREAM_SIZE);
}

inline auto TcpClient::data_available() -> int
{
    auto in_stream = gsl::narrow<int>(binary_reader_.available());
    auto in_socket = socket_.available();
    auto total = in_stream + in_socket;
    if (total > 0) return total;
    check_connection();
    return 0;
}

inline auto TcpClient::is_active() const -> bool
{
    return socket_.impl()->initialized();
}

inline auto TcpClient::get_no_dalay() const -> bool
{
    return socket_.getNoDelay();
}

inline auto TcpClient::set_no_dalay(bool state) -> void
{
    socket_.setNoDelay(state);
}

inline auto TcpClient::get_receive_buffer_size() const -> int
{
    return socket_.getReceiveBufferSize();
}

inline auto TcpClient::set_receive_buffer_size(int size) -> void
{
    socket_.setReceiveBufferSize(size);
}

inline auto TcpClient::get_send_buffer_size() const -> int
{
    return socket_.getSendBufferSize();
}

inline auto TcpClient::set_send_buffer_size(int size) -> void
{
    socket_.setSendBufferSize(size);
}

inline auto TcpClient::get_local_endpoint() const -> std::pair<std::string, int>
{
    auto socket_addr = socket_.address();
    return std::pair{socket_addr.host().toString(), socket_addr.port()};
}

inline auto TcpClient::get_remote_endpoint() const -> std::pair<std::string, int>
{
    auto socket_addr = socket_.peerAddress();
    return std::pair{socket_addr.host().toString(), socket_addr.port()};
}

}  // namespace vsl::tcp

#endif  // VSL_TCP_TCP_CLIENT_IMPL_H
