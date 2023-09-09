#include <cstdio>

// -------------------------------------------------------------------------------- 

class Clock {
    int year;
public:
    Clock () {
        this->year = 2023;
    }
    Clock (int year) {
        if (!set_year(year)) {
            this->year = 2023;
        }
    }
    bool set_year(int year) {
        if (year < 2023) return false;
        this->year = year;
        return true;
    }
    int get_year(void) const {
        return year;
    }
};

// -------------------------------------------------------------------------------- 

// references: safer and convenient pointers
// we can use reference types as if they were of the referenced type
// * Changing the value of the reference type isn't possible with references
// * assignment to nullptr shouldn't be done

void add_year(Clock& clock) {
    clock.set_year(clock.get_year() + 1);   // no need for dereference
}

void ref_example(void) {
    Clock clock;
    printf("The year is %d.\n", clock.get_year());
    add_year(clock);                        // clock is implicitly passed by reference
    printf("The year is %d.\n", clock.get_year());
}

void impossible_reseating() {
    int original = 100;
    int& original_ref = original;
    printf("Original: %d\n", original);
    printf("Reference: %d\n", original_ref);

    int new_value = 200;
    /* 
     * original referecen DOESN'T references to
     * new_value now. it simply changes the value
     * it references (original) to new_value                    
     * this exemplifies the impossibility of reseating references
    */
    original_ref = new_value;   
    printf("Original: %d\n", original);
    printf("New value: %d\n", new_value);
    printf("Reference: %d\n", original_ref);
}

// -------------------------------------------------------------------------------- 

// linked lists and some special pointers and modifiers

class Element {
    Element *next{};
    const static int prefix_len = 2;
    char prefix[prefix_len];
    int operating_number;
public:
    void insert_after(Element *new_element) {
        new_element->next = this->next;
        this->next = new_element;
    }

    void set_prefix(const char prefix[prefix_len]) {
        for (size_t i = 0; i < prefix_len; i++) {
            this->prefix[i] = prefix[i];
        }
    }

    void set_operating_number(int operating_number) {
        this->operating_number = operating_number;
    }

    void set_element(const char prefix[2], int operating_number, Element *new_element) {
        set_prefix(prefix);
        set_operating_number(operating_number);
        if (new_element) insert_after(new_element);
    }

    /*
     * by putting const before the function body, we are promising
     * to the compiler that this method doesn't change the object's state
     */
    void print_info(void) const {
        printf("stormtrooper %c%c-%d\n",
                prefix[0],
                prefix[1],
                operating_number);
    }

    void print_forward(void) {
        for (Element *cursor = this; cursor; cursor = cursor->next) {
            cursor->print_info();
        }
    }
};

void trooper_traversal() {
    Element trooper1, trooper2, trooper3;
    trooper1.set_element("TK", 421, &trooper2);
    trooper2.set_element("FN", 2187, &trooper3);
    trooper3.set_element("LS", 5, nullptr);
    trooper1.print_forward();
}

// -------------------------------------------------------------------------------- 

/*
 * const is a qualifier valid for:
 * function arguments; (the argument won't have its value changed)
 * whole methods; (the whole method won't change the current object's state)
 * member variables; (cannot be modified after initialization)
 */

struct Avout {
    const char *name;
    Clock apert;
    Avout (const char *name, int year_of_apert, const char *message)
      : name{ name }, apert{ year_of_apert } {
    }
    void announce() const {
        // since we call apert.get_year, we must tell the compiler that this
        // method also is const. otherwise, it has no guarantees that it won't
        // change `apert` state
        printf("My name is %s and my next apert is %d.\n", name, apert.get_year());
    }
    /*
     * the const keyword only applies to the current object state
     * the object a (a pointer) can be changed, therefore cons't doesn't apply
     * to it
     */
    void read_only(int *a) const {
        (*a)++; // changing a's state
    }
};

/*
 * the below function fails to compile because it changes the avout variable's
 * state.
void liar_function(const Avout& avout) {
    avout.apert.add_year();
}
*/

void avout_example() {
    Avout raz{ "Erasmas", 3010, "Hey\n" };
    Avout jad{ "Jad", 4000, "hey\n" };
    raz.announce();
    jad.announce();

    int x = 10;
    raz.read_only(&x);
}

// -------------------------------------------------------------------------------- 

// auto

void auto_reference_type(void) {
    auto year = 2019;               // int
    auto &year_ref = year;          // int&
    const auto &year_cref = year;   // const int&
    auto *year_ptr = &year;         // int*
    const auto *year_cptr = &year;  // const int*
}

// -------------------------------------------------------------------------------- 

// exercises

// 3-2

char read_from(const char *str, int len, int pos) {
    return (pos > len ? '\0' : str[pos]);
}

void write_to(char *str, int len, int pos, char new_char) {
    pos > len ? 0 : str[pos] = new_char;
}

void bound_checked(void) {
    char lower[] = "abc?e";
    char upper[] = "ABC?E";
    char *upper_ptr = upper;
    write_to(lower, sizeof(lower), 3, 'd');
    write_to(upper_ptr, sizeof(upper), 3, 'D');
    printf("lower: %s\nupper: %s\n", lower, upper);
    write_to(lower, sizeof(lower), 7, 'g');
}

// 3-3

struct Element2 {
    Element2 *next{}, *previous{};
    int operating_number;
    void insert_after(Element2 *new_element) {
        new_element->next = this->next;
        new_element->previous = this;
        this->next = new_element;
    }

    void insert_before(Element2 *new_element) {
        new_element->previous = this->previous;
        new_element->next = this;
        this->previous = new_element;
    }
};

void doubly_linked_traversal(void) {
    Element2 t1, t2, t3;
    t1.operating_number = 102;
    t1.insert_after(&t2);
    t2.operating_number = 333;
    t2.insert_after(&t3);
    t3.operating_number = 682;

    printf("Forward traversal: ");
    for (Element2 *cursor = &t1; cursor; cursor = cursor->next)
        printf("%d ", cursor->operating_number);
    printf("\n");

    printf("Backward traversal: ");
    for (Element2 *cursor = &t3; cursor; cursor = cursor->previous)
        printf("%d ", cursor->operating_number);
    printf("\n");
}

// 3-4

void auto_refs(void) {
    auto original = 100;
    auto& original_ref = original;
    printf("Original: %d\n", original);
    printf("Reference: %d\n", original_ref);

    auto new_value = 200;
    original_ref = new_value;   
    printf("Original: %d\n", original);
    printf("New value: %d\n", new_value);
    printf("Reference: %d\n", original_ref);
}

int main(void) {
    auto_refs();
}
