#include <cstdio>
#include <cstdint>
#include <stdexcept>

// Statements

// -------------------------------------------------------------------------------- 
// Expression statements: Expressions followed by a semicolon. Only useful for
// its side effects.

void expression_stmts_example() {
    int x{};
    // All three statements below are expression statements. The second don't
    // has any side-effect.
    ++x;
    42;
    printf("The %d True Morty\n", x);
}

// -------------------------------------------------------------------------------- 
// Compound statements/blocks: Sequence of statements enclosed by braces { }.
// Blocks declare new scope, called block scope.

struct Tracer {
    Tracer(const char *name) : name{ name } {
        printf("%s constructed.\n", name);
    }

    ~Tracer() {
        printf("%s destructed.\n", name);
    }

private:
    const char* name;
};

void block_test() {
    Tracer main{ "main" };
    {
        printf("Block a\n");
        Tracer a1{ "a1" };
        Tracer a2{ "a2" };
    }
    {
        printf("Block b\n");
        Tracer b1{ "b1" };
        Tracer b2{ "b2" };
    }
}

// -------------------------------------------------------------------------------- 
// Declaration statements: Introduce identifiers.

// -------------------------------------------------------------------------------- 
// Function declaration: Provides a function's interface. 
void randomize(uint32_t&);

// The how_many function is allowed to call randomize because its interface has
// been defined, even if its implementation not yet.

void how_many() {
    size_t iterations{};
    uint32_t number{ 0x4C4347 };
    while (number != 0x474343) {
        randomize(number);
        ++iterations;
    }
    printf("It took %zd iterations.%d\n", iterations);
}

// Function definition: Its declaration and body. The body is the implementation.
void randomize(uint32_t& x) {
    x = 0x3FFFFFFF & (0x41C64E6D * x + 12345) % 0x80000000;
}

// This separation can also be done for methods. This is particularly useful for
// multiple-source-file programs.
struct RandomNumberGenerator {
    explicit RandomNumberGenerator(uint32_t seed) : number{ seed } {}
    uint32_t next();
private:
    uint32_t number;
};

uint32_t RandomNumberGenerator::next() {
    number = 0x3FFFFFFF & (0x41C64E6D * number + 12345) % 0x80000000;
    return number;
}

// -------------------------------------------------------------------------------- 
// Namespaces.

namespace BroopKidron13 {
   // All symbols declared within this block belong to the BroopKidron13
   // namespace.
}

// Namespace nesting. 1st way:
namespace BroopKidron13 {
    namespace Shaltanac {
    }
}
// 2nd way, more succint:
namespace BroopKidron13::Shaltanac {
    enum class Color {
        Mauve,
        Pink,
        Russet
    };

    struct JoopleberryShrub {
        const char* name;
        Color shade;
    };

    bool is_more_mauvey(const JoopleberryShrub& shrub) {
        return shrub.shade == Color::Mauve;
    }
}

void namespace_example() {
    const auto shaltanac_grass{ BroopKidron13::Shaltanac::Color::Russet };
    if (shaltanac_grass == BroopKidron13::Shaltanac::Color::Russet) {
        printf("The other Shaltanac's joopleberry shrub is always "
                "a more mauvey shade of pinky russet.\n");
    }
}

// `using` directive.
void using_example() {
    using BroopKidron13::Shaltanac::Color;
    const auto shaltanac_grass = Color::Russet;
    if (shaltanac_grass == Color::Russet) {
        printf("The other Shaltanac's joopleberry shrub is always "
                "a more mauvey shade of pinky russet.\n");
    }
}

// `using` can also be used to bring all the symbols from a given namespace.
// This must be used with care, and never in a header file.
using namespace BroopKidron13::Shaltanac;

void using_namespace_example() {
    const JoopleberryShrub yours {
        "The other Shaltanac",
        Color::Mauve
    };

    if (is_more_mauvey(yours)) {
        printf("%s's joopleberry shrub if always a more mauvey shade of pinky "
               "russet.\n", yours.name);
    }
}

// -------------------------------------------------------------------------------- 
// Type aliasing.

using String = const char[260];
using ShaltanacColor = BroopKidron13::Shaltanac::Color;

void type_alias_example() {
    const auto my_color{ ShaltanacColor::Russet };
    String saying {
        "The other Shaltanac's joopleberry shrub is "
        "always a more mauvey shade of pinky russet."
    };

    if (my_color == ShaltanacColor::Russet) {
        printf("%s\n", saying);
    }
}

// Type aliases are more powerful than typedefs and define macros, since they
// enable partial application of template parameters.

template <typename To, typename From>
struct NarrowCaster {
    To cast(From value) {
        const auto converted = static_cast<To>(value);
        const auto backwards = static_cast<From>(converted);
        if (value != backwards) throw std::runtime_error{ "Narrowed!" };
        return converted;
    }
};

