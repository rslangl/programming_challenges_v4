#include <getopt.h>
#include <iostream>
#include <vector>

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
  std::vector<std::string> ports{};
  std::string protocol;

  // p = port
  // r = remote host
  // t = transmission protocl (TCP/UDP)
  // h = help
  while ((opt = getopt(argc, argv, "p:r:h")) != -1) {
    switch (opt) {
    case 'p':
      if (const auto p = scanner::ports_from_input(optarg); p.has_value()) {
        ports = *p;
      }
      break;
    case 'r':
      if (auto h = scanner::hosts_from_input(optarg); h.has_value()) {
        hosts = *h;
      } else {
        std::cerr << h.error() << '\n';
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

  if (const auto sr = scanner::scan(hosts, ports); sr.has_value()) {
    for (const auto &[host, port_list] : *sr) {
      std::cout << "Scan report for host: " << host << '\n';
      std::cout << "  Port\tState" << '\n';
      for (const auto &[port, state] : port_list) {
        std::cout << "  " << port << "\t" << state << '\n';
      }
    }
    std::cout << '\n';
  }

  return 0;
}
