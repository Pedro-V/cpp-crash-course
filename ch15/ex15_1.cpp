#include <string>
#include <string_view>
#include <map>

constexpr char pos_a{ 97 }, pos_z{ 122 };
constexpr bool within_az(char x) { return pos_a <= x && pos_z >= x; };

constexpr char to_upper(char x) {
  if (within_az(x)) {
    return x - 32;
  }
  return x;
}

struct AlphaHistogram {
  void ingest(std::string_view str) {
    for (auto c : str) {
      counts[to_upper(c)]++;
    }
  }
  void print() const {
    for (auto [c, n] : counts) {
      printf("%c: ", c);
      while (n--) printf("*");
      printf("\n");
    }
  }
private:
  std::map<char, size_t> counts;
};

int main(int argc, char** argv) {
  AlphaHistogram hist;
  for (size_t i{ 1 }; i < argc; i++) {
    hist.ingest(argv[i]);
  }
  hist.print();
}
