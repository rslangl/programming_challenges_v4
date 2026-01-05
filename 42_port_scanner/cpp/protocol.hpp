#pragma once

#include <array>
#include <string_view>

namespace scanner {

enum class protocol { TCP, UDP };

const std::array<std::pair<protocol, std::string_view>, 2> protocol_map{
    {{protocol::TCP, "TCP"}, {protocol::UDP, "UDP"}}};

} // namespace scanner
