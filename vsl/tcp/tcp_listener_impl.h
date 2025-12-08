#ifndef VSL_TCP_TCP_LISTENER_IMPL_H
#define VSL_TCP_TCP_LISTENER_IMPL_H

#include <gsl/narrow>
#include <Poco/Exception.h>
#include <Poco/Net/SocketAddress.h>

#include <cstdint>
#include <string>
#include <string_view>

namespace vsl::tcp
{

inline auto TcpListener::start(int port) -> void
{
    try
    {
        const auto socket_addr = Poco::Net::SocketAddress{gsl::narrow<uint16_t>(port)};
        start(socket_addr);
    }
    catch (const Poco::Exception& ex)
    {
        throw_start_error(ex.displayText());
    }
}

inline auto TcpListener::start(const std::pair<std::string, int>& endpoint) -> void
{
    start(endpoint.first, endpoint.second);
}

inline auto TcpListener::start(const std::string& ip, int port) -> void
{
    try
    {
        const auto socket_addr = Poco::Net::SocketAddress{ip, gsl::narrow<uint16_t>(port)};
        start(socket_addr);
    }
    catch (const Poco::Exception& ex)
    {
        throw_start_error(ex.displayText());
    }
}

inline auto TcpListener::start(const std::string& endpoint) -> void
{
    try
    {
        const auto socket_addr = Poco::Net::SocketAddress{endpoint};
        start(socket_addr);
    }
    catch (const Poco::Exception& ex)
    {
        throw_start_error(ex.displayText());
    }
}

inline auto TcpListener::start(Poco::Net::SocketAddress socket_addr) -> void
{
    if (is_listening_)
    {
        throw_start_error("Already listening");
    }

    server_socket_.bind(socket_addr);
    server_socket_.listen();
    is_listening_ = true;
}

inline auto TcpListener::throw_start_error(std::string_view error_desc) -> void
{
    throw TcpListenerError{"Failed to start listening", error_desc};
}

inline auto TcpListener::stop() -> void
{
    try
    {
        server_socket_.close();
    }
    catch (const Poco::Exception& ex)
    {
        throw TcpListenerError{"Failed to stop listening", ex.displayText()};
    }

    is_listening_ = false;
}

inline auto TcpListener::accept_client(TcpClient::ByteOrder byte_order) -> TcpClient
{
    try
    {
        auto client_socket = server_socket_.acceptConnection();
        return TcpClient{client_socket, byte_order};
    }
    catch (const Poco::Exception& ex)
    {
        throw TcpListenerError{"Failed to accept client", ex.displayText()};
    }
}

inline auto TcpListener::is_listening() const -> bool
{
    return is_listening_;
}

}  // namespace vsl::tcp

#endif  // VSL_TCP_TCP_LISTENER_IMPL_H
