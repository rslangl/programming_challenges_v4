#include "scanner.hpp"

using namespace scanner;

namespace scanner {
namespace {
auto scan_one(int fd, const struct sockaddr_storage &remote) -> port_state {
  int status;
  auto *sa = reinterpret_cast<const struct sockaddr *>(&remote);

  // If IPv4
  if (sa->sa_family == AF_INET) {
    auto *in = reinterpret_cast<const struct sockaddr_in *>(&sa);

    // TODO: check if TCP
    if ((status = connect(fd, reinterpret_cast<const struct sockaddr *>(&in),
                          sizeof(struct sockaddr_in))) != 0) {
      std::cerr << "ERROR: Could not connect to host\n";
      return port_state::UNKNOWN;
    }

    // If IPv6
  } else if (sa->sa_family == AF_INET6) {
    // auto *in6 = reinterpret_cast<const struct sockaddr_in6 *>(&sa);
  } else {
    // No-op
  }

  return port_state::OPEN;
}
} // namespace

auto scan(const std::vector<std::string> hosts,
          const std::vector<uint16_t> ports /*, const protocol protocol*/)
    -> void {

  std::for_each(hosts.begin(), hosts.end(), [&ports](const auto host) -> void {
    std::for_each(ports.begin(), ports.end(), [&host](const auto port) -> void {
      if (auto opt = make_socket(host, port); opt) {
        auto [fd, remote] = std::move(*opt);

        auto port_state = scan_one(fd, remote);
        print(port, port_state);

      } else {
        std::cerr << "ERROR: Could not create network socket for host '" << host
                  << "'\n";
      }
    });
  });
}

} // namespace scanner
