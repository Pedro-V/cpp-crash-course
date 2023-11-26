#include <cstdio>
#include <cstring>
#include <set>

struct CStringComparator {
  bool operator()(const char* s1, const char* s2) const noexcept {
    return std::strcmp(s1, s2) < 0;
  }
};

int main(int argc, char** argv) {
  std::set<const char*, CStringComparator> args;
  for (size_t i{ 1 }; i < argc; i++) {
    args.insert(argv[i]);
  }
  for (const auto& arg : args) {
    printf("%s ", arg);
  }
  printf("\n");
}
