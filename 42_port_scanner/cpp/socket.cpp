#include "socket.hpp"

namespace scanner {

namespace {
auto inline socket_type(protocol p) -> int {
  switch (p) {
  case protocol::TCP:
    return SOCK_STREAM;
  case protocol::UDP:
    return SOCK_DGRAM;
  default:
    return -1;
  }
}
} // namespace

auto make_socket(std::string_view host, ip_type type, const uint16_t port,
                 protocol protocol)
    -> std::expected<struct socket_data, scanner_error> {

  int st, flags, fd;
  struct sockaddr_storage remote{};

  if ((st = socket_type(protocol)) == -1) {
    return std::unexpected(scanner_error::invalid_input);
  }

  if ((fd = socket(AF_INET, st, 0)) == -1) {
    std::cerr << "ERROR: Error creating socket\n";
    return std::unexpected(scanner_error::socket_error);
  }

  // Acquire existing flags for socket
  if ((flags = fcntl(fd, F_GETFL, 0)) != 0) { // TODO: causes error
    std::cerr << "ERROR: Could not get socket flags";
    return std::unexpected(scanner_error::socket_error);
  }

  // Configure socket to non-blocking
  if (fcntl(fd, F_SETFL, flags |= O_NONBLOCK) != 0) {
    std::cerr << "ERROR: Could not configure to non-blocking socket\n";
    return std::unexpected(scanner_error::socket_error);
  }
  // setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

  // Handle the socket as either IPv4 or IPv6 by applying proper casting,
  // before converting host input to binary format
  switch (type) {
  case ip_type::IPv4: {
    auto *ipv4 = reinterpret_cast<sockaddr_in *>(&remote);
    ipv4->sin_family = AF_INET;
    ipv4->sin_port = htons(port);

    if (inet_pton(AF_INET, host.data(), &ipv4->sin_addr) != 1) {
      return std::unexpected(scanner_error::socket_error);
    }
    break;
  }
  case ip_type::IPv6: {
    auto *ipv6 = reinterpret_cast<sockaddr_in6 *>(&remote);
    ipv6->sin6_family = AF_INET6;
    ipv6->sin6_port = htons(port);

    if (inet_pton(AF_INET6, host.data(), &ipv6->sin6_addr) != 1) {
      return std::unexpected(scanner_error::socket_error);
    }
    break;
  }
  default:
    return std::unexpected(scanner_error::invalid_input);
  }

  return socket_data{fd, remote};
}

} // namespace scanner
