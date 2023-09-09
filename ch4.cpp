#include <cstdio>

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

static Tracer t1{ "Static variable" };
thread_local Tracer t2{ "Thread-local variable" };

void life_cycle_tracker(void) {
    printf("A\n");
    Tracer t3{ "Automatic variable" };
    printf("B\n");
    const auto* t4 = new Tracer{ "Dynamic variable 1" };
    delete t4;
    // memory leak below
    const auto& t5 = new Tracer{ "Dynamic variable 2" };
    auto t6 = t1;
    auto& t7 = t1;
    auto& t8 = t2;
    printf("C\n");
}

#define __lft \
life_cycle_tracker(); \
printf("D\n");

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

int main(void) {
    exception_main();
}
