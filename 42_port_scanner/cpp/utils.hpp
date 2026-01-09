#pragma once

#include "port_state.hpp"
#include "protocol.hpp"
#include <cstdint>
#include <expected>
#include <iostream>
#include <ranges>
#include <regex>
#include <vector>

namespace scanner {

auto ports_from_input(const char *portarg)
    -> std::expected<std::vector<const char *>, std::string>;

auto hosts_from_input(const char *hostarg)
    -> std::expected<std::vector<const char *>, std::string>;

auto protocol_from_input(const char *protocolarg)
    -> std::expected<const char *, std::string>;

} // namespace scanner
