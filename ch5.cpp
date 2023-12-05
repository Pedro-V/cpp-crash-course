#include <cstdio>
#include <stdexcept>

// Polymorphism: the property of writing once and reusing with differente (poly)
// types, or forms.

// implementation-inheritance uses base class inheritance feature.
// modern practice emphasizes composition-based polymorphism.

// -------------------------------------------------------------------------------- 

class BaseClass {
    const char* holistic_detective = "Dirk Gently";
public:
    const char* member = "gold";
    int the_answer() const { return 42; }
};

class DerivedClass : public BaseClass {};

void inheritance_example(void) {
    DerivedClass x{};
    printf("The answer is %d\n", x.the_answer());
    printf("%s member\n", x.member);
    // doesnt compile, since DerivedClass doesn't inherit private members
    // printf("%s's Holistic Detective Agency\n", x.holistic_detective);
}

// virtual methods allow that a derived class's implementation should be used if supplied

struct A {
    virtual const char* m() const {
        return "A method";
    }
};

struct B : A {
    const char* m() const override {
        return "B method";
    }
};

void virtual_method_example() {
    A base;
    B derived;
    A& ref1 = base;
    A& ref2 = derived;
    printf("A:                    %s\n", base.m());
    printf("B:                    %s\n", derived.m());
    printf("A&, A-initialized:    %s\n", ref1.m());
    // if it weren't for the virtual keyword, this would fallback to A's method
    printf("A&, B-initialized:    %s\n", ref2.m());
}

// a type that features pure virtual method is called an abstract type
// it cannot be instantiated
class Abstract {
public:
    virtual const char* non_virtual() const {
        return "I'm not a pure virtual method.";
    }
    // pure virtual method: requires that derived classes implement it
    virtual const char* pure_virtual() const = 0;
};

class Concrete : public Abstract {
public:
    const char* pure_virtual() const override {
        return "Implemented pure virtual method\n";
    }
};

// Classes that only define pure virtual methods are called pure-virtual classes.
// interfaces are always pure-virtual classes
class PureVirtualClass {
    virtual const char* only_method() const = 0;
    // We didn't define a virtual destructor to this interface, therefore we
    // risk leaking resources.
};

class NullClass {
    // virtual ~NullClass() = default;
};

class NonNullClass : public NullClass {
public:
    NonNullClass() {
        printf("NonNullClass() invoked.\n");
    }
    ~NonNullClass() {
        printf("~NonNullClass() invoked.\n");
    }
};


void pvc_example(void) {
    // compile-error: can't instantiate abstract types
    // Abstract a{};
    Concrete c{};
    printf(c.pure_virtual());
    NullClass* x{ new NonNullClass{} };
    printf("Deleting x as a NonNullClass*.\n");
    // this leaks memory. can be corrected by creating a virtual destructor in NullClass
    delete x;
}

// -------------------------------------------------------------------------------- 

// Logger interface and implementations

struct Logger {
    virtual ~Logger() = default;
    virtual void log_transfer(long from, long to, double amount) = 0;
};

struct ConsoleLogger : Logger {
    // exercise 5-4
    ConsoleLogger(const char* logger_name) : logger_name{ logger_name } {}

    void log_transfer(long from, long to, double amount) override {
        printf("[cons %s] %ld -> %ld: %f\n", logger_name, from, to, amount);
    }
private:
    const char* logger_name;
};

struct FileLogger : Logger {
    void log_transfer(long from, long to, double amount) override {
        printf("[file] %ld -> %ld: %f\n", from, to, amount);
    }
};

// -------------------------------------------------------------------------------- 

// exercises 
// 5-1

struct AccountDatabase {
    virtual ~AccountDatabase() = default;
    virtual double retrieve(long id) = 0;
    virtual void set(long id, long amount) = 0;
};

// 5-2
struct InMemoryAccountDatabase : AccountDatabase {
    InMemoryAccountDatabase(size_t max_accounts)
      : max_accounts{ max_accounts },
        accounts{ new long[max_accounts] } {}

    ~InMemoryAccountDatabase() {
        delete accounts;
    }

    void check_id(long id) {
        if (id > max_accounts) {
            throw std::length_error{ "ID greater than maximum number of accounts" };
        }
    }

    double retrieve(long id) override {
        check_id(id);
        return accounts[id];
    }

    void set(long id, long amount) override {
        check_id(id);
        accounts[id] = amount;
    }
private:
    long* accounts;
    size_t max_accounts;
};

// -------------------------------------------------------------------------------- 

// there are two approaches for using interfaces, since they cannot be instantiated

// - constructor injection: passes a reference to an interface in the consumer's contructor
// useful when the interface consumed doesn't changes throughout the consumer life cycle
// - property injection: use a setter for an interface member pointer
// useful when the interface consumed changes throughout the consumer life cycle

// we combine the approaches below
struct Bank {
    Bank(Logger* logger, AccountDatabase& acc_db)
      : logger{ logger },
        acc_db{ acc_db } {}

    void make_transfer(long from, long to, double amount) {
        // extra
        auto from_balance = acc_db.retrieve(from);
        auto to_balance = acc_db.retrieve(to);
        acc_db.set(from, from_balance - amount);
        acc_db.set(to, to_balance + amount);
        // could do the rest, however this is just illustrative
        if (logger) logger->log_transfer(from, to, amount);
    }
    void set_logger(Logger* new_logger) {
        logger = new_logger;
    }
private:
    Logger* logger;
    // exercise 5-3
    AccountDatabase& acc_db;
};

void bank_example(void) {
    ConsoleLogger c_logger{ "c1" };
    FileLogger f_logger{};
    InMemoryAccountDatabase db{ 10'000 };
    Bank bank{ &c_logger, db };
    bank.make_transfer(1000, 2000, 49.95);
    bank.set_logger(&f_logger);
    bank.make_transfer(2000, 4000, 20.00);
}

// -------------------------------------------------------------------------------- 

int main(void) {
    virtual_method_example();
}
