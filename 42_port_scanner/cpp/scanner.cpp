#include "scanner.hpp"

using namespace scanner;

namespace scanner {
namespace {
auto parse_errno(int err) -> port_state {

  // char errbuf[256];
  // strerror_r(err, errbuf, sizeof(errbuf));

  switch (err) {
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

auto scan_one(int fd, const struct sockaddr_storage &remote) -> port_state {
  auto *sa = reinterpret_cast<const struct sockaddr *>(&remote);

  if (connect(fd, sa, sizeof(struct sockaddr_in)) == -1) {
    if (errno == EINPROGRESS) {
      // non-blocking socket: wait and check SO_ERROR
    }

    // TODO: handle unreachable hosts, errno values:
    // ETIMEDOUT, connection attempt timed out
    // EHOSTUNREACH, host unreachable
    // EADDRNOTAVAIL, invalid address
    return parse_errno(errno);
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

                auto port_state = scan_one(socket.fd, socket.addr);
                print(port, port_state);

              } else {
                std::cerr << "ERROR: Could not create network socket for host '"
                          << host << "'\n";
              }
            });
      });
}

} // namespace scanner
