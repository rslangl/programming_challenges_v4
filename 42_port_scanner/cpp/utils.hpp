#pragma once

#include "port_state.hpp"
#include "protocol.hpp"
#include <cstdint>
#include <iostream>
#include <optional>
#include <ranges>
#include <regex>
#include <vector>

namespace scanner {

// TODO: implement this struct in favor of nullopt, or use std::expected for
// C++23
//
// struct Result { T value; std::string error; bool ok;
// }
//
// static Result<T> success(T val) { return {std::move(val), {}, true}; }
// static Result<T> failure(std::string msg) { return {T{}, std::move(msg),
// false}; }
//
// Usage:
// Result<std::tuple<int, struct sockaddr_storage>>
// scanner::make_socket(std::string host, const uint16_t port, protocol
// protocol);
//
// auto res = scanner::make_socket(host, port, protocol::TCP);
// if(!res.ok) {
//  std::cerr << "ERROR: " << res.error << '\n';
// } else {
// // Use res.value
// }

auto ports_from_input(const char *portarg)
    -> std::optional<std::vector<uint16_t>>;

auto hosts_from_input(const char *hostarg)
    -> std::optional<std::vector<std::string>>;

auto protocol_from_input(const char *protocolarg) -> std::optional<protocol>;

auto print(uint16_t port, port_state state) -> void;

} // namespace scanner
