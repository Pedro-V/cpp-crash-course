#include <cstdio>

// -------------------------------------------------------------------------------- 

// scoped enum (c++)
enum class Race {
    Dinan,
    Teklan,
    Ivyn,
    Moiran,
    Camite,
    Julian,
    Aidan
};

// unscoped enum (same)
enum Flavour {
    orange,
    vanilla,
    lemon,
    chocolate
};

void treat_race(void) {
    Race langobard_race = Race::Teklan;
    // switch cases (same)
    switch (langobard_race) {
        case Race::Dinan:
            printf("You work hard.");
            break;
        case Race::Teklan:
            printf("You are very strong.");
            break;
        case Race::Ivyn:
            printf("You are a great leader.");
            break;
        default:
            printf("Error: unknown race!");
            break;
    }
}

// -------------------------------------------------------------------------------- 

// plain-old data classes (PODs) (diff)
struct Book {
    // general rule: order the members by theirs sizes on the definition
    char name[256];
    int year;
    int pages;
    bool hardcover;
};

void pod_declaration_example(void) {
    // diff: no need for `struct` before POD name
    Book neuromancer;
    neuromancer.pages = 271;
    printf("Neuromancer has %d pages.\n", neuromancer.pages);
}

// -------------------------------------------------------------------------------- 

// classes (c++)

struct ClockOfTheLongNow {
    // by default in `struct` classes, members assume public visibility
    // method
    void add_year(void) {
        year++;
    }
    int year;
};

void track_year(void) {
    ClockOfTheLongNow clock;
    clock.year = 2023;
    clock.add_year();
    clock.add_year();
    printf("year: %d\n", clock.year);
}

// -------------------------------------------------------------------------------- 

// access control, enforcing encapsulation, class invariants

// classes declared with the `class` keyword make members private by default
// this is the only difference between `struct` and `clock`

class Clock2 {
    int year;
public:
    // if this class didn't had a constructor, the class invariant of the
    // year < 2023 wouldn't actually hold under any circumstance:
    // declaring a Clock2 object would assign random values (the ones that are in
    // the stack at the moment of the declaration) to the year variable
    // there can be as many Constructors, as long as their types differ
    Clock2 (int year) {
        if (!set_year(year)) {
            // bad: silently corrects to 2023
            this->year = 2023;
        }
    }
    Clock2 () {
        this->year = 2023;
    }
    void add_year() {
        year++;
    }
    bool set_year(int new_year) {
        if (new_year < 2023) return false;
        year = new_year;
        return true;
    }
    int get_year() {
        return year;
    }
};

void track_year2(void) {
    Clock2 clock;
    printf("Default year: %d\n", clock.get_year());
    if (!clock.set_year(2022)) {
        clock.set_year(2023);
    }
    clock.add_year();
    printf("year: %d\n", clock.get_year());
}

// -------------------------------------------------------------------------------- 

// initialization

void primitive_initialization(void) {
    // 0-initialization
    int a = 0;  // explicit initialization with literals
    int b{}; // braced-initialization. always applicable
    int c = {}; // equals-plus-braces initialization
    int d;  // non-reliable

    // arbitrary values
    int e = 42;
    int f{ 42 };
    int g = { 42 };
    int h(42);
}

void pod_initialization(void) {
    // 0-initialization
    // this below is illegal
    // Book b1 = 0;
    // all fields set to 0
    Book b2{};
    Book b3 = {};
    // sets first 3 fields to arbitrary values. 4th field (bool hardcover) is
    // initialized to 0
    // this omission can only occur from right to left. trying to emit the pages
    // field only wouldn't compile
    Book b4{"Pride and Prejudice", 1813, 200};
    // all fields set
    Book b6{"Introduction to Algorithms", 2019, 1200, true};
    // initializes specified fields, 0-initializes others
    Book b7 = {.year = 2020};
    printf("%s, %d, %d, %b\n", b7.name, b7.year, b7.pages, b7.hardcover);
    // parentheses-initialization below is illegal
    // Boobk b8("Ruby", 2020);
    // enum initialization
    Race dog{ Race:: Julian };
}

void array_initialization(void) {
    int arr1[]{1, 2, 3};    // length 3: 1, 2, 3
    int arr2[5]{};          // length 5: 0, 0, 0, 0, 0
    int arr3[5]{1, 2, 3};   // length 5: 1, 2, 3, 0, 0
    int arr4[5];            // length 5: ?, ?, ?, ?, ?
}

class Taxonomist {
    // class members can't be initialized using parentheses
    bool b = true;
    int i{ 1970 };
    char string[8]{ "hello!" };
public:
    Taxonomist() {
        printf("No argument\n");
    }
    Taxonomist(char x) {
        printf("char: %c\n", x);
    }
    Taxonomist(int x) {
        printf("int: %d\n", x);
    }
    Taxonomist(float x) {
        printf("float: %f\n", x);
    }
};

void class_initialization(void) {
    Taxonomist t1, t2{ 'c' }, t3{ 65537 }, t4{ 6.02e23f };
    Taxonomist t5( 'g' ), t6 = { 'l' }, t7{};
    // this is special. it produces no output.
    // the compiler thinks it is a yet-to-be-defined function t8, which takes no
    // arguments and returns a Taxonomist object. This is the "most vexing parse" problem
    Taxonomist t8();

    // narrowing conversion
    float a{ 1 }, b{ 2 };
    // goes from 0.5 to 0. compiler emits no warning
    int narrowed_result(a / b);
    // goes from 0.5 to 0. compiler emits warning
    int result{ a / b };
}

// general rule: use braced initialization (aka uniform initialization) everywhere in C++
// unfortunately, C doesn't accept the same syntax everywhere

// -------------------------------------------------------------------------------- 

// destructor
// rarely called explicitly, since the compiler takes care of this
// doesn't take parameters
// the default destructor does nothing

class Earth {
public:
    ~Earth() {
        printf("Making way for hyperspace bypass\n");
    }
};

// -------------------------------------------------------------------------------- 

// exercises
// 2-1

enum class Operation {
    Add,
    Subtract,
    Multiply,
    Divide
};

// 2-2, 2-3

struct Calculator {
    Calculator(Operation op) {
        this->op = op;
    }

    int calculate(int a, int b) {
        int result;
        switch (op) {
            case Operation::Add:
                result = a + b;
                break;
            case Operation::Subtract:
                result = a - b;
                break;
            case Operation::Multiply:
                result = a * b;
                break;
            case Operation::Divide:
                result = a / b;
                break;
            default:
                break;
        }
        printf("Result is %d\n", result);
        return result;
    }
private:
    Operation op;
};

// 2-4

void calc_initialization(void) {
    Calculator c1{ Operation::Add };
    c1.calculate(10, 3);
    Calculator c2( Operation::Divide );
    c2.calculate(10, 3);
}

int main(int argc, char **argv) {
    calc_initialization();
}
