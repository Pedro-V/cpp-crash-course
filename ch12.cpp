// ---------------------------------------------------------------------------------
// Utilities

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

// ---------------------------------------------------------------------------------
// Data Structures

// tribool: bool-like with 3 states. Useful for modelling operations that are
// still pending.

#include <boost/logic/tribool.hpp>

using boost::logic::indeterminate;
boost::logic::tribool t = true, f = false, i = indeterminate;

TEST_CASE("Boost tribool converts to bool") {
  REQUIRE(t);
  REQUIRE_FALSE(f);
  REQUIRE(!f);
  REQUIRE_FALSE(!i);
  REQUIRE(indeterminate(i));
  REQUIRE_FALSE(indeterminate(t));
}

TEST_CASE("Boost tribool supports boolean operations") {
  auto t_or_f = t || f;
  REQUIRE(t_or_f);
  REQUIRE(indeterminate(t && indeterminate));
  REQUIRE(indeterminate(f || indeterminate));
  REQUIRE(indeterminate(!i));
}

TEST_CASE("Boost tribool works nicely with if statements") {
  if (i) FAIL("Indeterminate is true.");
  else if (!i) FAIL("indeterminate is false.");
  else {}
}


// optional: Values that might or might not be present.

#include <optional>

struct TheMatrix {
  TheMatrix(int x) : iteration{ x } {}
  const int iteration;
};

enum Pill { Red, Blue };

std::optional<TheMatrix> take(Pill pill) {
  if (pill == Pill::Blue) return TheMatrix{ 6 };
  return std::nullopt;
}

TEST_CASE("std::optional contains types") {
  if (auto matrix_opt = take(Pill::Blue)) {
    REQUIRE(matrix_opt->iteration == 6);
    auto& matrix = matrix_opt.value();
    REQUIRE(matrix.iteration == 6);
  } else {
    FAIL("The optional evaluated to false.");
  }
}

TEST_CASE("std::optional can be empty") {
  auto matrix_opt = take(Pill::Red);
  if (matrix_opt) FAIL("The Matrix is not empty.");
  REQUIRE_FALSE(matrix_opt.has_value());
}

TEST_CASE("Empty std::optional can fallback to some default") {
  auto matrix_opt = take(Pill::Red);
  auto m = matrix_opt.value_or(TheMatrix{ 7 });
  REQUIRE(m.iteration == 7);
}

TEST_CASE("Other operations over optionals") {
  auto opt_empty = std::optional<int>{};
  auto opt_full = std::optional<int>{ 30 };
  SECTION("operator== returns true if both are empty or both are full and equal") {
    REQUIRE(opt_empty == opt_empty);
    REQUIRE(opt_full == opt_full);
    REQUIRE_FALSE(opt_empty == opt_full);
  }

  SECTION("An empty opt'll always be less than another full opt."
          "If both are full, then usual behavior") {
    REQUIRE(opt_empty < opt_full);
    REQUIRE_FALSE(opt_full < opt_full);
  }

  SECTION("A full opt can be emptied") {
    opt_full.reset();
    REQUIRE_FALSE(opt_full);
  }

  SECTION("Opts can be swapped") {
    opt_empty.swap(opt_full);
    REQUIRE(opt_empty);
    REQUIRE_FALSE(opt_full);
  }
}

// pair: An ordered pair of potentially different types.

#include <utility>

struct Socialite{ const char* birthname; };
struct Valet{ const char* surname; };

Socialite bertie{ "Wilberforcer" };
Valet reginald{ "Jeeves" };

TEST_CASE("std::pair") {
  std::pair<Socialite, Valet> inimitable_duo{ bertie, reginald };

  SECTION("permits access to members") {
    REQUIRE(inimitable_duo.first.birthname == bertie.birthname);
    REQUIRE(inimitable_duo.second.surname == reginald.surname);
  }

  SECTION("works with structured binding") {
    auto& [idle_rich, butler] = inimitable_duo;
    REQUIRE(idle_rich.birthname == bertie.birthname);
    REQUIRE(butler.surname == reginald.surname);
  }
}

// tuple: a generalization of pair.

#include <tuple>

struct Acquaintance{ const char* nickname; };
Acquaintance hildebrand{ "Tuppy" };

