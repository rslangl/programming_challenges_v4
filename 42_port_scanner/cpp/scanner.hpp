#pragma once

#include "port_state.hpp"
#include "protocol.hpp"
#include "socket.hpp"
#include "utils.hpp"
#include <cstdint>
#include <cstring>
#include <expected>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <vector>

namespace scanner {

auto scan(const std::vector<std::string> hosts,
          const std::vector<uint16_t> ports, const protocol protocol)
    -> std::expected<std::vector<std::string>, std::string>;

} // namespace scanner
