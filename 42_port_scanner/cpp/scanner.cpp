#include "scanner.hpp"

using namespace scanner;

namespace scanner {
namespace {
auto scan_one(int fd, const struct sockaddr_storage &remote)
    -> std::expected<port_state, scanner_error> {
  auto *sa = reinterpret_cast<const struct sockaddr *>(&remote);

  if (connect(fd, sa, sizeof(struct sockaddr_in)) == -1) {

    // Non-blocking socket: wait until connect completes
    if (errno == EINPROGRESS) {
      fd_set write_fds;
      FD_ZERO(&write_fds);
      FD_SET(fd, &write_fds);
      select(fd + 1, nullptr, &write_fds, nullptr, nullptr);

      // Check if connection succeeded
      int error = 0;
      socklen_t err_len = sizeof(error);
      getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &err_len);

      if (error != 0) {
        return std::unexpected(scanner_error::connection_error);
      }
    }

    // TODO: handle unreachable hosts, errno values:
    // ETIMEDOUT, connection attempt timed out
    // EHOSTUNREACH, host unreachable
    // EADDRNOTAVAIL, invalid address
    switch (errno) {
    case EISCONN: {
      return port_state::OPEN;
      break;
    }
    case ECONNREFUSED: {
      return port_state::CLOSED;
      break;
    }
    case EACCES: {
      return port_state::CLOSED;
      break;
    }
    default:
      return port_state::UNKNOWN;
    }
  }

  return port_state::OPEN;
}
} // namespace

auto scan(const std::vector<std::string> hosts,
          const std::vector<uint16_t> ports, const protocol protocol) -> void {

  std::for_each(
      hosts.begin(), hosts.end(), [&ports, &protocol](const auto host) -> void {
        std::for_each(
            ports.begin(), ports.end(),
            [&host, &protocol](const auto port) -> void {
              if (auto opt = make_socket(host, ip_type::IPv4, port, protocol);
                  opt.has_value()) {

                auto socket = std::move(*opt);

                if (const auto port_state = scan_one(socket.fd, socket.addr);
                    port_state.has_value()) {
                  print(port, *port_state);
                } else {
                  // TODO: pretty-print error for host
                }

              } else {
                std::cerr << "ERROR: Could not create network socket for host '"
                          << host << "'\n";
              }
            });
      });
}

} // namespace scanner
