#pragma once

#include <array>
#include <string_view>

namespace scanner {

enum class protocol { TCP, UDP, COUNT };

namespace {
// TODO: consider using a LUT instead
// constexpr std::array<std::pair<protocol, std::string_view>, 2> protocol_map{{
//     {protocol::TCP, "TCP"},
//     {protocol::UDP, "UDP"},
// }};

inline constexpr std::array<std::string_view,
                            static_cast<size_t>(protocol::COUNT)>
    protocols = {"TCP", "UDP"};
} // namespace

auto constexpr protocol_stringrep(protocol prot) -> std::string_view {
  return protocols[static_cast<size_t>(prot)];
};

static_assert(static_cast<size_t>(protocol::COUNT) == protocols.size(),
              "Protocol enum and protocol_stringrep are out of sync");

} // namespace scanner
