// ---------------------------------------------------------------------------------
// Strings.

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <string>

// ---------------------------------------------------------------------------------
// std::string

// The STL provides a class template basic_string which can be specialized as
// string (ascii), wstring (implementation's locale), u16string (UTF-16) and
// u32string (UTF-32).

// Constructing

TEST_CASE("std::string supports constructing") {
  SECTION("empty strings") {
    std::string cheese;
    REQUIRE(cheese.empty());
  }

  SECTION("repeated characters") {
    std::string roadside_assistance(3, 'A');
    REQUIRE(roadside_assistance == "AAA");
  }
}

TEST_CASE("std::string supports constructing substrings") {
  auto word = "gobbledygook";
  REQUIRE(std::string(word) == "gobbledygook");
  REQUIRE(std::string(word, 6) == "gobble");
}

TEST_CASE("std::string supports") {
  std::string word("catawampus");

  SECTION("copy constructing") {
    REQUIRE(std::string(word) == "catawampus");
  }

  SECTION("move constructing") {
    REQUIRE(std::string(move(word)) == "catawampus");
    REQUIRE(word.empty());
  }
  
  SECTION("constructing from substrings") {
    REQUIRE(std::string(word, 0, 3) == "cat");
    REQUIRE(std::string(word, 4) == "wampus");
  }
}


TEST_CASE("constructing a string with") {
  SECTION("std::string(char*) stops at embedded nulls") {
    std::string str("idioglossia\0ellohay!");
    REQUIRE(str.length() == 11);
  }

  SECTION("operator \"\"s incorporates embedded nulls") {
    using namespace std::string_literals;
    auto str_lit = "idioglossia\0ellohay!"s;
    REQUIRE(str_lit.length() == 20);
  }
}

// String Storage and Small String Optimizations (SSO): std::string storage is
// very similar to std::vector's.
// Since many strings are small, an additional optimization is to store them in
// the object's storage, rather than in dynamic storage.

// Element and iterator access

TEST_CASE("string's c_str method makes null-terminated char* ") {
  // Many of std::vector methods are available to std::string.
  std::string word("horripilation");
  auto as_cstr = word.c_str();
  REQUIRE(as_cstr[0] == 'h');
  REQUIRE(as_cstr[1] == 'o');
  REQUIRE(as_cstr[11] == 'o');
  REQUIRE(as_cstr[12] == 'n');
  REQUIRE(as_cstr[13] == '\0');
}

// String comparisons 

TEST_CASE("std::string supports comparison with") {
  using namespace std::literals::string_literals;
  std::string word("allusion");
  SECTION("operator== and !=") {
    REQUIRE(word == "allusion");
    REQUIRE(word == "allusion"s);
    REQUIRE(word != "Allusion"s);
    REQUIRE(word != "illusion"s);
    REQUIRE_FALSE(word == "illusion"s);
  }
  SECTION("operator<") {
    REQUIRE(word < "illusion");
    REQUIRE(word < "illusion"s);
    REQUIRE(word > "Illusion"s);
  }
}

// Manipulating elements: adding

TEST_CASE("std::string supports appending with") {
  std::string word("butt");
  SECTION("push_back") {
    word.push_back('e');
    REQUIRE(word == "butte");
  }
  SECTION("operator+=") {
    word += "erfinger";
    REQUIRE(word == "butterfinger");
  }
  SECTION("append char") {
    word.append(1, 's');
    REQUIRE(word == "butts");
  }
  SECTION("append char*") {
    word.append("stockings", 5);
    REQUIRE(word == "buttstock");
  }
  SECTION("append half-open range") {
    std::string other("onomatopoeia");
    word.append(other.begin(), other.begin() + 2);
    REQUIRE(word == "button");
  }
}

// Manipulating elements: removing

