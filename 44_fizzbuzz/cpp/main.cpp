#include <iostream>
#include <charconv>
//#include <cstdio>

using namespace std;

auto main(int argc, char **argv) -> int {

  if (argc != 2) {
    std::cerr << "error: one numeric input is required\n";
    return 1;
  }

  int limit{};
  auto [ptr, ec] = std::from_chars(argv[1], argv[1] + sizeof(argv[1]), limit);

  if (ec == std::errc()) {

    for(int n = 0; n < limit; ++n) {
      if (n % 15 == 0) {
        std::cout << "fizzbuzz\n";
      } else if (n % 5 == 0) {
        std::cout << "fizz\n";
      } else if (n % 3 == 0) {
        std::cout << "buzz\n";
      } else {
        std::cout << n << "\n";
      }
    }

  } else {
    std::cout << "error: input not a valid number\n";
    return 1;
  }

  return 0;
}
