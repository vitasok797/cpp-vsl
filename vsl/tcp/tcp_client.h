#ifndef VSL_TCP_TCP_CLIENT_H
#define VSL_TCP_TCP_CLIENT_H

#include <vsl/concepts.h>

#include <fmt/format.h>
#include <Poco/BinaryReader.h>
#include <Poco/BinaryWriter.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/StreamSocket.h>

#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace vsl::tcp
{

class TcpError : public std::exception
{
  public:
    explicit TcpError(std::string_view error_message)
        : message_{error_message}
    {}

    explicit TcpError(std::string_view error_message, std::string_view error_desc)
        : message_{fmt::format("{} ({})", error_message, error_desc)}
    {}

    auto what() const noexcept -> const char* override
    {
        return message_.c_str();
    }

  private:
    std::string message_{};
};

class TcpClientError : public TcpError
{
  public:
    using TcpError::TcpError;
};

class TcpClientDisconnect : public TcpClientError
{
  public:
    using TcpClientError::TcpClientError;
};

class TcpClientGracefulShutdown : public TcpClientDisconnect
{
  public:
    TcpClientGracefulShutdown()
        : TcpClientDisconnect{"Graceful shutdown received"}
    {}
};

class TcpClientConnectionReset : public TcpClientDisconnect
{
  public:
    TcpClientConnectionReset()
        : TcpClientDisconnect{"Connection reset"}
    {}
};

class TcpClient final
{
  public:
    enum class ShutdownType
    {
        BOTH,
        RECEIVE,
        SEND,
    };

    enum class ByteOrder
    {
        NATIVE = 1,
        BIG_ENDIAN = 2,
        NETWORK = 2,
        LITTLE_ENDIAN = 3
    };

    struct size64_t
    {
        using type = uint64_t;
    };

    struct size32_t
    {
        using type = uint32_t;
    };

    TcpClient(ByteOrder byte_order = ByteOrder::NATIVE);

    auto connect(const std::pair<std::string, int>& endpoint) -> void;
    auto connect(const std::string& host, int port) -> void;
    auto connect(const std::string& endpoint) -> void;

    auto shutdown(ShutdownType how = ShutdownType::BOTH) -> void;
    auto close() -> void;

    template<typename T>
        requires vsl::numeric<T>
    auto read() -> T;

    template<typename T>
        requires vsl::numeric<T>
    auto write(T value) -> void;

    template<typename ItemType, typename SizeType = TcpClient::size64_t>
        requires vsl::one_of<SizeType, TcpClient::size64_t, TcpClient::size32_t>
    auto read_vector() -> std::vector<ItemType>;

    template<typename ItemType, typename SizeType = TcpClient::size64_t>
        requires vsl::one_of<SizeType, TcpClient::size64_t, TcpClient::size32_t>
    auto write_vector(const std::vector<ItemType>& vec) -> void;

    template<typename SizeType = TcpClient::size64_t>
        requires vsl::one_of<SizeType, TcpClient::size64_t, TcpClient::size32_t>
    auto read_string() -> std::string;

    template<typename SizeType = TcpClient::size64_t>
        requires vsl::one_of<SizeType, TcpClient::size64_t, TcpClient::size32_t>
    auto write_string(std::string_view str) -> void;

    template<typename T, std::size_t Extent>
        requires vsl::one_of<T, std::byte, uint8_t, char, unsigned char>
    auto read_raw(std::span<T, Extent> buffer) -> void;

    template<typename T, std::size_t Extent>
        requires vsl::one_of<T, std::byte, uint8_t, char, unsigned char>
    auto write_raw(std::span<T, Extent> buffer) -> void;

    auto flush() -> void;

    auto wait_for_disconnect() -> void;
    auto data_available() -> int;
    auto is_active() const -> bool;

    auto get_no_dalay() const -> bool;
    auto set_no_dalay(bool state) -> void;

    auto get_receive_buffer_size() const -> int;
    auto set_receive_buffer_size(int size) -> void;

    auto get_send_buffer_size() const -> int;
    auto set_send_buffer_size(int size) -> void;

    auto get_local_endpoint() const -> std::pair<std::string, int>;
    auto get_remote_endpoint() const -> std::pair<std::string, int>;

  private:
    friend class TcpListener;

    explicit TcpClient(Poco::Net::StreamSocket socket, ByteOrder byte_order);

    auto connect(Poco::Net::SocketAddress socket_addr) -> void;
    auto throw_connect_error(std::string_view error_desc) -> void;
    auto check_connection() -> void;

    template<typename T>
        requires vsl::one_of<T, Poco::BinaryReader, Poco::BinaryWriter>
    auto check_stream_status(T& stream) const -> void;

    Poco::Net::StreamSocket socket_;
    std::shared_ptr<Poco::Net::SocketStream> socket_stream_;
    Poco::BinaryReader binary_reader_;
    Poco::BinaryWriter binary_writer_;
};

}  // namespace vsl::tcp

#include "tcp_client_impl.h"

#endif  // VSL_TCP_TCP_CLIENT_H
