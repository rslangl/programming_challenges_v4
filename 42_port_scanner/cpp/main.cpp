#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <optional>
#include <ranges>
#include <regex>
#include <string_view>
#include <unistd.h>
#include <vector>

// class HostConnection {
// public:
//   HostConnection() {
//     this->_socket = socket(AF_INET, SOCK_STREAM, 0);
//     this->_address.sin_family = AF_INET;
//   }
//

namespace scanner {

const std::regex ipv4_regex("^(((?!25?[6-9])[12]\\d|[1-9])?\\d\\.?\\b){4}$");

enum class protocol { TCP, UDP };

const std::array<std::pair<std::string_view, protocol>, 2> protocol_map{
    {{"TCP", protocol::TCP}, {"UDP", protocol::UDP}}};

auto tokenize(const std::string input) -> std::vector<std::string> {
  std::vector<std::string> tokens{};

  for (auto subrange : std::views::split(input, ',')) {
    tokens.push_back(std::string(subrange.begin(), subrange.end()));
  }

  return tokens;
}

auto ports(const char *portarg) -> std::optional<std::vector<uint16_t>> {
  std::vector<uint16_t> ports{};
  std::string input = std::string(portarg);

  std::vector<std::string> tokens = tokenize(input);
  std::for_each(tokens.begin(), tokens.end(),
                [&ports](const auto &token) -> void {
                  unsigned int port_val{};

                  auto [ptr, ec] = std::from_chars(
                      token.data(), token.data() + token.size(), port_val, 10);

                  if (ec != std::errc{} || port_val < 0 || port_val > 65535) {
                    std::cerr << "ERROR: Invalid numeric input '" << port_val
                              << "', ignoring\n";
                  } else {
                    ports.push_back(static_cast<uint16_t>(port_val));
                  }
                });

  if (ports.empty()) {
    std::cerr << "ERROR: Port list is empty\n";
    return std::nullopt;
  }

  return ports;
}

auto hosts(const char *hostarg) -> std::optional<std::vector<std::string>> {
  std::vector<std::string> hosts{};
  std::string input = std::string(hostarg);

  std::vector<std::string> tokens = tokenize(input);
  std::for_each(tokens.begin(), tokens.end(),
                [&hosts](const auto &token) -> void {
                  std::string host_val{};

                  if (std::regex_match(token, ipv4_regex)) {
                    host_val = token;
                    hosts.push_back(host_val);
                  } else {
                    std::cerr << "ERROR: Invalid host IPv4 addrress '" << token
                              << "', ignoring\n";
                  }
                });

  if (hosts.empty()) {
    std::cerr << "ERROR: Host list is empty\n";
    return std::nullopt;
  }

  return hosts;
}

auto set_protocol(char *protocolarg) -> std::optional<protocol> {
  std::string input = std::string(protocolarg);

  for (auto [name, prot] : protocol_map) {
    if (name == input) {
      return prot;
    }
  }

  return std::nullopt;
}
} // namespace scanner

auto print_help() -> void {
  std::cout << "Usage: " << '\n'
            << "  -p  Port(s) to scan" << '\n'
            << "  -h  Target host" << '\n'
            << "  -t  Transmission protocl (UDP/TCP)" << '\n';
}

auto main(int argc, char *argv[]) -> int {

  int opt{};

  std::vector<std::string> hosts;
  std::vector<uint16_t> ports;
  scanner::protocol protocol;

  // p = port
  // h = host
  // t = transmission protocl (TCP/UDP)
  // h = help
  while ((opt = getopt(argc, argv, "p:h:t:h")) != -1) {
    switch (opt) {
    case 'p':
      if (auto p = scanner::ports(optarg)) {
        ports = *p;
      } else {
        std::cerr << "ERROR: Invalid port input: " << optarg << '\n';
        return -1;
      }
      continue;
    case 'h':
      if (auto h = scanner::hosts(optarg)) {
        hosts = *h;
      } else {
        std::cerr << "ERROR: Minimum one host input is required\n";
        return -1;
      }
      continue;
    case 't':
      if (auto p = scanner::set_protocol(optarg)) {
        protocol = *p;
      } else {
        std::cerr << "ERROR: Invalid protocol selection: " << optarg << '\n';
        return -1;
      }
      continue;
    default:
      print_help();
      return 1;
    }
  }

  std::cout << "DEBUG: Ports:" << '\n';
  for (auto &el : ports) {
    std::cout << static_cast<int>(el) << '\n';
  }

  std::cout << "DEBUG: Hosts:" << '\n';
  for (auto &el : hosts) {
    std::cout << el << '\n';
  }

  return 0;
}
