#ifndef VSL_TCP_TCP_LISTENER_H
#define VSL_TCP_TCP_LISTENER_H

#include "tcp_client.h"

#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketAddress.h>

#include <string>
#include <string_view>

namespace vsl::tcp
{

class TcpListenerError : public TcpError
{
    using TcpError::TcpError;
};

class TcpListener final
{
  public:
    TcpListener() = default;

    auto start(int port) -> void;
    auto start(const std::pair<std::string, int>& endpoint) -> void;
    auto start(const std::string& ip, int port) -> void;
    auto start(const std::string& endpoint) -> void;
    auto stop() -> void;
    auto accept_client(TcpClient::ByteOrder byte_order = TcpClient::ByteOrder::NATIVE) -> TcpClient;
    auto get_port() -> int;
    auto is_listening() const -> bool;

  private:
    auto start(Poco::Net::SocketAddress socket_addr) -> void;
    auto throw_start_error(std::string_view error_desc) -> void;

    Poco::Net::ServerSocket server_socket_{};
    bool is_listening_{false};
};

}  // namespace vsl::tcp

#include "tcp_listener_impl.h"

#endif  // VSL_TCP_TCP_LISTENER_H
