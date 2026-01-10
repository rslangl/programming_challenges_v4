#pragma once

#include <cstdint>
#include <cstring>
#include <expected>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

namespace scanner {

auto scan(const std::vector<std::string> hosts,
          const std::vector<std::string> ports)
    -> std::expected<
        std::map<std::string, std::vector<std::pair<std::string, std::string>>>,
        std::string>;

} // namespace scanner
