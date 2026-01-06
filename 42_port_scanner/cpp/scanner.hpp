#pragma once

#include "port_state.hpp"
#include "protocol.hpp"
#include "socket.hpp"
#include "utils.hpp"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <vector>

namespace scanner {

/// @brief Runs a probe scan for all ports on all hosts
/// @param hosts    The list of hosts to run scans on
/// @pamam ports    The list of ports to probe
/// @param protocol The port protocol to probe (TCP/UDP)
auto scan(const std::vector<std::string> hosts,
          const std::vector<uint16_t> ports, const protocol protocol) -> void;

} // namespace scanner
