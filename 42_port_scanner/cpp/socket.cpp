#include "socket.hpp"

namespace scanner {
auto make_socket(std::string host, const uint16_t port)
    -> std::optional<std::tuple<int, struct sockaddr_storage>> {

  int status, fd;
  struct sockaddr_storage remote;

  if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    std::cerr << "ERROR: Error creating socket\n";
    return std::nullopt;
  }

  // Configure socket
  // fcntl(fd, F_SETFL, SOCK_NONBLOCK);
  // setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
  //
  auto *ipv4 = reinterpret_cast<sockaddr_in *>(&remote);
  ipv4->sin_family = AF_INET;
  ipv4->sin_port = htons(port);

  if ((status = inet_pton(AF_INET, host.data(), &ipv4->sin_addr)) != 1) {
    std::cerr << "ERROR: Could not convert '" << host
              << "' to network notation\n";
    return std::nullopt;
  }

  return std::make_tuple(fd, remote);
}

} // namespace scanner
