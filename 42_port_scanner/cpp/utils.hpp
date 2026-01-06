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

enum class scanner_error { invalid_input, socket_error, connection_error };

auto ports_from_input(const char *portarg)
    -> std::expected<std::vector<uint16_t>, scanner_error>;

auto hosts_from_input(const char *hostarg)
    -> std::expected<std::vector<std::string>, scanner_error>;

auto protocol_from_input(const char *protocolarg)
    -> std::expected<protocol, scanner_error>;

auto print(uint16_t port, port_state state) -> void;

} // namespace scanner
