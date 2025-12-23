#include <cstdint>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

auto print_help() -> void {
  std::cout << "Usage: " << '\n'
            << "  -p  Port(s) to scan" << '\n'
            << "  -h  Target host" << '\n'
            << "  -t  Transmission protocl (UDP/TCP)" << '\n';
}

struct scan_job {
  vector<int> ports;
  vector<string> hosts;
  string protocol;
};

auto main(int argc, char *argv[]) -> int {

  int opt{};

  const int protocol_max_len = 3;
  struct scan_job job{};

  // p = port
  // h = host
  // t = transmission protocl (TCP/UDP)
  // h = help
  while ((opt = getopt(argc, argv, "p:h:t:h")) != -1) {
    switch (opt) {
    case 'p':
      std::cout << "Port: " << optarg << '\n';
      while (*optarg) {
        optarg++;
        job.ports.push_back(reinterpret_cast<uintptr_t>(optarg));
      }
      break;
    case 'h':
      std::cout << "Host: " << optarg << '\n';
      while (*optarg) {
        optarg++;
        job.hosts.push_back(string(optarg));
      }
      break;
    case 't':
      std::cout << "Protocol: " << optarg << '\n';
      job.protocol = string(protocol_max_len, *optarg);
      if (job.protocol != "UDP" || job.protocol != "TCP") {
        std::cerr << "ERROR: Illegal protocol" << '\n';
        return 1;
      }
      break;
    default:
      print_help();
      return 1;
    }
  }

  return 0;
}
