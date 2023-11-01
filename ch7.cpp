#include <cstdio>
#include <stdexcept>

// Expressions are computations that produce results and side effects.

// -------------------------------------------------------------------------------- 
// operator overloading.

/*
 * C++ has some operators:
 * logical;
 * arithmetic;
 * assignment;
 * increment/decrement;
 * comparison;
 * member access;
 * ternary conditional;
 * comma.
 * We can overload some of them for our own classes.
 */

struct CheckedInteger {
    CheckedInteger(unsigned int value) : value{ value } {}

    CheckedInteger operator+(unsigned int other) const {
        CheckedInteger result{ value + other };
        if (result.value < value) throw std::overflow_error{ "Overflow!" };
        return result;
    }

    const unsigned int value;
};

#include <limits>
void operator_overloading_test() {
    CheckedInteger a{ 100 };
    auto b = a + 200;
    printf("a + 200 = %u\n", b.value);
    try {
        // Numeric_limits is a template class that exposes constants for limits
        // of the template type parameter.
        auto c = a + std::numeric_limits<unsigned int>::max();
    } catch (const std::overflow_error& e) {
        printf("(a + max) Exception: %s\n", e.what());
    }
}


// -------------------------------------------------------------------------------- 
// Overloading `new` and `delete`.

#include <new>
struct Bucket {
    const static size_t data_size{ 4096 };
    std::byte data[data_size];
};

struct LargeBucket {
    const static size_t data_size{ 1048576 };   // 1 MB
    std::byte data[data_size];
};

struct Heap {
    void *allocate(size_t bytes) {
        if (bytes > LargeBucket::data_size) throw std::bad_alloc{};
        if (bytes > Bucket::data_size) {
            return big_bucket.data;
        }
        for (size_t i = 0; i < n_heap_buckets; i++) {
            if (!bucket_used[i]) {
                bucket_used[i] = true;
                return buckets[i].data;
            }
        }
        throw std::bad_alloc{};
    }

    void free(void *p) {
        for (size_t i = 0; i < n_heap_buckets; i++) {
            if (buckets[i].data == p) {
                bucket_used[i] = false;
                return;
            }
        }
    }
    
    static const size_t n_heap_buckets{ 10 };
    Bucket buckets[n_heap_buckets]{};
    LargeBucket big_bucket{};
    bool bucket_used[n_heap_buckets]{};
};

// We overload the new and delete operators. Now, we are limited to 10 4096-byte
// sized buckets.
// Commented out for using default new/delete later.

/*
Heap heap;

void *operator new(size_t n_bytes) {
    return heap.allocate(n_bytes);
}

void operator delete(void *p) {
    return heap.free(p);
}

void heap_test() {
    printf("Buckets:   %p\n", heap.buckets);
    auto breakfast = new unsigned int{ 0xC0FFE };
    auto dinner = new unsigned int{ 0xDEADBEEF };
    printf("Breakfast: %p 0x%X\n", breakfast, *breakfast);
    printf("Dinner:    %p 0x%X\n", dinner, *dinner);
    delete breakfast;
    delete dinner;
    try {
        while (true) {
            new char;
            printf("Allocated a char.\n");
        }
    } catch (const std::bad_alloc&) {
        printf("std::bad_alloc caught.\n");
    }
}
*/

// -------------------------------------------------------------------------------- 
// Placement operators: allows new to place data on already-allocated memory.
// This allows finer-grained control over the object's lifecycle, but requires
// manual call of its destructor.
// `delete` can't be called because the data wasn't allocated by `new`, only
// placed by it.

struct Point {
    Point() : x{}, y{}, z{} {
        printf("Point at %p constructed.\n", this);
    }

    ~Point() {
        printf("Point at %p destructed.\n", this);
    }

    double x, y, z;
};

void placement_test() {
    const auto point_size = sizeof(Point);
    std::byte data[3 * point_size];
    printf("Data starts at %p.\n", data);
    auto p1 = new(&data[0 * point_size]) Point{};
    auto p2 = new(&data[1 * point_size]) Point{};
    auto p3 = new(&data[2 * point_size]) Point{};
    p1->~Point();
    p2->~Point();
    p3->~Point();
}

// -------------------------------------------------------------------------------- 
// Evaluation order.

