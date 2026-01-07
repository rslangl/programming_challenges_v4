#pragma once

#include "protocol.hpp"
#include "utils.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <system_error>
#include <tuple>
#include <unistd.h>

namespace scanner {

enum class ip_type { IPv4, IPv6 };

struct socket_data {
  int fd{-1};
  struct sockaddr_storage addr;
};

// auto make_socket(std::string_view host, ip_type type, const uint16_t port
//              /*protocol protocol*/)
// -> std::expected<struct socket_data, std::string_view>;
//
auto make_socket(const char *host, const char *port, const char *protocol)
    -> std::expected<struct socket_data, std::error_code>;

} // namespace scanner
