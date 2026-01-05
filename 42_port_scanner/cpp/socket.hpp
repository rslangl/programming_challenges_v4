#pragma once

#include <arpa/inet.h>
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

/// @brief Creates a internet socket for a given host and port
/// @param host IPv4 address of the host
/// @param port Host port to be scanned
/// @returns Tuple containing a file descriptor and socket address storage
auto make_socket(std::string host, const uint16_t port)
    -> std::optional<std::tuple<int, struct sockaddr_storage>>;

} // namespace scanner
