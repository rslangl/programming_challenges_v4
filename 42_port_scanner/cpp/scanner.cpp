#include "scanner.hpp"

using namespace scanner;

namespace scanner {
namespace {

struct scan_result {
  std::string host;
  // List of port-state pairs
  std::vector<std::pair<std::string, std::string>>;
};

// TODO: handle unreachable hosts, errno values:
// ETIMEDOUT, connection attempt timed out
// EHOSTUNREACH, host unreachable
// EADDRNOTAVAIL, invalid address
auto inline parse_conn_res(int err) -> int {
  switch (err) {
  case EISCONN: {
    return port_state::OPEN;
    break;
  }
  case ECONNREFUSED: {
    return port_state::CLOSED;
    break;
  }
  // Firewall
  case EACCES: {
    return port_state::CLOSED;
    break;
  }
  default:
    return port_state::UNKNOWN;
  }
}

// auto scan_one(int fd, const struct sockaddr_storage &remote)
//     -> std::expected<port_state, std::error_code> {
//
//   auto *sa = reinterpret_cast<const struct sockaddr *>(&remote);
//
//   struct addrinfo hints{};
//   struct addrinfo *res;
//
//   // std::memset(&hints, 0, sizeof(hints));
//
//   //  Do not care whether IPv4 or IPv6
//   hints.ai_family = AF_UNSPEC;
//   // TCP or UDP socket
//   hints.ai_socktype = socket_type(protocol);
//
//   int rc = getaddrinfo(host.data(), port_buffer, &hints, &res);
//   if (rc != 0) {
//     std::cerr << "resolve failed: " << gai_strerror(rc) << "\n";
//     continue;
//   }
//
//   for (struct addrinfo *addr = res; addr; addr = addr->addr_next) {
//
//     if (connect(fd, sa, sizeof(struct sockaddr_in)) == -1) {
//
//       // Non-blocking socket: wait until connect completes
//       if (errno == EINPROGRESS) {
//         fd_set write_fds;
//         FD_ZERO(&write_fds);
//         FD_SET(fd, &write_fds);
//         select(fd + 1, nullptr, &write_fds, nullptr, nullptr);
//
//         // Check if connection succeeded
//         int error = 0;
//         socklen_t err_len = sizeof(error);
//         getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &err_len);
//
//         if (error != 0) {
//           return std::unexpected(
//               std::error_code(errno, std::generic_category()));
//         }
//       }
//
//       // For non-blocking sockets returning connection error and is not a
//       // connection in progress, manual interpretation of the return value is
//       // needed
//       return parse_conn_res(errno);
//     }
//   }
//
//   // For non-blocking sockets, a -1 return value also includes a connection
//   is
//   // still in process, and must therefore be handled accordingly
//
//   // 0-returned value means successful connection for this host and port
//   return port_state::OPEN;
// }
} // namespace

// auto scan(const std::vector<std::string> hosts,
//           const std::vector<uint16_t> ports, const protocol protocol)
//     -> std::expected<std::vector<std::string>, std::string> {
//
//   std::vector<scan_result> res{};
//
//   for (const auto &host : hosts) {
//     struct scan_result h_res{};
//     h_res.host = host;
//
//     for (const auto &port : ports) {
//
//       // TODO: consider creating `hosts` and `ports` using
//       // `std::vector<const char*>` to avoid conversions
//       // auto port_data = to_charptr(port);
//       // auto host_data = to_charptr(host);
//
//       auto socket = make_socket(host.data(), port.data());
//
//       if (!socket) {
//         std::ostringstream err_msg;
//         std::error_code ec = socket.error();
//         err_msg << "socket instantiation failed (" << ec.value()
//                 << "): " << ec.message() << '\n';
//         return std::unexpected(err_msg.str());
//       }
//
//       // TODO: execute scan_one
//       // auto socket = std::move(*s);
//     }
//
//     // TODO: add to `res`
//   }
//
//   return res;
// }

auto scan(const std::vector<struct target> targets)
    -> std::expected<std::vector<scan_result>, std::string> {}

std::vector<scan_result> res{};

for (const auto &target : targets) {
  struct addrinfo hints{};
  hints.ai_family = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;

  int rc;
  struct addrinfo *res;
  if ((rc = getaddrinfo(target.host.c_str(), target.port.c_str(), &hints,
                        &res)) != 0) {
    std::stringstream err_msg;
    err_msg << "resolve failed: " << gai_strerror(rc) << '\n';
    // TODO: populate entry in res accordingly, requring a union, using
    // err_msg.str()
    continue;
  }

  // Iterate all resolved IP addresses for the remote host and create a socket
  // for each
  for (struct addrinfo *ai = res; ai; ai = ai->ai_next) {
    int fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (fd == -1) {
      // TODO: could not create socket for host, handle accordingly
      continue;
    }

    if (connect(fd, ai->ai_addr, ai->ai_addrlen) == 0) {
      // TODO: connection successful, report port for host as open
      break;
    }

    close(fd);
  }

  freeaddrinfo(res);
}

} // namespace scanner