void evaluation_order_test() {
    auto a = 10;
    // We have absolutely no idea how the expression below is evaluated.
    // Since the result of the whole expression depends of the evaluation order
    // of their subexpressions, then b's value isn't well defined.
    // In other words, this causes undefined behavior.
    auto b = ++a + a;
    // The only operators that constrain evaluation order are `&&`, `||`, `?:`,
    // `,` (left-to-right) and `new` (the constructor argument is evaluated
    // before the call to the allocator function).
}

// -------------------------------------------------------------------------------- 
// Type conversions.
#include <cstdint>

// Implicit type conversion:

// Promotion rules that occur with arithmetic operators are a kind of
// implicit type conversion.
void implicit_conversion_test() {
    // Depending on the form of the conversion, narrowing may occurr
    auto x = 2.718281828459L;   // double
    // silent truncation.
    uint8_t y = x;
    // Braced-initialization prevents narrowing.
    uint8_t z{ x };
}

void integer_integer_conversion() {
    // If unsigned, discard the high-order bits: will always be 255.
    uint8_t unsigned_byte = 0b111111111;
    // Otherwise, undefined (implementation-defined) behavior.
    int8_t signed_byte = 0b111111111;
    printf("unsigned_byte: %u\nsigned_byte: %d\n", unsigned_byte, signed_byte);
}

void fp_fp_conversion() {
    // Generally, if the destination can fit the source value, it's ok.
    double x = std::numeric_limits<float>::max();
    long double y = std::numeric_limits<double>::max();
    // Otherwise, we have undefined behavior.
    float z = std::numeric_limits<long double>::max();
    printf("x: %g\ny: %Lg\nz: %g\n", x, y, z);
}

#include <cmath>
void bool_conversion() {
    // Anything (let it be pointer, integer or floating-point) non-zero
    // value converts to true. This includes special values like inf and nan.
    float f = std::numeric_limits<double>::max();
    void *p = nullptr;
    double nan = std::nan("");
    printf("%f -> %s\n", f, f ? "true" : "false");
    printf("%p -> %s\n", p, p ? "true" : "false");
    printf("%f -> %s\n", nan, nan ? "true" : "false");
}

// -------------------------------------------------------------------------------- 
// Explicit type conversion (casts).

void explicit_type_conversion() {
    // Braced initialization is an approach for explicit type conversions.
    int32_t a = 100;
    int64_t b{ a };
    if (a == b) printf("Non-narrowing conversion!\n");
    // The compiler emits a warning whenever braced initialization would cause a
    // narrowing conversion.
    int32_t c{ b };
}


// -------------------------------------------------------------------------------- 
// C-style casts
void trainwreck(const char *read_only) {
    auto as_unsigned = (unsigned char*)read_only;
    // segmentation fault
    *as_unsigned = 'b';
}

void uncompilable_trainwreck(const char *read_only) {
    // doesn't compile: this named cast doesn't throw away const correctness
    //auto as_unsigned = reinterpret_cast<unsigned char*>(read_only);
    // segmentation fault
    //*as_unsigned = 'b';
}

void trainwreck_test() {
    auto ezra = "Ezra";
    printf("Before trainwreck: %s\n", ezra);
    trainwreck(ezra);
    printf("After trainwreck: %s\n", ezra);
}

// -------------------------------------------------------------------------------- 
// User-defined type conversion.

struct ReadOnlyInt {
    ReadOnlyInt(int val) : val{ val } {}
    // User-defined type conversion from a ReadOnlyInt to an int.
    operator int() const {
        return val;
    }
private:
    const int val;
};

struct ExplicitReadOnlyInt {
    ExplicitReadOnlyInt(int val) : val{ val } {}
    // Explicit user-defined type conversion from a ReadOnlyInt to an int.
    explicit operator int() const {
        return val;
    }
private:
    const int val;
};

void user_defined_type_conversion_test() {
    ReadOnlyInt the_answer{ 42 };
    // Implicit
    auto ten_answers = 10 * the_answer;
    printf("Implicit type conversion: %d\n", ten_answers);

    // Explicit
    ExplicitReadOnlyInt explicit_the_answer{ 42 };
    // doesn't compile
    //auto explicit_ten_answers = 10 * explicit_the_answer;
    auto explicit_ten_answers = static_cast<int>(the_answer) * 10;
    printf("Explicit type conversion: %d\n", explicit_ten_answers);
}

// -------------------------------------------------------------------------------- 
// Constant expressions: The `constexpr` keyword is used on expressions where
// all information for evaluation is available at compile time, compelling the
// compiler to do so.

// All constexpr expressions are const, since they're all fixed at runtime.

