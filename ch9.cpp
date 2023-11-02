// -------------------------------------------------------------------------------- 
// Functions

#include <cstdio>
#include <cstdint>
#include <stdexcept>

// -------------------------------------------------------------------------------- 
// Function declarations can have prefix and suffix modifiers, that alter the
// function's semantics:

/*
 * Prefix modifiers:
 * - static: If a method, indicated the function isn't associated with any
 * instance, but with the class itself. Otherwise, indicates internal linkage;
 * - virtual: Indicates a method can be overriden;
 * - override: Indicates intention to override the inherited method;
 * - constexpr: Indicates the function should be evaluated at compile time;
 * - attributes: Cited on previous chapter, such as [[noretur]];
 * - inline: Indicates the function calls should be inlined by the compiler;
 */

inline void inlined_example() {
    printf("I'm inlined!\n");
}

/*
 * Suffix modifiers:
 * - const: The method won't modify the instance's state;
 * - noexcept: The function will not throw an exception. Enables some optimizations;
 * - final: The method can't be overridden;
 * - volatile: Indicates every access on the function should be treated as a
 * visible side effect. Disables some optimizations;
 */

struct BostonCorbett {
    virtual void shoot() final {
        printf("What a God we have... God avenged Abraham Lincoln\n");
    }
};

struct BostonCorbettJunior : BostonCorbett {
    // Compile error:
    // void shoot() override {};
};

// -------------------------------------------------------------------------------- 
// auto return type: A function's return type can be deducted. Not recommended,
// since types are a kind of documentation. This can be sometimes useful in
// the use of templates:

// -> means the function's return type will be the result of decltype, which
// output the type of an expression.
template <typename X, typename Y>
auto add(X x, Y y) -> decltype (x + y) {
    return x + y;
}

// -------------------------------------------------------------------------------- 
// Variadic functions: Accept variable number of arguments.

#include <cstdarg>
int sum_c_style(size_t n, ...) {
    va_list args;
    va_start(args, n);
    int result{};
    while (n--) {
        auto next_element = va_arg(args, int);
        result += next_element;
    }
    va_end(args);
    return result;
}

// -------------------------------------------------------------------------------- 
// Variadic functions with variadic templates are safer and more powerful than
// C-style variadic functions.

// Compile-time recursion:
template <typename T>
constexpr auto sum_comp_time(T x) {
    return x;
}

template <typename T, typename... Args>
constexpr T sum_comp_time(T x, Args... args) {
    return x + sum(args...);
}

// Fold-expressions:
template <typename... T>
constexpr auto sum_fold(T... args) {
    return (... + args);
}

// -------------------------------------------------------------------------------- 
// Function pointers.

float add(float a, int b) {
    return a + b;
}

float subtract(float a, int b) {
    return a - b;
}

void function_pointers_example() {
    const float first{ 100 };
    const int second{ 20 };

    float(*operation)(float, int){};
    printf("operation initialized to %p\n", operation);
    printf("&add = 0x%p\n", operation = add);
    printf("operation(100, 20) = %g\n", operation(100, 20));
    printf("&subtract = 0x%p\n", operation = subtract);
}

// -------------------------------------------------------------------------------- 
// Function call operator.

// This is called a function object because of its overload of operator().
struct CountIf {
    CountIf(char x) : x{ x } {}
    size_t operator()(const char* str) const {
        size_t index{}, result{};
        while (str[index]) {
            if (str[index] == x) result++;
            index++;
        }
        return result;
    }
private:
    const char x;
};

void func_operator_example() {
    CountIf s_counter{ 's' };
    auto sally = s_counter("Sally sells seashells by the seashore.");
    printf("Sally: %zd\n", sally);
}

// -------------------------------------------------------------------------------- 
// Lambda expressions.

// Polymorphic high-order function.
template <typename Fn, typename T>
void transform(Fn fn, const T *in, T *out, size_t length) {
    for (size_t i{}; i < length; i++) {
        out[i] = fn(in[i]);
    }
}