TEST_CASE("std::tuple permits access to members with std::get") {
  using Trio = std::tuple<Socialite, Valet, Acquaintance>;
  Trio truculent_trio{ bertie, reginald, hildebrand };

  SECTION("with index") {
    auto& bertie_ref = std::get<0>(truculent_trio);
    REQUIRE(bertie_ref.birthname == bertie.birthname);
  }

  SECTION("with type") {
    auto& tuppy_ref = std::get<Acquaintance>(truculent_trio);
    REQUIRE(tuppy_ref.nickname == hildebrand.nickname);
  }
}

TEST_CASE("std::tuple construction operations") {
  SECTION("make_tuple") {
    auto t1 = std::make_tuple<int, float, char>(3, 9.0f, 'a');
    auto t2 = std::make_tuple(3, 9.0f, 'a');
  }

  SECTION("tuple concatenation") {
    using T = std::tuple<int, char>;
    T t1 = { 10, 'X' }, t2 = { 20, 'F' };

    auto t3 = tuple_cat(t1, t2);
    REQUIRE(std::get<1>(t3) == 'X');
    REQUIRE(std::get<2>(t3) == 20);
  }
}

// any: Stores single values of any type. 

#include <any>

struct EscapeCapsule {
  EscapeCapsule(int x) : weight_kg{ x } {}
  int weight_kg;
};

TEST_CASE("std::any allows us to std::any_cast into a type") {
  std::any hagunemnon;
  // The arguments are passed to the chosen type constructor.
  hagunemnon.emplace<EscapeCapsule>(600);
  auto capsule = std::any_cast<EscapeCapsule>(hagunemnon);
  REQUIRE(capsule.weight_kg == 600);
  REQUIRE_THROWS_AS(std::any_cast<float>(hagunemnon), std::bad_any_cast);
}

// variant: Type-safe unions. Stricter than any.

#include <variant>

struct BugblatterBeast {
  BugblatterBeast() : is_ravenous{ true }, weight_kg{ 20000 } {}
  bool is_ravenous;
  int weight_kg;
};

TEST_CASE("std::variant allows for") {
  // The variant is only default constructible because BugblatterBeast, its
  // first template parameter, also is.
  std::variant<BugblatterBeast, EscapeCapsule> hagunemnon;

  SECTION("checking for the index of the current type with index") {
    REQUIRE(hagunemnon.index() == 0);
  }

  SECTION("set its current type from its list with emplace") {
    hagunemnon.emplace<EscapeCapsule>(600);
    REQUIRE(hagunemnon.index() == 1);
  }

  SECTION("acessing the current type with index/type with std::get and std::get_if") {
    REQUIRE(std::get<0>(hagunemnon).weight_kg == 20000);
    REQUIRE(std::get<BugblatterBeast>(hagunemnon).weight_kg == 20000);
    REQUIRE(std::get_if<BugblatterBeast>(&hagunemnon)->is_ravenous);
  }

  SECTION("applying callable objects without including type information with"
          "std::visit") {
    hagunemnon.emplace<EscapeCapsule>(600);
    auto lbs = std::visit([](auto& x){ return 2.2*x.weight_kg; }, hagunemnon);
    REQUIRE(lbs == 600 * 2.2);
  }

  SECTION("type safety with exceptions when acessing an unset type with std::get") {
    REQUIRE_THROWS_AS(std::get<1>(hagunemnon), std::bad_variant_access);
  }

  SECTION("type safety with the default nullptr when acessing an unset type"
          "with std::get_if") {
    REQUIRE(std::get_if<EscapeCapsule>(&hagunemnon) == nullptr);
  }
}

// ---------------------------------------------------------------------------------
// Date and Time

// Boost DateTime: Gregorian-calendar-based date programming. There are also
// APIs for Posix and Local time.

#include <boost/date_time/gregorian/gregorian.hpp>

TEST_CASE("Invalid boost::gregorian::dates throw exceptions") {
  using boost::gregorian::date;
  using boost::gregorian::bad_day_of_month;

  REQUIRE_THROWS_AS(date(1986, 9, 32), bad_day_of_month);
}

TEST_CASE("Getting the current day") {
  auto d_local = boost::gregorian::day_clock::local_day();
  auto d_univ = boost::gregorian::day_clock::universal_day();
}

TEST_CASE("boost::gregorian::date supports basic calendar functions") {
  boost::gregorian::date d{ 1986, 9, 15 };
  REQUIRE(d.year() == 1986);
  REQUIRE(d.month() == 9);
  REQUIRE(d.day() == 15);
  REQUIRE(d.day_of_year() == 258);
  REQUIRE(d.day_of_week() == boost::date_time::Monday);
}