template <typename From>
using short_caster = NarrowCaster<short, From>;

// Structured Binding declaration.
struct TextFile {
    bool success;
    const char *contents;
    size_t n_bytes;
};

TextFile read_text_file(const char *path) {
    const char static contents[]{ "Sometimes the goat is you." };
    return TextFile {
        true,
        contents,
        sizeof(contents)
    };
}

void structured_binding_example() {
    const auto [success, contents, length] = read_text_file("README.txt");
    if (success) {
        printf("Read %zd bytes: %s\n", length, contents);
    } else {
        printf("Failed to open README.txt.");
    }
}

// -------------------------------------------------------------------------------- 
// Attributes: Adding information to the compiler about expression statements.
// The [[noreturn]] attribute indicates that a function doesn't return. There
// are others, such as [[deprecated("reason")]], [[nodiscard]], etc.

[[noreturn]] void pitcher() {
    throw std::runtime_error{ "Knuckleball." };
}

// This adds compiler warnings.
[[deprecated("Prefer another function")]] void deprecated_function() {};

void attributes_example() {
    try {
        pitcher();
    } catch(const std::exception& e) {
        printf("exception: %s\n", e.what());
    }
    deprecated_function();
}

// -------------------------------------------------------------------------------- 
// Selection statements.
// if statements.

template <typename T>
constexpr const char *sign(const T& x) {
    const char *result{};
    if (x == 0) {
        result = "zero";
    } else if (x > 0) {
        result = "positive";
    } else {
        result = "negative";
    }
    return result;
}

void if_example() {
    printf("float 100 is %\n", sign(100.0f));
    printf("int  -200 is %\n", sign(-200));
    printf("char    0 is %\n", sign(char{}));
}

// Initialization statements can be put inside if and switch statements.

void test_init_in_if() {
    // Honestly, I find this hardly readable.
    if (const auto [success, txt, len] = read_text_file("README.txt"); success) {
        printf("Read %d bytes: %s\n", len, txt);
    } else {
        printf("Failed to open README.txt.\n");
    }
}

// constexpr ifs are evaluated at compile time. The code is emitted depending on
// the evaluation.
#include <type_traits>

template <typename T>
auto value_of(T x) {
    if constexpr (std::is_pointer<T>::value) {
        if (!x) throw std::runtime_error{ "Null pointer dereference." };
        return *x;
    } else {
        return x;
    }
}

void constexpr_if_example() {
    unsigned long level{ 8998 };
    auto level_ptr = &level;
    auto &level_ref = level;
    printf("Power level = %lu\n", value_of(level_ptr));
    ++*level_ptr;
    printf("Power level = %lu\n", value_of(level_ref));
    ++level_ref;
    printf("Power level = %lu\n", value_of(level++));
    try {
        level_ptr = nullptr;
        value_of(level_ptr);
    } catch (const std::exception& e) {
        printf("Exception: %s\n", e.what());
    }
}

// -------------------------------------------------------------------------------- 
// Iteration statements. The classical while, do-while and for loops are not shown
// here. 

// Range-based for loop: More concise and direct. Can be used over range
// expressions, which exposes begin and end methods. Both have to return
// iterators (object supporting !=, ++ and * (dereference)).

void range_based_for_example() {
    const int x[]{ 1, 1, 2, 3, 5, 8 };
    for (const auto element : x) {
        printf("%d ", element);
    }
}

// Implementing a range of our own.
struct FibonacciIterator {
    bool operator!=(int x) const {
        return x >= current;
    }

    FibonacciIterator& operator++() {
        const auto tmp = current;
        current += last;
        last = tmp;
        return *this;
    }

    int operator*() const {
        return current;
    }
private:
    int current{ 1 }, last{ 1 };
};

struct FibonacciRange {
    explicit FibonacciRange(int max) : max{ max } {}

    FibonacciIterator begin() const {
        return FibonacciIterator{};
    }

    int end() const {
        return max;
    }
private:
    const int max;
};

// Ranges are only worth implementing for code that will be used a lot.
void fib_range_example() {
    for (const auto i : FibonacciRange{ 5000 }) {
        printf("%d ", i);
    }
    printf("\n");
}

// Jump statements are not covered here.

// -------------------------------------------------------------------------------- 
// Exercises.

// 8-1

#include "fibonacci.h"

void ex_8_1() {
    for (const auto i : Fibonacci::FibonacciRange{ 5000 }) {
        printf("%d ", i);
    }
    printf("\n");
}

// 8-{2,3}

#include "prime_number.h"

void prime_number_example() {
    for (const auto i : PrimeNumber::PrimeNumberRange{ 5000 }) {
        printf("%d ", i);
    }
    printf("\n");
}

int main() {
    prime_number_example();
}
