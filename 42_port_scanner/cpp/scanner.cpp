#include "scanner.hpp"

using namespace scanner;

namespace scanner {
// namespace {

// TODO: handle unreachable hosts, errno values:
// ETIMEDOUT, connection attempt timed out
// EHOSTUNREACH, host unreachable
// EADDRNOTAVAIL, invalid address
// auto inline parse_conn_res(int err) -> port_state {
//   switch (err) {
//   case EISCONN: {
//     return port_state::OPEN;
//     break;
//   }
//   case ECONNREFUSED: {
//     return port_state::CLOSED;
//     break;
//   }
//   // Firewall
//   case EACCES: {
//     return port_state::CLOSED;
//     break;
//   }
//   default:
//     return port_state::UNKNOWN;
//   }
// }
// } // namespace
//
auto scan(const std::vector<const char *> hosts,
          const std::vector<const char *> ports)
    -> std::expected<std::vector<std::string>, std::string> {

  std::vector<std::string> res{};

  for (const auto &host : hosts) {
    struct addrinfo hints{};
    hints.ai_family = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;

    int rc;
    struct addrinfo *res;

    for (const auto &port : ports) {
      if ((rc = getaddrinfo(host, port, &hints, &res)) != 0) {
        std::stringstream err_msg;
        err_msg << "resolve failed: " << gai_strerror(rc) << '\n';
        // TODO: populate entry in res accordingly, requring a union, using
        // err_msg.str()
        continue;
      }

      // Iterate all resolved IP addresses for the remote host and create a
      // socket for each
      for (struct addrinfo *ai = res; ai; ai = ai->ai_next) {
        int fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (fd == -1) {
          // TODO: could not create socket for host, handle accordingly
          continue;
        }

        int conn = connect(fd, ai->ai_addr, ai->ai_addrlen);
        if (conn == 0) {
          // TODO: connection successful, report port for host as open
          break;
        }

        // Connection in process
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
            // return std::unexpected(std::error_code(errno,
            // std::generic_category()));
            std::stringstream err_msg;
            err_msg << "resolve failed: " << gai_strerror(rc) << '\n';
            return std::unexpected(err_msg.str());
          }
        }

        // TODO: at this point, connection is a failure

        close(fd);
      }
    }

    freeaddrinfo(res);
  }
  return res;
}
} // namespace scanner
