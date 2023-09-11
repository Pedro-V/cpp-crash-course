#include <cstdio>

// compile-time polymorphism can be achieved in C++ with templates
// template is a class or function with template parameters, which can be any type

template<typename X, typename Y, typename Z>
struct MyTemplateClass {
    X foo (Y&);
private:
    Z* member;
};

template<typename X, typename Y, typename Z>
X my_template_function(Y& arg1, const Z* arg2) {}

void template_instantiation(void) {
    MyTemplateClass<int, float, char*> instantiated_template{};
    int x = 10, y = -10;
    my_template_function<void, int, int>(y, &x);
}

// -------------------------------------------------------------------------------- 

// named conversions
// used when we can't use implicit conversions nor constructor to get the types we need

// const_cast
void carbon_thaw(const int& encased_solo) {
    //encased_solo++; // can't modify const
    auto& hibernation_sick_solo = const_cast<int&>(encased_solo);
    hibernation_sick_solo++;
}

void test_const_cast(void) {
    int x = 10;
    carbon_thaw(x);
    printf("x = %d\n", x);
}

int main(void) {
}
