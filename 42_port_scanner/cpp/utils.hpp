#pragma once

#include <cstdint>
#include <expected>
#include <iostream>
#include <ranges>
#include <regex>
#include <vector>

namespace scanner {

auto ports_from_input(const char *portarg)
    -> std::expected<std::vector<std::string>, std::string>;

auto hosts_from_input(const char *hostarg)
    -> std::expected<std::vector<std::string>, std::string>;

} // namespace scanner