TEST_CASE("std::string support removal with") {
  std::string word("therein");
  SECTION("pop_back") {
    word.pop_back();
    word.pop_back();
    REQUIRE(word == "there");
  }
  SECTION("clear") {
    word.clear();
    REQUIRE(word.empty());
  }
  SECTION("erase using half-open range") {
    word.erase(word.begin(), word.begin() + 3);
    REQUIRE(word == "rein");
  }
  SECTION("erase using an index and length") {
    word.erase(5, 2);
    REQUIRE(word == "there");
  }
}

// Manipulating elements: additional operations

TEST_CASE("std::string replace works with") {
  std::string word("substitution");
  SECTION("a range and a char*") {
    word.replace(word.begin()+9, word.end(), "e");
    REQUIRE(word == "substitute");
  }
  SECTION("two ranges") {
    std::string other("innuendo");
    word.replace(word.begin(), word.begin()+3, other.begin(), other.begin()+2);
    REQUIRE(word == "institution");
  }
  SECTION("an index/length and a string") {
    std::string other("vers");
    word.replace(3, 6, other);
    REQUIRE(word == "subversion");
  }
}

TEST_CASE("std::string resize") {
  std::string word("shamp");
  SECTION("can remove elements") {
    word.resize(4);
    REQUIRE(word == "sham");
  }
  SECTION("can add elements") {
    word.resize(7, 'o');
    REQUIRE(word == "shampoo");
  }
}

TEST_CASE("std::string substr with") {
  std::string word("hobbits");
  SECTION("no arguments copies the string") {
    REQUIRE(word.substr() == "hobbits");
  }
  SECTION("position takes the remainder") {
    REQUIRE(word.substr(3) == "bits");
  }
  SECTION("position/index takes a substring") {
    REQUIRE(word.substr(3, 3) == "bit");
    REQUIRE(word == "hobbits");
  }
}

// Search

TEST_CASE("std::string find") {
  using namespace std::literals::string_literals;
  std::string word("pizzazz");
  SECTION("locates substrings from strings") {
    REQUIRE(word.find("zz"s) == 2);     // pi(z)zazz
  }
  SECTION("accepts a position argument") {
    REQUIRE(word.find("zz"s, 3) == 5);  // pizza(z)z
  }
  SECTION("locates substrings from char*") {
    REQUIRE(word.find("zaz") == 3);     // piz(z)azz
  }
  SECTION("returns npos when not found") {
    REQUIRE(word.find('x') == std::string::npos);
  }
}

TEST_CASE("std::string rfind") {
  using namespace std::literals::string_literals;
  std::string word("pizzazz");
  SECTION("locates substrings from strings") {
    REQUIRE(word.rfind("zz"s) == 5);    // pizza(z)z
  }
  SECTION("accepts a position argument") {
    REQUIRE(word.rfind("zz"s, 3) == 2); // pi(z)zazz
  }
  SECTION("locates substrings from char*") {
    REQUIRE(word.rfind("zaz") == 3);    // piz(z)azz
  }
  SECTION("returns npos when not found") {
    REQUIRE(word.rfind('x') == std::string::npos);
  }
}

TEST_CASE("std::string find_first_of") {
  using namespace std::literals::string_literals;
  std::string sentence("I am a Zizzer-Zazzer-Zuzz as you can plainly see.");
  SECTION("locates characters within another string") {
    REQUIRE(sentence.find_first_of("Zz"s) == 7); // (Z)izzer
  }
  SECTION("accepts a position argument") {
    REQUIRE(sentence.find_first_of("Zz"s, 11) == 14); // (Z)azzer
  }
  SECTION("returns npos when not found") {
    REQUIRE(sentence.find_first_of("Xx"s) == std::string::npos);
  }
}

TEST_CASE("std::string find_first_of variations") {
  using namespace std::literals::string_literals;
  std::string sentence("I am a Zizzer-Zazzer-Zuzz as you can plainly see.");
  SECTION("find_last_of finds last element within another string") {
    REQUIRE(sentence.find_last_of("Zz"s) == 24);  //Zuz(z)
  }
  SECTION("find_first_not_of finds first element not within another string") {
    REQUIRE(sentence.find_first_not_of(" -IZaeimrz"s) == 22); // Z(u)zz
  }
  SECTION("find_last_not_of finds last element not within another string") {
    REQUIRE(sentence.find_last_not_of(" .es"s) == 43); // plainl(y)
  }
}