TEST_CASE("boost::gregorian::date supports calendar arithmetic") {
  boost::gregorian::date d1{ 1986, 9, 15 };
  boost::gregorian::date d2{ 2019, 8, 1 };

  auto duration = d2 - d1;
  REQUIRE(duration.days() == 12008);
}

TEST_CASE("date and date_duration support addition") {
  boost::gregorian::date d1{ 1986, 9, 15 };
  boost::gregorian::date_duration dur{ 12008 };
  auto d2 = d1 + dur;
  REQUIRE(d2 == boost::gregorian::from_string("2019/8/1"));
}

TEST_CASE("boost::gregorian::date supports periods") {
  boost::gregorian::date d1{ 1986, 9, 15 };
  boost::gregorian::date d2{ 2019, 8, 1 };
  boost::gregorian::date_period p{ d1, d2 };
  REQUIRE(p.contains(boost::gregorian::date{ 1987, 10, 27 }));
}

TEST_CASE("Other operations of boost::gregorian::date_period") {
  boost::gregorian::date d1{ 1986, 9, 15 };
  boost::gregorian::date d2{ 2000, 1, 1 };
  boost::gregorian::date d3{ 2002, 1, 1 };
  boost::gregorian::date d4{ 2019, 8, 1 };

  boost::gregorian::date_period p1{ d1, d4 };
  boost::gregorian::date_period p2{ d2, d3 };

  SECTION("First/last day and length of the period") {
    REQUIRE(p1.begin() == d1);
    REQUIRE(p2.last() == (d3 - boost::gregorian::date_duration{ 1 }));
    REQUIRE(p2.length() == boost::gregorian::date_duration{ 731 });
  }

  SECTION("Check for relations between periods and dates") {
    REQUIRE(p1.contains(p2));
    REQUIRE(p1.intersects(p2));

    REQUIRE(p2.is_after(d1));
    REQUIRE(p2.is_before(d4));
  }
}

// Chrono: stdlib clocks, for computations that depend on time or for timing
// code.

#include <chrono>

TEST_CASE("std::chrono supports several clocks") {
  // Each of the 3 clocks expose a static now method, with varying
  // guarantees. It returns a time_point object of the current value in each clock.

  // System-wide, real-time clock.
  auto sys_now = std::chrono::system_clock::now();
  // High-resolution has the shortest tick period available. A tick is the
  // smallest atomic change that a clock can measure.
  auto hires_now = std::chrono::high_resolution_clock::now();
  // Steady clock that guarantees its value will never decrease (a system might
  // have to deal with leap seconds or inaccurate clocks).
  auto steady_now = std::chrono::steady_clock::now();

  // Epoch is the first time point of a clock.
  REQUIRE(sys_now.time_since_epoch().count() > 0);
  REQUIRE(hires_now.time_since_epoch().count() > 0);
  REQUIRE(steady_now.time_since_epoch().count() > 0);
}

TEST_CASE("std::chrono supports several units of measurement") {
  using namespace std::literals::chrono_literals;

  auto one_s = std::chrono::seconds(1);
  // Some other duration literals are ns, us, ms, s, m and h.
  auto thousand_ms = 1000ms;
  REQUIRE(one_s == thousand_ms);
}

