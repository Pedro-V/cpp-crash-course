// ---------------------------------------------------------------------------------
// Up and running

// The majority of content in this chapter is for beginner. I jump straight to
// the exercises.

// ---------------------------------------------------------------------------------
// Includes

#include <cstdio>

// ---------------------------------------------------------------------------------
// Exercises.

// 1-1
int absolute_value(int x) {
    return x < 0 ? -x : x;
}

// 1-4
int sum(int a, int b) {
    return a + b;
}

int main() {
    int x = -10, y = 1, z = 0;
    printf("The absolute values of the variables are:\n"
            "|x| = %d\n"
            "|y| = %d\n"
            "|z| = %d\n",
            absolute_value(x),
            absolute_value(y),
            absolute_value(z));
    printf("And their sum are %d\n", sum(sum(x, y), z));
}
