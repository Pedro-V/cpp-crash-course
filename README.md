# [C++ Crash Course](https://www.amazon.com/C-Crash-Course-Josh-Lospinoso/dp/1593278888/ref=sr_1_1?crid=254N8A8UJMOYA&keywords=c%2B%2B+crash+course+lospinoso&qid=1694284767&sprefix=c%2B%2B+crash+course+lospinoso%2Caps%2C228&sr=8-1) Notes


## Feature compatibility

On this tree, features are defined in the following way:

- `c`: C specific. Compiles with C compilers, doesn't with C++ ones.
- `same`: Compiles with both C++ and C compilers, same implementation.
- `diff`: Compiles with both C++ and C compilers, but different implementation
  (generated code) by main compilers
- `c++`: C++ specific. Compiles with C++ compilers, doesn't with C ones.

Unnanotated features are assumed to be `c++`.
