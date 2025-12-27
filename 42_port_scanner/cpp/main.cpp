#include <algorithm>
#include <arpa/inet.h>
#include <array>
// #include <bitset>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <optional>
#include <ranges>
#include <regex>
#include <string_view>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

namespace scanner {

// Dislaimer: taken from https://stackoverflow.com/a/36760050, which uses
// lookahead to check if the numbers start with 25 and then doesnt't accept them
// if they continue with numbers between 6 and 9
const std::regex ipv4_regex("^(((?!25?[6-9])[12]\\d|[1-9])?\\d\\.?\\b){4}$");

enum class protocol { TCP, UDP };

const std::array<std::pair<std::string_view, protocol>, 2> protocol_map{
    {{"TCP", protocol::TCP}, {"UDP", protocol::UDP}}};

enum class port_state { CLOSED, OPEN, UNKNOWN };

auto port_state_string(port_state state) -> std::string {
  switch (state) {
  case port_state::UNKNOWN:
    return "UNKNOWN";
  case port_state::CLOSED:
    return "CLOSED";
  case port_state::OPEN:
    return "OPEN";
  }
}

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

auto create_socket(std::string host, const uint16_t port)
    -> std::optional<std::tuple<int, struct sockaddr_storage>> {

  int status, fd;
  struct sockaddr_storage remote;

  if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    std::cerr << "ERROR: Error creating socket\n";
    return std::nullopt;
  }

  // Configure socket
  // fcntl(fd, F_SETFL, SOCK_NONBLOCK);
  // setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
  //
  auto *ipv4 = reinterpret_cast<sockaddr_in *>(&remote);
  ipv4->sin_family = AF_INET;
  ipv4->sin_port = htons(port);

  if ((status = inet_pton(AF_INET, host.data(), &ipv4->sin_addr)) != 1) {
    std::cerr << "ERROR: Could not convert '" << host
              << "' to network notation\n";
    return std::nullopt;
  }

  return std::make_tuple(fd, remote);
}

auto scan(int fd, struct sockaddr *out) -> port_state {
  int status;

  if ((status = connect(fd, out, sizeof(*out))) != 0) {
    std::cerr << "ERROR: Could not connect to host\n";
    return port_state::UNKNOWN;
  }

  // if ((status = send(fd, probe, probe_len, 0)) != 0) {
  //   std::cerr << "ERROR: Could not send probe to host\n";
  //   return port_state::UNKNOWN;
  // }

  return port_state::OPEN;
}

auto print(uint16_t port, port_state state) -> void {
  std::ostringstream oss;
  oss << "port=" << port << ";state=" << port_state_string(state) << '\n';
  std::cout << oss.str();
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

  std::for_each(hosts.begin(), hosts.end(), [&ports](const auto host) -> void {
    std::for_each(ports.begin(), ports.end(), [&host](const auto port) -> void {
      if (auto opt = scanner::create_socket(host, port); opt) {
        auto [fd, remote] = std::move(*opt);

        auto port_state =
            scanner::scan(fd, reinterpret_cast<sockaddr *>(&remote));
        scanner::print(port, port_state);

      } else {
        std::cerr << "ERROR: Could not create network socket for host '" << host
                  << "'\n";
      }
    });
  });

  return 0;
}
