#include <getopt.h>
#include <iostream>
#include <optional>
#include <vector>

#include "protocol.hpp"
#include "scanner.hpp"
#include "utils.hpp"

static auto print_help() -> void {
  std::cout << "Usage: " << '\n'
            << "  -p  Port(s) to scan" << '\n'
            << "  -h  Target host" << '\n'
            << "  -t  Transmission protocl (UDP/TCP)" << '\n';
}

auto main(int argc, char *argv[]) -> int {

  int opt{};

  std::vector<std::string> hosts;
  std::vector<uint16_t> ports;
  // scanner::protocol protocol;

  // p = port
  // h = host
  // t = transmission protocl (TCP/UDP)
  // h = help
  while ((opt = getopt(argc, argv, "p:h:t:h")) != -1) {
    switch (opt) {
    case 'p':
      if (auto p = scanner::ports_from_input(optarg)) {
        ports = *p;
      }
      break;
    case 'h':
      if (auto h = scanner::hosts_from_input(optarg)) {
        hosts = *h;
      }
      break;
    // case 't':
    //   if (auto p = scanner::protocol_from_input(optarg)) {
    //     protocol = *p;
    //   } else {
    //     std::cerr << "ERROR: Invalid protocol selection: " << optarg << '\n';
    //     return -1;
    //   }
    //   continue;
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

  scanner::scan(hosts, ports);

  return 0;
}
