#include <cstdio>
#include <cstring>
#include <utility>

// -------------------------------------------------------------------------------- 

// automatic storage duration is the default memory allocation/deallocation
// scheme for traditional function stack-based variables

// static storage duration:

void power_up_rat_thing(int nuclear_isotopes) {
    // encapsulation: lifetime starts at first function call and end at program
    // exit
    // however, it can only be referred inside this function's scope
    // it may be turned into a thread-local variable with the appropriate modifier
    static thread_local int rat_things_power = 200;
    rat_things_power += nuclear_isotopes;
    const auto waste_heat = rat_things_power * 20;
    if (waste_heat > 10'000) {
        printf("Warning! Hot doggie!\n");
    }
    printf("Rat-thing power = %d\n", rat_things_power);
}

// static members

class RatThing {
public:
    static int rat_things_power;
    static void power_up_rat_thing(int nuclear_isotopes) {
        rat_things_power += nuclear_isotopes;
        const auto waste_heat = rat_things_power * 20;
        if (waste_heat > 10'000) {
            printf("Warning! Hot doggie!\n");
        }
        printf("Rat-thing power = %d\n", rat_things_power);
    }
};

int RatThing::rat_things_power = 200;

// -------------------------------------------------------------------------------- 

// dynamic storage duration

void dyn_sd(void) {
    // the new keyword is the primary way to allocate dynamic objects.
    // Optionally, you can initialize the object too
    int *arr, *ptr = new int{ 42 };
    // the delete keyword deallocates memory of dynamic objects
    // the contents aren't zero-ed out, they remain how they previously were in the heap
    // until being overwritten
    delete ptr;
    // arrays can be dynamically allocated. the size specifier can be a
    // variable, so the array size can be determined at runtime
    printf("pointer before allocation: %p\n", arr);
    printf("Inform the dynamic array size: ");
    int size;
    scanf("%d", &size);
    // this surprisingly compiles: if size < 2 (the amount of initialized
    // values), it throws a runtime exception
    arr = new int[size]{ 3, 2 };
    printf("pointer after allocation: %p\n", arr);
    for (size_t i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    // without deleting, we can have memory leaks
    delete[] arr;
    printf("\npointer after deallocation: %p\n", arr);
}

// -------------------------------------------------------------------------------- 

// the object life cycle

class Tracer {
    const char* name;
public:
    Tracer(const char *name) : name{ name } {
        printf("%s constructed.\n", name);
    }
    ~Tracer() {
        printf("%s destructed.\n", name);
    }
};

// uncomment when running this section
// static Tracer t1{ "Static variable" };
thread_local Tracer t2{ "Thread-local variable" };

void life_cycle_tracker(void) {
    printf("A\n");
    Tracer t3{ "Automatic variable" };
    printf("B\n");
    const auto* t4 = new Tracer{ "Dynamic variable 1" };
    delete t4;
    // memory leak below
    const auto& t5 = new Tracer{ "Dynamic variable 2" };
    // auto t6 = t1;
    // auto& t7 = t1;
    auto& t8 = t2;
    printf("C\n");
}

// -------------------------------------------------------------------------------- 

// exceptions: types that communicate error conditions
// when error conditions occurs, exceptions are thrown, after which it's in
// flight, where normal execution is stopped and an exception handler is
// searched for dealing with the in-flight exception.

#include <stdexcept>

class Groucho {
public:
    void forget(int x) {
        if (x == 0xFACE) {
            throw std::runtime_error{ "I'd be glad to make an exception. "};
        }
        printf("Forgot 0x%X\n", x);
    }
};

void exception_main(void) {
    Groucho groucho;
    // try-catch blocks can have catches for each possibly-throwed exception
    try {
        groucho.forget(0xC0DE);
        groucho.forget(0xFACE);
        groucho.forget(0xC0FFEE);
    } catch (const std::runtime_error& e) {
        printf("exception caught with message: %s\n", e.what());
    }

    // exception inheritance and catch chaining
    try {
        throw std::logic_error{ "It's not about who wrong "
                                "it's not about who right" };
    } catch (std::logic_error& ex) {
        // log exception and terminate the program; there's a programming error
    } catch (std::runtime_error& ex) {
        // do our best to recover gracefully
    } catch (std::exception& ex) {
        // matches any exception that derives from std::exception and is not a
        // {logic,runtime}_error
    } catch (...) {
        // panic! an unforeseen exception type was thrown
    }
}

// noexcept is a contract where we promise to the compiler that no exception
// will be thrown in a certain function. otherwise, the C++ runtime calls
// terminate. using noexcept allows the compiler to do some extra optimizations.

bool is_odd(int x) noexcept {
    return 1 == (x % 2);
}

void hari_kari(void) noexcept {
    throw std::runtime_error{ "Goodbye, cruel world." };
}

// exceptions are in-flight throughout the call stack. this means when the
// runtime unwinds a function's stack frame, the automatic storage duration
// objects are destroyed. if a certain object's destructor can throw an
// exception, then unwinding a stack frame can cause two exceptions to be
// in-flight. in this case, the C++ runtime calls terminate.

// this could be avoided if the exception was handled inside the destructor

struct CyberdineSeries800 {
    CyberdineSeries800(void) {
        printf("I'm a friend of Sarah Connor.");
    }
    ~CyberdineSeries800(void) {
        throw std::runtime_error{ "I'll be back" };
    }
};

void two_exceptions(void) {
    try {
        CyberdineSeries800 t800;
        throw std::runtime_error{ "Come with me if you want to live." };
    } catch (const std::exception& e) {
        printf("Caught exception: %s\n", e.what());
    }
}

// -------------------------------------------------------------------------------- 

// putting it all together

class SimpleString {
    size_t max_size;
    char* buffer;
    size_t length;
public:
    SimpleString(size_t max_size) : max_size{ max_size }, length {} {
        if (max_size == 0) {
            throw std::runtime_error{ "Max size must be at least 1." };
        }
        buffer = new char[max_size];
        buffer[0] = 0;
    }

    // copy constructor (copy semantics)
    SimpleString(const SimpleString& other)
      : max_size{ other.max_size },
        buffer{ new char[other.max_size] },
        length{ other.length }
    {
        std::strncpy(buffer, other.buffer, max_size);
    }

    // copy assignment operator
    SimpleString& operator=(const SimpleString& other) {
        // handles self-copying
        if (this == &other) return *this;
        delete[] buffer;
        buffer = new char[other.max_size];
        length = other.length;
        max_size = other.max_size;
        std::strncpy(buffer, other.buffer, max_size);
        return *this;
    }

    // move constructor
    // noexcep avoid the compiler falling back to a copy constructor
    // && is a rvalue reference type
    SimpleString(SimpleString&& other) noexcept 
      : max_size{ other.max_size },
        buffer{ other.buffer },
        length{ other.length }
    {
        other.length = 0;
        other.buffer = nullptr;
        other.max_size = 0;
    }

    // move assignment operator
    SimpleString& operator=(SimpleString&& other) noexcept {
        if (this == &other) return *this;
        delete[] buffer;
        buffer = other.buffer;
        length = other.length;
        max_size = other.max_size;
        other.buffer = nullptr;
        other.length = 0;
        other.max_size = 0;
        return *this;
    }

    ~SimpleString(void) {
        delete[] buffer;
    }

    void print(const char* tag) const {
        printf("%s: %s", tag, buffer);
    }

    bool append_line(const char* x) {
        const auto x_len = strlen(x);
        if (x_len + length + 2 > max_size) return false;
        std::strncpy(buffer + length, x, max_size - length);
        length += x_len;
        buffer[length++] = '\n';
        buffer[length] = 0;
        return true;
    }
};

void simple_string_usage(void) {
    SimpleString string{ 115 };
    string.append_line("Starbuck, whaddya hear?");
    string.append_line("Nothin' but the rain.");
    string.print("A");
    string.append_line("Grab your gun and bring the cat in.");
    string.append_line("Aye-aye sir, coming home.");
    string.print("B");
    if (!string.append_line("Galactica!")) {
        printf("String was not big enough to append another message.\n");
    }
}

// the class below illustrates that an object's member constructors are called
// before the object's constructor. in destruction phase, the opposite occurs:
// the object's destructor is called first.

class SimpleStringOwner {
    SimpleString string;
public:
    // member initialization with wasteful copy
    SimpleStringOwner(const SimpleString& x) : string{ x } {}
    // without wasteful copy, using move semantics
    SimpleStringOwner(SimpleString&& x) : string{ std::move(x) } {}

    SimpleStringOwner(const char* x) : string{ 10 } {
        if (!string.append_line(x)) {
            printf("Exception thrown!\n");
            throw std::runtime_error{ "Not enough memory!" };
        }
        string.print("Constructed");
    }
    ~SimpleStringOwner(void) {
        string.print("About to destroy");
    }
};

void fn_c(void) {
    SimpleStringOwner c{ "cccccccccc" };
}

void fn_b(void) {
    SimpleStringOwner b{ "b" };
    fn_c();
}

void stack_unwind_exceptions(void) {
    try {
        SimpleStringOwner a{ "a" };
        fn_b();
        SimpleStringOwner d{ "d" };
    } catch (const std::exception& e) {
        printf("Exception: %s\n", e.what());
    }
}

// the tecnhique of pairing the allocation and deallocation of memory with the
// constructor and destructor of an object is also called
// "resource acquisition is initialization" (RAII).
// exceptions, and their stack-unwinding property, are only safe with RAII
// objects. otherwise, memory leaks can occur.

/*
 * exceptions alternatives: (for embedded contexts, for example)
 * exposing methods for communicating wether class invariants could be established
 * return multiple values using structured binding declaration and pattern matching
 * these are shown below:
 */

class Judge {
    int age;
public:
    Judge(int age) : age{ age } {}
    bool is_valid_age(void) {
        return age < 75;
    }
};

struct Result {
    Judge j;
    bool success;
};

Result make_judge(int age) {
    Judge j{ age };
    return { j, j.is_valid_age() };
}

void no_exceptions_main(void) {
    auto [j, success] = make_judge(75);
    printf("Class invariant %s\n",
            success ? "holds =)" : "doesn't hold =(");
}

// -------------------------------------------------------------------------------- 

// copy semantics
// copying is the act of creating a new, independent object y, initially equal
// to an original object x.
// that is, initially x == y, and changes to either don't affect the other.

// example: passing arguments to functions by value

int add_one(int x) {
    return ++x;
}

// the behavior is similar for fundamental and POD types

void test_add_one(void) {
    auto original = 1;
    auto result = add_one(original);
    printf("Original: %d, result: %d\n", original, result);
}

// classes require more care:

void copy_simple_string(void) {
    SimpleString a{ 100 };
    // comment the implemented copy assignment operator in the class definition
    SimpleString a_copy = a;    // a_copy.buffer points to same memory address
                                // as a.buffer
    a_copy.append_line("Hey hey hey");
    a.print("1");
    // since a and a_copy are automatic objects, after this function ends a
    // double free occurs, since the second destructor call tries to deallocate
    // already-deallocated memory.
}

void foo(SimpleString x) {
    x.append_line("This change is lost.");
}

void test_copy_semantics(void) {
    SimpleString a{ 100 };
    a.append_line("We apologize for the");
    SimpleString a_copy{ a };
    a.append_line("inconvenience.");
    a_copy.append_line("incontinence.");
    a.print("a");
    a_copy.print("a_copy");

    SimpleString b{ 20 };
    foo(b); // passing by value to functions invokes the copy constructor
    b.print("b still empty");
    printf("\n\n");

    // testing the copy assignment operator
    SimpleString c{ 50 };
    c.append_line("We apologize for the");
    SimpleString d{ 50 };
    d.append_line("Last message");
    c.print("c");
    d.print("d");
    d = c;
    c.print("c");
    d.print("d");
}

// default copy can be explicitly declared for classes
// it invokes copy construction or assignment on each of a class's members.
// sometimes this behaviour may be wrong (as with SimpleString)

struct Replicant {
    Replicant(const Replicant&) = default;
    Replicant& operator=(const Replicant&) = default;
};

// Some classes shouldn't be copied. The delete keyword makes the compiler
// reject code that copies classes in this category.

struct Highlander {
    Highlander(const Highlander&) = delete;
    Highlander& operator=(const Highlander&) = delete;
};

/*
void doesnt_compile(void) {
    Highlander a;
    Highlander b{ a };
}
*/

// -------------------------------------------------------------------------------- 

// move semantics
// copying can be unperformant. when we have a pointed-to object x that is useless
// after the construction of a new object y derived from x, it makes more sense
// to move the guts of x into y, without copy. After this move, x is considered
// in a moved-from state, where it can only be reassigned or destructed.
// using moved-from objects is a recipe for disaster

// value categories
// lvalue: any value that has a name
// rvalue: anything that isn't an lvalue

void val_cats(void) {
    SimpleString a{ 50 };                       // 50 is an rvalue
    SimpleStringOwner b{ a };                   // a is an lvalue
    SimpleStringOwner c{ SimpleString{ 50 } };  // the constructor argument is an rvalue
}

// we can communicate to the compiler that a function accepts lvalues or
// rvalues, using {l,r}value references

void ref_type(int& x) {
    printf("lvalue reference %d\n", x);
}

void ref_type(int&& x) {
    printf("rvalue reference %d\n", x);
}

void r_l_value_refererences(void) {
    auto x = 1;
    ref_type(x);
    ref_type(2);
    ref_type(x + 2);
    printf("\n");
    // std::move, from <utility>, casts from a lvalue reference to an rvalue reference
    ref_type(std::move(x));
    // lvalues suppresses moving; rvalues enables moving
}

void move_semantics_example(void) {
    SimpleString a{ 50 };
    a.append_line("We apologize for the");
    SimpleString b{ 50 };
    b.append_line("Last message");
    a.print("a");
    b.print("b");
    b = std::move(a);
    // a is moved-from
    b.print("b");
}

// -------------------------------------------------------------------------------- 

// exercises
// 4-1 - 4-5

#include <ctime>
#include <unistd.h>

struct TimerClass {
    TimerClass(const char* name)
      : timestamp{ std::time(nullptr) },
        name{ name } {}
    // copy semantics
    TimerClass(const TimerClass& other)
      : timestamp{ other.timestamp },
        name{ other.name } {}
    TimerClass& operator=(const TimerClass& other) {
        if (this == &other) return *this;
        timestamp = other.timestamp;
        name = other.name;
        return *this;
    }
    // move semantics
    TimerClass(TimerClass&& other) noexcept 
      : timestamp{ other.timestamp },
        name{ other.name } {
        other.timestamp = 0;
        other.name = nullptr;
      }
    TimerClass& operator=(TimerClass&& other) noexcept {
        if (this == &other) return *this;
        timestamp = other.timestamp;
        name = other.name;
        other.timestamp = 0;
        other.name = nullptr;
        return *this;
    }
    // destructor
    ~TimerClass(void) {
        if (timestamp == 0) return;
        auto current_time = std::time(nullptr);
        auto age = current_time - timestamp;
        auto hours = age / 3600;
        auto minutes = (age % 3600) / 60;
        auto seconds = age % 60;
        printf("Age of the timer %s: %d hours, %d minutes, %d seconds\n",
                name, hours, minutes, seconds);
    }
private:
    std::time_t timestamp;
    const char* name;
};

void test_timer(void) {
    TimerClass t1{"t1"};
    TimerClass t2{ t1 };
    TimerClass t3 = t2;
    TimerClass t4{ std::move(t2) };
    TimerClass t5 = std::move(t3);
    sleep(3);
}

int main(void) {}
