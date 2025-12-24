#include <algorithm>
#include <cstdint>
#include <iostream>
#include <ranges>
#include <string_view>
#include <unistd.h>
#include <vector>

auto print_help() -> void {
  std::cout << "Usage: " << '\n'
            << "  -p  Port(s) to scan" << '\n'
            << "  -h  Target host" << '\n'
            << "  -t  Transmission protocl (UDP/TCP)" << '\n';
}

class ScanJob {
public:
  ScanJob() : _ports{}, _hosts{} {}

  auto add_hosts(char *hostarg) -> void {
    parse_input<std::string>(hostarg, &_hosts);
  }

  auto add_ports(char *portarg) -> void {
    parse_input<uint8_t>(portarg, &_ports);
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
  /**
   *  Validates the user-provided input based on the type of data we expect
   */
  template <class T>
  auto validate(std::string regex, std::vector<T> *data) -> void {}

  /**
   *  Takes the stringified user-provided input and splits it based on given
   * delimiters
   */
  auto tokenize(std::string input) -> std::vector<std::string> {
    std::vector<std::string> tokens{};

    for (auto subrange : std::views::split(input, ',')) {
      tokens.push_back(std::string(subrange.begin(), subrange.end()));
    }

    return tokens;
  }

  /**
   *  Takes the raw user-provided input and parses it
   */
  template <class T> auto parse_input(char *arg, std::vector<T> *data) -> void {
    std::string input = std::string(arg);
    std::vector<std::string> tokens = tokenize(input);

    std::for_each(tokens.begin(), tokens.end(), [data](auto &token) -> void {
      std::cout << "Token: " << token << '\n';
      data->push_back(static_cast<T>(token));
    });
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
      break;
    case 'h':
      job.add_ports(optarg);
      break;
    case 't':
      job.add_protocol(optarg);
      break;
    default:
      print_help();
      return 1;
    }
  }

  std::cout << "Ports:" << '\n';
  for (auto &el : job.ports()) {
    std::cout << el << '\n';
  }

  std::cout << "Hosts:" << '\n';
  for (auto &el : job.hosts()) {
    std::cout << el << '\n';
  }

  return 0;
}
