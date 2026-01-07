#include <getopt.h>
#include <iostream>
#include <vector>

#include "protocol.hpp"
#include "scanner.hpp"
#include "utils.hpp"

static auto print_help() -> void {
  std::cout << "Usage: " << '\n'
            << "  -p  Port(s) to scan" << '\n'
            << "  -r  Remote host" << '\n'
            << "  -t  Transmission protocol (UDP/TCP)" << '\n';
}

auto main(int argc, char *argv[]) -> int {

  int opt{};

  std::vector<std::string> hosts{};
  std::vector<uint16_t> ports{};
  scanner::protocol protocol;

  // p = port
  // r = remote host
  // t = transmission protocl (TCP/UDP)
  // h = help
  while ((opt = getopt(argc, argv, "p:r:t:h")) != -1) {
    switch (opt) {
    case 'p':
      if (const auto p = scanner::ports_from_input(optarg); p.has_value()) {
        ports = *p;
      }
      break;
    case 'r':
      if (auto h = scanner::hosts_from_input(optarg); h.has_value()) {
        hosts = *h;
      }
      break;
    case 't':
      if (auto p = scanner::protocol_from_input(optarg); p.has_value()) {
        protocol = *p;
      }
      break;
    case '?':
      print_help();
      return 1;
    default:
      print_help();
      return 1;
    }
  }

  if (ports.empty()) {
    std::cerr << "ERROR: Invalid port input\n";
    print_help();
    return 1;
  }

  if (hosts.empty()) {
    std::cerr << "ERROR: Minimum one host input is required\n";
    print_help();
    return 1;
  }

  scanner::scan(hosts, ports, protocol);

  return 0;
}
