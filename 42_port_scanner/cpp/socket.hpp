#pragma once

#include "protocol.hpp"
#include <arpa/inet.h>
#include <cstdint>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <optional>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <tuple>
#include <unistd.h>

namespace scanner {

enum class iptype { IPv4, IPv6 };

struct socket_data {
  int fd{-1};
  struct sockaddr_storage addr;
};

/// @brief Creates a internet socket for a given host and port
/// @param host IPv4 address of the host
/// @param port Host port to be scanned
/// @returns Tuple containing a file descriptor and socket address storage
auto make_socket(std::string_view host, iptype type, const uint16_t port,
                 protocol protocol) -> std::optional<struct socket_data>;

} // namespace scanner