TEST_CASE("std::chrono supports duration_cast") {
  using namespace std::chrono;
  auto billion_ns_as_s = duration_cast<seconds>(1'000'000'000ns);
  REQUIRE(billion_ns_as_s.count() == 1);
}

#include <thread>

TEST_CASE("std::chrono used to sleep") {
  using namespace std::literals::chrono_literals;

  auto start = std::chrono::system_clock::now();
  std::this_thread::sleep_for(100ms);
  auto end = std::chrono::system_clock::now();
  REQUIRE(end - start >= 100ms);
}

struct Stopwatch {
  Stopwatch(std::chrono::nanoseconds& result)
    : result{ result },
    start{ std::chrono::high_resolution_clock::now() } {} 

  ~Stopwatch() {
    result = std::chrono::high_resolution_clock::now() - start;
  }
private:
  std::chrono::nanoseconds& result;
  const std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

#include <cstdio>
#include <cstdint>

TEST_CASE("Timing with Chrono") {
  const size_t n = 1'000'000;
  std::chrono::nanoseconds elapsed;
  {
    Stopwatch sw{ elapsed };
    volatile double result{ 1.23e45 };
    for (double i = 1; i < n; i++) {
      result /= i;
    }
  }
  auto time_per_division = elapsed.count() / double{ n };
  printf("Took %gns per division.\n", time_per_division);
}

// ---------------------------------------------------------------------------------
// Numerics.

// Numeric functions.

TEST_CASE("Common math functions in the stdlib") {
#include <cmath>
  REQUIRE(std::abs(-10) == Approx(10));              // absolute value
  REQUIRE(std::fmod(10, 3) == Approx(1));            // remainder of fp division
  REQUIRE(std::remainder(10, 3) == Approx(1));       // signed remainder
  REQUIRE(std::fma(10, 3, 5) == Approx(10 * 3 + 5)); // x * y + z aka
                                                     // fused multiplication addition.
  REQUIRE(std::exp(1) == Approx(2.7182818));         // e^x
  REQUIRE(std::exp2(1) == Approx(2));                // 2^x
  REQUIRE(std::log(exp(1)) == Approx(1));            // ln x
  REQUIRE(std::log10(10) == Approx(1));              // log_10 x
  REQUIRE(std::log2(2) == Approx(1));                // log_2 x
  REQUIRE(std::erf(100) == Approx(1));               // Gauss error function at x
  REQUIRE(std::pow(2, 3) == Approx(8));              // x^y
  REQUIRE(std::sqrt(4) == Approx(2));                // x^0.5
  REQUIRE(std::cbrt(27) == Approx(3));               // x^(1/3)
  REQUIRE(std::hypot(4, 3) == Approx(5));            // sqrt(x^2 + y^2)
  // All [inverse] trigonometric functions, and their associated
  // hyperbolic functions, are available: sin, cos, tan, asin, acos, atan, sinh,
  // cosh, tanh, asinh, acosh, atanh.
  REQUIRE(std::ceil(3.5) == Approx(4));              // Nearest integer >= x
  REQUIRE(std::floor(3.5) == Approx(3));             // Nearest integer <= x
  REQUIRE(std::round(3.5) == Approx(4));             // Nearest integer equal to x.
                                                     // Rounds up from zero in midpoints
#include <numeric>
  REQUIRE(std::gcd(3, 9) == 3);                 // Greates Common Divisor of x and y
  REQUIRE(std::lcm(3, 9) == 9);                 // Least Common Multiple of x and y
}

// Complex numbers.

#include <complex>

TEST_CASE("std::complex has a real and imaginary component") {
  std::complex<double> a{ 0.5, 14.13 };
  REQUIRE(a.real() == Approx(0.5));
  REQUIRE(std::imag(a) == Approx(14.13));
}


TEST_CASE("std::complex operations") {
  auto c_eq = [](auto c1, auto c2) {
    return c1.imag() == c2.imag() && c1.real() == c2.real();
  };
  std::complex<double> c1{ 3, 10 }, c2{ 3.87, 9.32 };

  SECTION("complex-complex arithmetic") {
    std::complex result = { 6.87, 19.32 };
    REQUIRE(c1 + c2 == result);
  }

  SECTION("complex-scalar arithmetic") {
    std::complex result = { 103.0, 10.0 };
    REQUIRE(c1 + 100.0 == result);
  }

  SECTION("Other operations on std::complex") {
    REQUIRE(std::abs(c1) == Approx(10.4403));             // magnitude
    REQUIRE(std::arg(c1) == Approx(1.27933L));            // phase angle
    REQUIRE(std::norm(c1) == Approx(109L));               // squared magnitude
    REQUIRE(std::conj(c1) == std::complex{ 3.0, -10.0 }); // conjugate 
    // All [inverse] trigonometric functions are available.
    auto c3 = std::polar(10.4403, 1.2793);                // complex from polar
  }
}

// Random numbers.

// Pseudo-RNG is generally best achieved with the Mersenne Twister, std::mt19937_64.
// Crypto-secure RNG is generally best achieved with std::random_device.
// Both (and others) are random number engines.

#include <random>

TEST_CASE("mt19937_64 is pseudorandom") {
  std::mt19937_64 mt_engine{ 91586 };
  // The generated sequence is determined by the seed:
  REQUIRE(mt_engine() == 8346843996631475880);
  REQUIRE(mt_engine() == 2237671392849523263);
  REQUIRE(mt_engine() == 7333164488732543658);
}

TEST_CASE("std::random_device is invocable") {
  std::random_device rd_engine{};
  REQUIRE_NOTHROW(rd_engine());
}

// Random number distributions: Maps numbers to probability densities.
// Available in stdlib:
// uniform, normal, log-normal, chi-squared, cauchy, fisher, student,
// bernoulli, binomial, geometric, poisson, exponential, gamma, weibull and
// extreme value.

// Additional ones available in Boost:
// beta, hypergeometric, logistic and inverse normal.

TEST_CASE("std::uniform_int_distribution produces uniform ints") {
  std::mt19937_64 mt_engine{ 102787 };
  // The parameters are its min and max limits.
  std::uniform_int_distribution<int> int_d{ 0, 10 };
  const size_t n{ 1'000'000 };
  int sum{};
  for (size_t i{}; i < n; i++) {
    // The distribution accepts a random number engine for getting samples.
    sum += int_d(mt_engine);
  }
  const auto sample_mean = sum / double{ n };
  REQUIRE(sample_mean == Approx(5).epsilon(.1));
}

// Numeric limits.

#include <limits>

TEST_CASE("std::numeric_limits::min provides the smallest finite value") {
  auto my_cup = std::numeric_limits<int>::min();
  auto underfloweth = my_cup - 1;
  REQUIRE(my_cup < underfloweth);
}

TEST_CASE("Other <limits> functions") {
  REQUIRE(std::numeric_limits<int>::is_signed);
  REQUIRE(std::numeric_limits<int>::is_integer);
  REQUIRE_FALSE(std::numeric_limits<int>::has_infinity);
  REQUIRE(std::numeric_limits<int>::digits10 == 9);
}

// Boost numeric conversion: Provides runtime bounds checking.

#include <boost/numeric/conversion/converter.hpp>
#include <boost/numeric/conversion/cast.hpp>

using double_to_int = boost::numeric::converter<int, double>;

TEST_CASE("boost::numeric::converter implements operator()") {
  double_to_int dti;
  REQUIRE(dti(3.14159) == 3);
}

TEST_CASE("boost::numeric::converter checks for {under,over}flow") {
  auto yuge = std::numeric_limits<double>::max();
  double_to_int dti;
  REQUIRE_THROWS_AS(dti(yuge), boost::numeric::positive_overflow);
}

TEST_CASE("boost::numeric_cast checks for {under,over}flow") {
  // boost::converter can have its behavior, such as overflow handling and
  // rounding behavior. In case the default is fine, Boost provides a
  // numeric_cast function template as shortcut:
  auto yuge = std::numeric_limits<double>::max();
  REQUIRE_THROWS_AS(boost::numeric_cast<int>(yuge), boost::numeric::positive_overflow);
}

// Compile-time rational arithmetic

#include <ratio>

TEST_CASE("std::ratio basics") {
  using ten = std::ratio<10, 1>;
  using two_thirds = std::ratio<2, 3>;
  using result = std::ratio_multiply<ten, two_thirds>;
  REQUIRE(result::num == 20);
  REQUIRE(result::den == 3);
}

TEST_CASE("std::ratio literals") {
  // Current literals range from quecto to quetta.
  using result = std::ratio_divide<std::deca, std::deci>;
  REQUIRE(result::num == 100);
  REQUIRE(result::den == 1);
}

// ---------------------------------------------------------------------------------
// Exercises.

// Exercise 12-1

template<typename To, typename From>
std::optional<To> narrow_cast(From value) {
  const auto converted = static_cast<To>(value);
  const auto backwards = static_cast<From>(converted);
  if (value == backwards) return converted;
  return std::nullopt;
}

TEST_CASE("narrow_cast optionally returns the converted value") {
  SECTION("nullopt with narrow conversion") {
    int cyclic{ 142857 };
    REQUIRE(narrow_cast<short>(cyclic) == std::nullopt);
  }

  SECTION("full without narrow conversion") {
    int perfect{ 496 };
    REQUIRE(narrow_cast<short>(perfect) != std::nullopt);
  }
}

// Exercise 12-2

struct PasswordGenerator {
  PasswordGenerator() = default;

  void generate(size_t password_size) {
    auto pwd = new char[password_size + 1];

    for (size_t i{}; i < password_size; i++) {
      auto idx = int_d(rd_engine);
      pwd[i] = alphanumerics[idx];
    }

    pwd[password_size] = '\0';
    printf("Generated password: %s\n", pwd);

    delete[] pwd;
  }
private:
  const char* alphanumerics{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
  };
  std::uniform_int_distribution<int> int_d{ 0, 61 };
  std::random_device rd_engine{};
};

TEST_CASE("PasswordGenerator generates passwords") {
  PasswordGenerator pwd_g{};

  pwd_g.generate(3);
  pwd_g.generate(40);
  pwd_g.generate(311);
}
