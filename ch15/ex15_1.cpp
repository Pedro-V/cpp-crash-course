#include <string>
#include <string_view>
#include <map>
#include <cctype>

struct AlphaHistogram {
  void ingest(std::string_view str) {
    for (auto c : str) {
      if (std::isalpha(c)) counts[std::toupper(c)]++;
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