// Numeric conversions

TEST_CASE("STL string conversion function") {
  using namespace std::literals::string_literals;
  SECTION("to_string") {
    REQUIRE("8675309"s == std::to_string(8675309));
  }
  SECTION("to_wstring") {
    REQUIRE(L"109951.162778"s == std::to_wstring(109951.162778));
  }
  SECTION("stoi") {
    REQUIRE(std::stoi("8675309"s) == 8675309);
  }
  SECTION("stoi out of range") {
    REQUIRE_THROWS_AS(std::stoi("1099511627776"s), std::out_of_range);
  }
  SECTION("stoul with all valid characters") {
    size_t last_character{};
    const auto result = std::stoul("0xD3C34C3D"s, &last_character, 16);
    REQUIRE(result == 0xD3C34C3D);
    REQUIRE(last_character == 10);
  }
  SECTION("stoul") {
    size_t last_character{};
    const auto result = std::stoul("42six"s, &last_character);
    REQUIRE(result == 42);
    REQUIRE(last_character == 2);
  }
  SECTION("stod") {
    REQUIRE(std::stod("2.7182818"s) == Approx(2.7182818));
  }
}

// ---------------------------------------------------------------------------------
// String view: drop-in replacement for const string&.
// Its main usage is when passing a string literal as a parameter for a
// function. If the parameter is of type const string&, a std::string is
// constructed, if it's of type std::string_view, a string_view is constructed,
// which is less expensive, since it's just a pointer and a length.

#include <string_view>

TEST_CASE("std::string_view supports") {
  SECTION("default construction") {
    std::string_view view;
    REQUIRE(view.data() == nullptr);
    REQUIRE(view.size() == 0);
    REQUIRE(view.empty());
  }
  SECTION("construction from a string") {
    std::string word("sacrosanct");
    std::string_view view(word);
    REQUIRE(view == "sacrosanct");
  }
  SECTION("construction from a C-string") {
    auto word = "viewership";
    std::string_view view(word);
    REQUIRE(view == "viewership");
  }
  SECTION("construction from C-string and length") {
    auto word = "viewership";
    std::string_view view(word, 4);
    REQUIRE(view == "view");
  }
}

TEST_CASE("std::string_view is modifiable with") {
  // All operations supported by std::string are supported by std::string_view.
  std::string_view view("previewing");
  SECTION("remove_prefix") {
    view.remove_prefix(3);
    REQUIRE(view == "viewing");
  }
  SECTION("remove_suffix") {
    view.remove_suffix(3);
    REQUIRE(view == "preview");
  }
}

size_t count_vees(std::string_view my_view) {
  size_t result{};
  for (auto letter : my_view) {
    if (letter == 'v') result++;
  }
  return result;
}

// ---------------------------------------------------------------------------------
// Regex: Strings that define search patterns.

#include <regex>

TEST_CASE("std::basic_regex constructs from a string literal") {
  std::regex zip_regex{ R"((\w{2})?(\d{5})(-\d{4})?)" };
  // Gets the count of groups.
  REQUIRE(zip_regex.mark_count() == 3);
}

TEST_CASE("std::sub_match") {
  std::regex regex{ R"((\w{2})(\d{5})(-\d{4})?)" };
  // std::smatch is a std::match_results for std::string. It stores additional
  // information about the match.
  std::smatch results;
  SECTION("returns true given matching string") {
    std::string zip("NJ07936-3173");
    const auto matched = std::regex_match(zip, results, regex);
    REQUIRE(matched);
    // std::match_results contain std::sub_matches
    REQUIRE(results[0] == "NJ07936-3173");
    REQUIRE(results[1] == "NJ");
    REQUIRE(results[2] == "07936");
    REQUIRE(results[3] == "-3173");
  }
  SECTION("returns false given non-matching string") {
    std::string zip("Iomega Zip 100");
    const auto matched = std::regex_match(zip, results, regex);
    REQUIRE_FALSE(matched);
  }
}

