#pragma once

#include "port_state.hpp"
#include <cstdint>
#include <iostream>
#include <optional>
#include <ranges>
#include <regex>
#include <vector>

namespace scanner {

auto ports_from_input(const char *portarg)
    -> std::optional<std::vector<uint16_t>>;

auto hosts_from_input(const char *hostarg)
    -> std::optional<std::vector<std::string>>;

// auto protocol_from_input(char *protocolarg) -> std::optional<protocol>;

auto print(uint16_t port, port_state state) -> void;

} // namespace scanner
