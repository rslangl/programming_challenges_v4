#include "scanner.hpp"

using namespace scanner;

namespace scanner {

using port_state = std::pair<std::string, std::string>;
using host_result = std::vector<port_state>;

auto scan(const std::vector<std::string> hosts,
          const std::vector<std::string> ports, std::string protocol)
    -> std::expected<
        std::map<std::string, std::vector<std::pair<std::string, std::string>>>,
        std::string> {

  // TODO: use protocol here to shutup the compiler
  std::cerr << protocol << '\n';

  std::map<std::string, host_result> scan{};

  for (const auto &host : hosts) {

    for (const auto &port : ports) {

      // Return code
      int rc;
      // Linked list of resolved addresses for host
      struct addrinfo *res = nullptr;
      // Control-flow
      bool is_open = false;
      bool is_closed = false;
      bool saw_timeout = false;

      // Host configurations
      struct addrinfo hints{};
      // Any address family, both IPv4 and IPv6
      hints.ai_family = AF_UNSPEC;
      // Protocol, either TCP (SOCK_STREAM) or UDP (SOCK_DRGRAM)
      hints.ai_socktype = SOCK_STREAM;

      rc = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
      if (rc != 0) {
        return std::unexpected(
            std::format("getaddrinfo() error: {}", gai_strerror(rc)));
      }

      // Iterate all resolved IP addresses for the remote host and create a
      // socket for each
      for (struct addrinfo *ai = res; ai != nullptr; ai = ai->ai_next) {

        int fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (fd == -1) {
          // Socket creation failed for this address for this host, try the next
          // one
          continue;
        }

        // Set socket to non-blocking
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags == -1) {
          close(fd);
          return std::unexpected(std::format("fcntl() get"));
        }

        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
          close(fd);
          return std::unexpected(std::format("fcntl() set"));
        }

        // Attempt connection
        rc = connect(fd, ai->ai_addr, ai->ai_addrlen);

        // Connection successful, report as open
        if (rc == 0) {
          is_open = true;
          close(fd);
          break;
        } else if (rc == -1 && errno == EINPROGRESS) {
          // Non-blocking connection in process
          fd_set write_fds;
          FD_ZERO(&write_fds);
          FD_SET(fd, &write_fds);

          // Specify timeout
          struct timeval timeout{};
          timeout.tv_sec = 5;
          timeout.tv_usec = 0;

          // Wait until file descriptor for socket gets ready
          rc = select(fd + 1, nullptr, &write_fds, nullptr, &timeout);
          int err = 0;
          socklen_t err_len = sizeof(err);

          if (rc > 0 && FD_ISSET(fd, &write_fds)) {

            if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &err_len) < 0) {
              close(fd);
              return std::unexpected(std::string{"getsockopt() error"});
            }

            // TODO: handle unreachable hosts, errno values:
            // EHOSTUNREACH, host unreachable
            // EADDRNOTAVAIL, invalid address
            if (err == 0 || err == EISCONN) {
              is_open = true;
            } else if (err == ECONNREFUSED || err == EACCES) {
              is_closed = true;
            } else if (err == ETIMEDOUT) {
              saw_timeout = true;
            } else {
              is_closed = true;
            }

            close(fd);
            break;

            // select() timeout
          } else if (rc == 0) {
            saw_timeout = true;
            close(fd);
            break;

          } else {
            close(fd);
            std::error_code ec(errno, std::system_category());
            return std::unexpected(
                std::format("select() error: {}", ec.message()));
          }
        } else if (errno == ECONNREFUSED || errno == EACCES) {
          is_closed = true;
          close(fd);
          break;
        } else {
          close(fd);
        }
      } // end address loop

      freeaddrinfo(res);

      std::string state;
      if (is_open) {
        state = "OPEN";
      } else if (is_closed) {
        state = "CLOSED";
      } else if (saw_timeout) {
        state = "FILTERED";
      } else {
        state = "UNKNOWN";
      }

      scan[host].push_back({port, state});

    } // end port loop
  } // end host loop

  return scan;
}

} // namespace scanner
