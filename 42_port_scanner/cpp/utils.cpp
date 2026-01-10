#include "utils.hpp"

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
    -> std::expected<std::vector<std::string>, std::string> {

  std::vector<std::string> ports{};
  std::string input = std::string(portarg);

  std::vector<std::string> tokens = tokenize(input);

  for (const auto &token : tokens) {
    std::size_t pos{};
    const int val{std::stoi(token.c_str(), &pos)};

    if (val < 0 || val > 65535) {
      return std::unexpected(std::format("invalid input value for port: {}",
                                         static_cast<size_t>(val)));
    }

    ports.push_back(token);
  }

  return ports;
}

auto hosts_from_input(const char *hostarg)
    -> std::expected<std::vector<std::string>, std::string> {

  std::vector<std::string> hosts{};
  std::string input = std::string(hostarg);

  std::vector<std::string> tokens = tokenize(input);

  static const std::regex ipv4_regex(
      "^(((?!25?[6-9])[12]\\d|[1-9])?\\d\\.?\\b){4}$");
  // static const std::regex url_regex(
  //     "^https?:\\/\\/(?:www\\.)?[-a-zA-Z0-9@:%.\\+~#=]{1,"
  //     "256}\\.[a-zA-Z0-9()]{1,6}\\b(?:[-a-zA-Z0-9()@:%\\+.~#?&\\/=]*)$");
  //
  for (const auto &token : tokens) {
    if (!std::regex_match(token, ipv4_regex)) {
      return std::unexpected(
          std::format("invalid input value for host: {}", token.c_str()));
    }

    hosts.push_back(token);
  }

  return hosts;
}

} // namespace scanner
