#include <cstdio>
#include <stdexcept>

// compile-time polymorphism can be achieved in C++ with templates
// template is a class or function with template parameters, which can be of any type

template<typename X, typename Y, typename Z>
struct MyTemplateClass {
    X foo (Y&);
private:
    Z* member;
};

template<typename X, typename Y, typename Z>
X my_template_function(Y& arg1, const Z* arg2) {}

void template_instantiation_example() {
    MyTemplateClass<int, float, char*> instantiated_template{};
    int x = 10, y = -10;
    my_template_function<void, int, int>(y, &x);
}

// -------------------------------------------------------------------------------- 

// named conversions
// used when we can't use implicit conversions nor constructor to get the types we need

// const_cast: cast away const qualifiers
void carbon_thaw(const int& encased_solo) {
    //encased_solo++; // can't modify const
    auto& hibernation_sick_solo = const_cast<int&>(encased_solo);
    hibernation_sick_solo++;
}

void test_const_cast() {
    int x = 10;
    carbon_thaw(x);
    printf("x = %d\n", x);
}

// -------------------------------------------------------------------------------- 
// static_cast: reverse well-defined implicit conversion

short increment_as_short(void *target) {
    auto as_short = static_cast<short*>(target);
    (*as_short)++;
    return *as_short;
}

void static_cast_example() {
    short beast{ 665 };
    auto mark_of_the_beast = increment_as_short(&beast);
    printf("%d is the mark of the beast\n", mark_of_the_beast);
}

// -------------------------------------------------------------------------------- 
// reinterpret_cast: type conversions that aren't well defined

void reinterpret_cast_example() {
    auto timer = reinterpret_cast<const unsigned long*>(0x1000);
    printf("Timer is %lu\n", *timer);
}

// -------------------------------------------------------------------------------- 
// implementing a static_cast that checks for narrowing

template <typename To, typename From>
To narrow_cast(From value) {
    const auto converted = static_cast<To>(value);
    const auto backwards = static_cast<From>(converted);
    if (value != backwards) throw std::runtime_error{ "Narrowed!" };
    return converted;
}

void narrow_cast_example() {
    int perfect{ 496 };
    const auto perfect_short = narrow_cast<short>(perfect);
    printf("perfect_short: %d\n", perfect_short);
    try {
        int cyclic{ 142857 };
        const auto cyclic_short = narrow_cast<short>(cyclic);
        printf("cyclic_short: %d\n", cyclic_short);
    } catch (const std::runtime_error& e) {
        printf("Exception: %s\n", e.what());
    }
}

// -------------------------------------------------------------------------------- 
// generic mean

template <typename T>
T mean(const T* values, size_t length) {
    T result{};
    for (size_t i = 0; i < length; i++) {
        result += values[i];
    }
    return result / length;
}

void mean_example() {
    const double nums_d[] {1.0, 2.0, 3.0, 4.0 };
    printf("double: %f\n", mean(nums_d, 4));

    const float nums_f[] {1.0f, 2.0f, 3.0f, 4.0f };
    printf("float: %f\n", mean(nums_f, 4));

    const int nums_i[] { 1, 2, 3, 4 };
    printf("int: %d\n", mean(nums_i, 4));
}

// -------------------------------------------------------------------------------- 
// generic unique pointer

template <typename T>
struct SimpleUniquePointer {
    SimpleUniquePointer() = default;
    SimpleUniquePointer(T* pointer) : pointer{ pointer } {}
    ~SimpleUniquePointer () {
        if (pointer) delete pointer;
    }
    SimpleUniquePointer(const SimpleUniquePointer&) = delete;
    SimpleUniquePointer& operator=(const SimpleUniquePointer&) = delete;
    SimpleUniquePointer(SimpleUniquePointer&& other) noexcept
        : pointer{ other.pointer } {
        other.pointer = nullptr;
    }
    SimpleUniquePointer& operator=(SimpleUniquePointer&& other) noexcept {
        if (pointer) delete pointer;
        pointer = other.pointer;
        other.pointer = nullptr;
        return *this;
    }
    T* get() {
        return pointer;
    }
private:
    T* pointer;
}

int main(void) {
    mean_example();
}
