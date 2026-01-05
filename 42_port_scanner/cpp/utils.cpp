#include "utils.hpp"
#include "port_state.hpp"

namespace scanner {
namespace {
auto tokenize(const std::string input) -> std::vector<std::string> {
  std::vector<std::string> tokens{};

  for (auto subrange : std::views::split(input, ',')) {
    tokens.push_back(std::string(subrange.begin(), subrange.end()));
  }

  return tokens;
}
} // namespace

auto ports_from_input(const char *portarg)
    -> std::optional<std::vector<uint16_t>> {

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

auto hosts_from_input(const char *hostarg)
    -> std::optional<std::vector<std::string>> {
  std::vector<std::string> hosts{};
  std::string input = std::string(hostarg);

  std::vector<std::string> tokens = tokenize(input);

  static const std::regex ipv4_regex(
      "^(((?!25?[6-9])[12]\\d|[1-9])?\\d\\.?\\b){4}$");

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

// auto protocol_from_input(char *protocolarg) -> std::optional<protocol> {
//   std::string input = std::string(protocolarg);
//
//   for (auto [name, prot] : protocol_map) {
//     if (name == input) {
//       return prot;
//     }
//   }
//
//   return std::nullopt;
// }
//
auto print(uint16_t port, port_state state) -> void {
  std::ostringstream oss;
  oss << "port=" << port << ";state=" << port_stringrep(state) << '\n';
  std::cout << oss.str();
}

} // namespace scanner
