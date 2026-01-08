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

// TODO: union type containing either result set or error output, use as field
// in target

class target {
public:
  target() : host{}, ports{} {}
  const char *host;
  std::vector<char *> ports;
};

auto scan(const std::vector<const char *> hosts,
          const std::vector<const char *> ports)
    -> std::expected<std::vector<std::string>, std::string>;

} // namespace scanner
