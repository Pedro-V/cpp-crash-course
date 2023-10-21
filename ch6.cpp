#include <cstdio>
#include <utility>
#include <stdexcept>
#include <type_traits>

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
    // can't be copied
    SimpleUniquePointer(const SimpleUniquePointer&) = delete;
    SimpleUniquePointer& operator=(const SimpleUniquePointer&) = delete;
    // move semantics
    SimpleUniquePointer(SimpleUniquePointer&& other) noexcept
        : pointer{ other.pointer }
    {
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
};

struct Tracer {
    Tracer(const char *name) : name{ name } {
        printf("%s constructed.\n", name);
    }
    ~Tracer() {
        printf("%s destructed.\n", name);
    }
private:
    const char *name;
};

void consumer(SimpleUniquePointer<Tracer> consumer_ptr) {
    printf("(cons) consumer_ptr: %p\n", consumer_ptr.get());
}

void unique_pointer_example() {
    auto ptr_a = SimpleUniquePointer(new Tracer{ "ptr_a" });
    printf("(main) ptr_a: %p\n", ptr_a.get());
    // we need to move it, since it's non-copyable.
    consumer(std::move(ptr_a));
    printf("(main) ptr_a: %p\n", ptr_a.get());
}

// -------------------------------------------------------------------------------- 
// concepts: allowing for type-checking at template instantiation (and not on
// first use).
// TODO: update this to ISO concepts syntax. 2nd Edition of the book has it.

/*
template<typename T>
concept Averageable = std::is_default_constructible<T>::value &&
    requires (T a, T b) {
        { a += b } -> T;
        { a / size_t{ 1 } } -> T;
    };

template <Averageable T>
T mean_with_concepts(const T* values, size_t length) {
    T result{};
    for (size_t i = 0; i < length; i++) {
        result += values[i];
    }
    return result / length;
}

void mean_with_concepts_test() {
    // still works fine.
    const double nums_d[] = { 1.0f, 2.0f, 3.0f, 4.0f };
    printf("double: %f \n", mean_with_concepts(nums_d, sizeof(nums_d)));
    // won't work, with better compiler diagnostics.
    auto value1 = 0.0, value2 = 1.0;
    const auto values[] { &value1, &value2 };
    mean_with_concepts(values, 2);
}
*/

// -------------------------------------------------------------------------------- 
// non-type template parameters

template <size_t Index, typename T, size_t Length>
T& get(T (&arr)[Length]) {
    static_assert(Index < Length, "Out-of-bounds access");
    return arr[Index];
}

void get_test() {
    int fib[]{ 1, 1, 2, 0 };
    printf("%d %d %d ", get<0>(fib), get<1>(fib), get<2>(fib));
    get<3>(fib) = get<1>(fib) + get<2>(fib);
    printf("%d\n", get<3>(fib));
    // using out-of-bounds or non-const index results in compile error.
}

// -------------------------------------------------------------------------------- 
// variadic templates.

// Arguments is a parameter pack type.
template <typename T, typename... Arguments>
SimpleUniquePointer<T> make_simple_unique(Arguments... arguments) {
    return SimpleUniquePointer<T>{ new T{ arguments... } };
}

struct Dog {
    Dog(const char *name, int age) : name{ name }, age{ age } {}
    void get_description(char *output) {
        sprintf(output, "%s, aged %d", name, age);
    }
private:
    int age;
    const char *name;
};

void variadic_template_test() {
    // the variadic template can deduce the rest of the parameter pack.
    auto p = make_simple_unique<Dog>("Rufus", 11);
    char info[100];
    p.get()->get_description(info);
    printf("%s\n", info);
}

// -------------------------------------------------------------------------------- 
// exercises

// 6-1 mode function
int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

int mode(const int *values, size_t length) {
    auto *sorted = const_cast<int*>(values);
    qsort(sorted, length, sizeof(int), compare);
    int most_frequent = 0, most_frequent_occurrences = 0;
    int curr, curr_occurrences;
    for (size_t i = 0; i < length; i++) {
        if ((i == 0) || (curr != sorted[i])) {
            curr = sorted[i];
            curr_occurrences = 0;
        }
        curr_occurrences++;
        if (curr_occurrences > most_frequent_occurrences) {
            most_frequent = curr;
            most_frequent_occurrences = curr_occurrences;
        }
    }
    return most_frequent;
}

// 6-2 template mode function

template <typename T>
T mode_template(const T *values, size_t length) {
    auto* sorted = const_cast<T*>(values);
    qsort(sorted, length, sizeof(T), compare);
    T most_frequent, curr;
    int most_frequent_occurrences = 0, curr_occurrences;
    for (size_t i = 0; i < length; i++) {
        if ((i == 0) || (curr != sorted[i])) {
            curr = sorted[i];
            curr_occurrences = 0;
        }
        curr_occurrences++;
        if (curr_occurrences > most_frequent_occurrences) {
            most_frequent = curr;
            most_frequent_occurrences = curr_occurrences;
        }
    }
    return most_frequent;
}

// 6-3 TODO: learn how concepts are implemented in the ISO std

// 6-4 
template<typename T, size_t Length>
T mean_static(T (&values)[Length]) {
    T result{};
    for (size_t i = 0; i < Length; i++) {
        result += values[i];
    }
    return result / Length;
}

// 6-5
template<typename T>
struct Bank {
    void make_transfer(T from, T to, double amount) {
        from -= amount;
        to += amount;
    }
};

// 6-6

struct Account {
    void operator -=(double amount) {
        balance += amount;
    }
    void operator +=(double amount) {
        balance -= amount;
    }
private:
    float balance;
};

// 6-7

struct IAccount {
    virtual void operator -=(double amount) = 0;
    virtual void operator +=(double amount) = 0;
};

struct CheckingAccount : IAccount {
    void operator -=(double amount) override {
        checking_balance -= amount;
    }
    void operator +=(double amount) override {
        checking_balance += amount;
    }
private:
    float checking_balance;
};

struct SavingsAccount : IAccount {
    void operator -=(double amount) override {
        savings_balance -= amount;
    }
    void operator +=(double amount) override {
        savings_balance += amount;
    }
private:
    float savings_balance;
};


int main() {
}
