#pragma once

#include <array>
#include <string_view>
#include <utility>

namespace scanner {

enum class port_state { CLOSED, OPEN, UNKNOWN, COUNT };

// By using an std::array, we achieve a fixed mapping at compile time, where
// strings are read-only, and no branches, loops, or allocations are used. This
// is desired for efficiency as this string representation might be called many
// times during prettyprinting of the port state map for all hosts
namespace {
inline constexpr std::array<std::string_view,
                            static_cast<size_t>(port_state::COUNT)>
    port_states = {"CLOSED", "OPEN", "UNKNOWN"};
}

auto constexpr port_stringrep(port_state state) -> std::string_view {
  return port_states[static_cast<size_t>(state)];
};

static_assert(static_cast<size_t>(port_state::COUNT) == port_states.size(),
              "Port state enum and port_stringrep are out of sync");

} // namespace scanner
