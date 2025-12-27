#include <algorithm>
#include <concepts>
#include <cstdint>
#include <iostream>
#include <ranges>
#include <regex>
#include <string_view>
#include <unistd.h>
#include <vector>

const std::regex ipv4_regex("^(((?!25?[6-9])[12]\\d|[1-9])?\\d\\.?\\b){4}$");

// static auto is_ipv4(const std::string host) -> bool {
//   constexpr std::regex host_regex(ipv4_regex);
// }

auto print_help() -> void {
  std::cout << "Usage: " << '\n'
            << "  -p  Port(s) to scan" << '\n'
            << "  -h  Target host" << '\n'
            << "  -t  Transmission protocl (UDP/TCP)" << '\n';
}

class ScanJob {
public:
  ScanJob() : _ports{}, _hosts{} {}

  auto add_ports(char *portarg) -> void {

    std::string input = std::string(portarg);
    std::vector<std::string> tokens = tokenize(input);

    std::for_each(
        tokens.begin(), tokens.end(), [this](const auto &token) -> void {
          unsigned int port_val{};

          auto [ptr, ec] = std::from_chars(
              token.data(), token.data() + token.size(), port_val, 10);

          if (ec != std::errc{} || port_val < 0 || port_val > 255) {
            std::cerr << "ERROR: Invalid numeric input: " << port_val << '\n';
          }

          this->_ports.push_back(static_cast<uint8_t>(port_val));
        });
  }

  auto add_hosts(char *hostarg) -> void {

    std::string input = std::string(hostarg);
    std::vector<std::string> tokens = tokenize(input);

    std::for_each(tokens.begin(), tokens.end(),
                  [this](const auto &token) -> void {
                    std::string host_val{};

                    if (std::regex_match(token, ipv4_regex)) {
                      host_val = token;
                      this->_hosts.push_back(host_val);
                    } else {
                      std::cerr << "ERROR: Invalid host IPv4 addrress" << '\n';
                    }
                  });
  }

  auto add_protocol(char *protocolarg)
      -> void { // TODO: act as proper error when invalid
    std::string p = protocolarg;

    if (p != "TCP" || p != "UDP") {
      std::cerr << "ERROR: Invalid protocol argument" << '\n';
    }

    _protocol = p;
  }

  auto ports() -> const std::vector<uint8_t> & { return _ports; }

  auto hosts() -> const std::vector<std::string> & { return _hosts; }

  ~ScanJob() {}

private:
  auto tokenize(std::string input) -> std::vector<std::string> {
    std::vector<std::string> tokens{};

    for (auto subrange : std::views::split(input, ',')) {
      tokens.push_back(std::string(subrange.begin(), subrange.end()));
    }

    return tokens;
  }

  std::vector<uint8_t> _ports;
  std::vector<std::string> _hosts;
  std::string _protocol;
};

auto main(int argc, char *argv[]) -> int {

  int opt{};
  ScanJob job{};

  // p = port
  // h = host
  // t = transmission protocl (TCP/UDP)
  // h = help
  while ((opt = getopt(argc, argv, "p:h:t:h")) != -1) {
    switch (opt) {
    case 'p':
      job.add_ports(optarg);
      continue;
    case 'h':
      job.add_hosts(optarg);
      continue;
    case 't':
      job.add_protocol(optarg);
      continue;
    default:
      print_help();
      return 1;
    }
  }

  std::cout << "DEBUG: Ports:" << '\n';
  for (auto &el : job.ports()) {
    std::cout << static_cast<int>(el) << '\n';
  }

  std::cout << "DEBUG: Hosts:" << '\n';
  for (auto &el : job.hosts()) {
    std::cout << el << '\n';
  }

  if (job.ports().empty()) {
    std::cerr << "ERROR: Minimum one port input is required" << '\n';
    return 1;
  }

  if (job.hosts().empty()) {
    std::cerr << "ERROR: Minimum one host input is required" << '\n';
    return 1;
  }

  return 0;
}