void lambda_example() {
    int x = 10, y = 5, z = 0;
    printf("x = %d y = %d z = %d\n", x, y, z);
    /*
     * The below lambda will capture
     * - All values in the current scope by value (=); Capturing all by
     *   reference would be just &.
     * - x by reference (&x);
     * - y by value, aliased to my_y;
     * Obs: A lambda expression can capture an enclosing exist with [this] (by
     * reference) or with [*this] (by value).
     */
    auto lambda = [=, &x, my_y=y](const char *str) {
        x++;
        // 2 compile errors, since they are read-only:
        // my_y++;
        // z++;
        printf("x = %d y = %d z = %d str = %s\n", x, y, z, str);
    };
    lambda("By Alonzo Church");
    printf("x = %d y = %d z = %d\n", x, y, z);

    size_t len{ 3 };
    int base_int[]{ 1, 2, 3 }, a[len];
    float base_float[]{ 10.f, 20.f, 30.f }, b[len];
    auto translate = [](auto x) { return 10 * x + 5; };
    transform(translate, base_int, a, len);
    transform(translate, base_float, b, len);

    for (size_t i{}; i < len; i++) {
        printf("Element %zd: %d %.2f\n", i, a[i], b[i]);
    }
}

// -------------------------------------------------------------------------------- 
// std::function is a class template for generic function pointers: static
// functions, function objects and lambdas. 
// It may also be described as a wrapper around a callable object.
#include <functional>

void static_func() {
    printf("A static function.\n");
}

void function_example() {
    // Starts in empty mode: no callable object. Trying to call it causes error.
    std::function<void()> empty_func;
    try {
        empty_func();
    } catch (const std::bad_function_call& e) {
        printf("Exception: %s\n", e.what());
    }
    // Assigning a callable object:
    std::function<void()> func{ []{ printf("A lambda.\n"); } };
    func();
    func = static_func;
    func();
}

size_t count_spaces(const char* str) {
    size_t index{}, result{};
    while (str[index]) {
        if (str[index] == ' ') result++;
        index++;
    }
    return result;
}

void function_array_example() {
    int num_func_types{ 3 };
    std::function<size_t(const char*)> funcs[num_func_types]{
        count_spaces,
        CountIf{ 'e' },
        [](const char* str) {
            size_t index{};
            while (str[index]) index ++;
            return index;
        }
    };

    auto text = "Sailor went to sea to see what he could see.";
    for (size_t i{}; i < num_func_types; i++) {
        printf("func #%zd: %zd\n", i, funcs[i](text));
    }
}

// The main function section is not explored here.

// -------------------------------------------------------------------------------- 
// Exercises.
// 9-1

template <typename Fn, typename In, typename Out>
Out fold(Fn f, In* input, size_t length, Out initial){
    if (length == 0) {
        return initial;
    } else {
        return f(input[0], fold(f, input + 1, length - 1, initial));
    }
}

void fold_example() {
    int data[]{ 100, 200, 300, 400, 500 };
    size_t data_len = 5;
    std::function<int (int, int)> operations[]{
        [](int x, int y){ return x + y; },
        [](int x, int y){ return x * y; },
        [](int x, int y){ return x > y ? x : y; },
        [](int x, int y){ return x < y ? x : y; },
        [](int x, int y){ return y + (x > 200); }
    };
    int initials[]{ 0, 1, data[0], data[0], 0 };
    const char *names[]{ "sum", "product", "maximum", "minimum", "greater than 200" };
    for (size_t i{}; i < data_len; i++) {
        printf("%s = %d\n",
                names[i],
                fold(operations[i], data, data_len, initials[i]));
    }
}

// 9-2
struct LengthElement {
    uint32_t length;
    uint32_t term_count;
};

struct LengthHistogram {
    LengthHistogram(uint32_t num_terms) : num_terms{ num_terms } {
        len_elems = new uint32_t[num_terms]{};
    }

    ~LengthHistogram() {
        delete[] len_elems;
    }

    void ingest(const char* term) {
        auto term_length = length(term);
        insert(term_length);
    }

    void insert(size_t term_length) {
        size_t i = 0;
        while ( i < num_terms && len_elems[i].length != term_length) {
            i++;
        }

        len_elems[i].term_count++;
    }

    size_t length(const char* term) {
        size_t i = 0;
        while (term[i]) i++;
        return i;
    }

    size_t 

private:
    LengthElement *len_elem;
    const uint32_t num_terms;
};


int main() {
    fold_example();
}