struct Color {
    float H, S, V;
};

constexpr uint8_t max(uint8_t a, uint8_t b) {
    return a > b ? a : b;
}

constexpr uint8_t max(uint8_t a, uint8_t b, uint8_t c) {
    return max(a, max(b, c));
}

constexpr uint8_t min(uint8_t a, uint8_t b) {
    return a < b ? a : b;
}

constexpr uint8_t min(uint8_t a, uint8_t b, uint8_t c) {
    return min(a, min(b, c));
}

constexpr float modulo(float dividend, float divisor) {
    const auto quotient = dividend / divisor;
    return divisor * (quotient - static_cast<uint8_t>(quotient));
}

constexpr Color rgb_to_hsv(uint8_t r, uint8_t g, uint8_t b) {
    Color c{};
    const auto c_max = max(r, g, b);
    c.V = c_max / 255.0f;

    const auto c_min = min(r, g, b);
    const auto delta = c.V - c_min / 255.0f;
    c.S = c_max == 0 ? 0 : delta / c.V;
    
    if (c_max == c_min) {
        c.H = 0;
        return c;
    }
    if (c_max == r) {
        c.H = (g / 255.0f - b / 255.0f) / delta;
    } else if (c_max == g) {
        c.H = (b / 255.0f - r / 255.0f) / delta + 2.0f;
    } else if (c_max == b) {
        c.H = (r / 255.0f - g / 255.0f) / delta + 4.0f;
    }

    c.H *= 60.0f;
    c.H = c.H >= 0.0f ? c.H : c.H + 360.0f;
    c.H = modulo(c.H, 360.0f);
    return c;
}

void print_Color(Color c) {
    printf("H=%f S=%f V=%f\n", c.H, c.S, c.V);
}

void constexpr_test() {
    auto black = rgb_to_hsv(0, 0, 0);
    auto white = rgb_to_hsv(255, 255, 255);
    auto red   = rgb_to_hsv(255, 0, 0);
    auto green = rgb_to_hsv(0, 255, 0);
    auto blue  = rgb_to_hsv(0, 0, 255);
    Color colors[]{ black, white, red, green, blue };
    auto len = sizeof(colors) / sizeof(Color);
    for (size_t i = 0; i < len; i++) {
        print_Color(colors[i]);
    }
}

// -------------------------------------------------------------------------------- 
// Volatile Expressions: Every access on the volatile expression is treated as a
// visible side effect, avoiding compiler optimizations over it.

// Withou the volatile keyword, the compiler might eliminate the dead store and
// redundant loads.
// With it, we tell the compiler not to optimize away any access made to x.
int foo(int& x) {
    x = 10;     // dead store
    x = 20;
    auto y = x; // redundant load
    y = x;      // redundant load
    return y;   // might as weel return x
}

// -------------------------------------------------------------------------------- 
// Exercises.


// 7-1
template <size_t Length>
struct UnsignedBigInteger {
    UnsignedBigInteger(uint8_t input_data[Length]) {
        for (size_t i = 0; i < Length; i++) {
            bits[i] = input_data[i];
        }
    }

    UnsignedBigInteger operator+(const UnsignedBigInteger& other) {
        // Assumes other has the same bit length as current instance.
        uint8_t carry = 0;
        uint8_t sum;
        UnsignedBigInteger<Length> result;

        for (size_t i = Length - 1; i > -1; i++) {
            sum = carry + this[i] + other[i];
            carry = sum / 2;
            result[i] = sum % 2;
        }

        if (carry != 0) {
            throw std::runtime_error{ "Overflow!" };
        }

        return result;
    }

    UnsignedBigInteger operator-(const UnsignedBigInteger& other) {
        uint8_t borrow = 0;
        int sub;
        UnsignedBigInteger<Length> result;

        for (size_t i = Length - 1; i > -1; i++) {
            sub = this[i] - other[i] - borrow;
            if (sub < 0) {
                sub += 2;
            }
            result[i] = sub;
            borrow = (this[i] < other[i] + borrow) ? 1 : 0;
        }
        if (borrow == 1) {
            throw std::runtime_error{ "Underflow!" };
        }

        return result;
    }

    uint8_t operator[](size_t i) {
        if (i >= Length) {
            throw std::runtime_error{ "index greater than size" };
        }
        else {
            return bits[i];
        }
    }
private:
    uint8_t bits[Length];
};

// 7-2
// Defined just below Bucket

int main() {
}