TEST_CASE("when only part of a string matches a regex, std::regex_") {
  std::regex regex{ R"((\w{2})(\d{5})(-\d{4})?)" };
  std::string sentence("The string NJ07936-3173 is a ZIP Code.");
  SECTION("match returns false") {
    REQUIRE_FALSE(std::regex_match(sentence, regex));
  }
  SECTION("search returns true") {
    REQUIRE(std::regex_search(sentence, regex));
  }
}

TEST_CASE("std::regex_replace") {
  std::regex regex{ R"([aeiou])" };
  std::string phrase("queueing and cooeeing in eutopia");
  const auto result = std::regex_replace(phrase, regex, "_");
  REQUIRE(result == "q_____ng _nd c_____ng _n __t_p__");
}

// ---------------------------------------------------------------------------------
// Boost String Algorithms
// TODO: cover this section. On first read, I found it too complicated and
// badly-explained.

// ---------------------------------------------------------------------------------
// Boost Tokenizer

#include <boost/tokenizer.hpp>
#include <string>

TEST_CASE("boost::tokenizer splits token-delimited strings") {
  std::string palindrome("A man, a plan, a canal, Panama!");
  boost::char_separator<char> comma{ "," };
  boost::tokenizer<boost::char_separator<char>> tokens{ palindrome, comma };
  auto itr = tokens.begin();
  REQUIRE(*itr == "A man");
  itr++;
  REQUIRE(*itr == " a plan");
  itr++;
  REQUIRE(*itr == " a canal");
  itr++;
  REQUIRE(*itr == " Panama!");
}

// ---------------------------------------------------------------------------------
// Exercises

#include <cctype>

// Exercise 15-1
// Implemented in ex15_1.cpp

// Rest of exercises are implemented as functions for simplicity.

// Exercise 15-2

bool is_palindrome(std::string_view str) {
    for (size_t i = 0, j = str.length() - 1; i < j; ++i, --j) {
        while (i < j && !std::isalnum(str[i])) {
            ++i;
        }
        while (i < j && !std::isalnum(str[j])) {
            --j;
        }
        if (std::tolower(str[i]) != std::tolower(str[j])) {
            return false;
        }
    }

    return true;
}

TEST_CASE("is_palindrome") {
  using namespace std::literals::string_literals;
  SECTION("returns true for palindromes") {
    REQUIRE(is_palindrome("A man, a plan, a canal, Panama!"s));
  }
  SECTION("returns false for non-palindromes") {
    REQUIRE_FALSE(is_palindrome("A woman, a plan, a canal, Panama!"s));
  }
}

// Exercise 15-3

bool is_vowel(char c) {
  std::string vowels("aeiouAEIOU");
  return vowels.find(c) != std::string::npos;
}

int vowel_count(std::string_view str) {
  int count{};
  for (auto c : str) {
    if (is_vowel(c)) count++;
  }
  return count;
}

TEST_CASE("vowel_count") {
  SECTION("identifies no vowels") {
    std::string word("bdrfhn");
    REQUIRE(vowel_count(word) == 0);
  }
  SECTION("identifies single vowel") {
    std::string single_vowel("a");
    REQUIRE(vowel_count(single_vowel) == 1);
  }
  SECTION("identifies only vowels") {
    std::string only_vowels("aaeiiouu");
    REQUIRE(vowel_count(only_vowels) == 8);
  }
  SECTION("identifies mixed vowels and consonants") {
    std::string mixed("Normal phrase");
    REQUIRE(vowel_count(mixed) == 4);
  }
}

// TODO
// Exercie 15-4
