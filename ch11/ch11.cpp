/*
 * RAII: Paradigm for the design of dynamic objects, where dynamic storage is
 * acquired in the constructor and released in the destructor. Avoids common
 * errors such as:
 * - Dangling pointers;
 * - Use after free;
 * - Double free.
 *
 * Smart Pointers: Class templates that behave like pointers and implement RAII
 * for dynamic objects. There are five categories, based on their ownership
 * models: scoped, unique, shared, weak and intrusive. 
 */

// ---------------------------------------------------------------------------------
// Terms:
// - Transferable: Can be moved;
// - Exclusive: Cannot be copied;

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <cstdio>
// Exposes unique, shared and weak pointers
#include <memory>

struct DeadMenOfDunharrow {
  DeadMenOfDunharrow(const char *m="")
    : message{ m } {
    oaths_to_fulfill++;
  }

  ~DeadMenOfDunharrow() {
    oaths_to_fulfill--;
  }

  const char* message;
  static int oaths_to_fulfill;
};
int DeadMenOfDunharrow::oaths_to_fulfill{};

// ---------------------------------------------------------------------------------
// Scoped pointers: Non-transferable, exclusive ownership of a single dynamic
// object.

#include <boost/smart_ptr/scoped_ptr.hpp>
#include <boost/smart_ptr/scoped_array.hpp>

using ScopedOathbreakers = boost::scoped_ptr<DeadMenOfDunharrow>;

TEST_CASE("ScopedPtr evaluates to") {
  SECTION("true when full") {
    ScopedOathbreakers aragorn{ new DeadMenOfDunharrow{} };
    REQUIRE(aragorn);
  }

  SECTION("false when empty") {
    ScopedOathbreakers aragorn;
    REQUIRE_FALSE(aragorn);
  }
}

TEST_CASE("ScopedPtr is an RAII wrapper.") {
  REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 0);
  ScopedOathbreakers aragorn{ new DeadMenOfDunharrow{} };
  REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 1);
  {
    ScopedOathbreakers legolas{ new DeadMenOfDunharrow{} };
    REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 2);
  }
  REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 1);
}

TEST_CASE("ScopedPtr supports pointer semantics, like") {
  auto message = "The way is shut";
  ScopedOathbreakers aragorn{ new DeadMenOfDunharrow{ message } };

  SECTION("operator*") {
    REQUIRE((*aragorn).message == message);
  }

  SECTION("operator->") {
    REQUIRE(aragorn->message == message);
  }

  SECTION("get(), which returns a raw pointer") {
    REQUIRE(aragorn.get() != nullptr);
  }
}

TEST_CASE("ScopedPtr supports comparison with nullptr") {
  SECTION("operator==") {
    ScopedOathbreakers legolas{};
    REQUIRE(legolas == nullptr);
  }

  SECTION("operator!=") {
    ScopedOathbreakers aragorn{ new DeadMenOfDunharrow{} };
    REQUIRE(aragorn != nullptr);
  }
}

TEST_CASE("ScopedPtr supports swap") {
  auto message1 = "The way is shut.";
  auto message2 = "Until the time comes.";
  ScopedOathbreakers aragorn{ new DeadMenOfDunharrow{ message1 } };
  ScopedOathbreakers legolas{ new DeadMenOfDunharrow{ message2 } };
  aragorn.swap(legolas);

  REQUIRE(legolas->message == message1);
  REQUIRE(aragorn->message == message2);
}

TEST_CASE("ScopedPtr reset") {
  ScopedOathbreakers aragorn{ new DeadMenOfDunharrow{} };

  SECTION("destructs owned object.") {
    aragorn.reset();
    REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 0);
  }

  SECTION("can replace an owned object.") {
    auto message = "It was made by those who are Dead.";
    auto new_dead_men = new DeadMenOfDunharrow{ message };
    REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 2);
    aragorn.reset(new_dead_men);
    REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 1);
    REQUIRE(aragorn->message == new_dead_men->message);
    REQUIRE(aragorn.get() == new_dead_men);
  }
}

void by_ref(const ScopedOathbreakers&) {}
void by_val(const ScopedOathbreakers) {}

TEST_CASE("ScopedPtr can") {
  ScopedOathbreakers aragorn{ new DeadMenOfDunharrow{} };
  
  SECTION("be passed by reference") {
    by_ref(aragorn);
  }

  SECTION("not be copied") {
    // DOES NOT COMPILE
    //by_val(aragorn);
    //auto son_of_arathorn = aragorn;
  }

  SECTION("not be moved") {
    // DOES NOT COMPILE
    //by_val(std::move(aragorn));
    //auto son_of_arathorn = std::move(aragorn);
  }
}

TEST_CASE("ScopedArray supports operator[]") {
  boost::scoped_array<int> squares{ new int[5]{ 0, 4, 9, 16, 25 } };
  squares[0] = 1;
  REQUIRE(squares[0] == 1);
  REQUIRE(squares[1] == 4);
  REQUIRE(squares[2] == 9);
}


// ---------------------------------------------------------------------------------
// Unique pointer: Transferable, exclusive ownership over a single dynamic
// object. Supports same basic operations as scoped_ptr.

#include <memory>

using UniqueOathbreakers = std::unique_ptr<DeadMenOfDunharrow>;

TEST_CASE("UniquePtr can be used in move") {
  // In previous versions of C++, the std::make_unique function was used to
  // avoid some memory leaks. Current C++ has corrected those errors.
  auto aragorn = std::make_unique<DeadMenOfDunharrow>();

  SECTION("construction") {
    auto son_of_arathorn{ std::move(aragorn) };
    REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 1);
  }

  SECTION("assignment") {
    auto son_of_arathorn = std::make_unique<DeadMenOfDunharrow>();
    REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 2);
    son_of_arathorn = std::move(aragorn);
    // Unique ptr can only own one dynamic object, so the previous owned object
    // get destructed after move assignment.
    REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 1);
  }
}

TEST_CASE("UniquePtr to array supports operator[]") {
  std::unique_ptr<int[]> squares{ new int[5]{1, 4, 9, 16, 25 } };
  squares[0] = 1;
  REQUIRE(squares[0] == 1);
  REQUIRE(squares[1] == 4);
  REQUIRE(squares[2] == 9);
}

TEST_CASE("UniquePtr has a deleter parameter") {
  auto my_deleter = [](int *x) {
    printf("Deleting an int at %p.\n", x);
    delete x;
  };
  std::unique_ptr<int, decltype(my_deleter)> my_up{ new int, my_deleter };
}

TEST_CASE("Deleter for file handling") {
  using FileGuard = std::unique_ptr<FILE, int(*)(FILE*)>;
  auto say_hello = [](FileGuard file) {
    fprintf(file.get(), "HELLO DAVE");
  };

  auto file = fopen("HAL9000", "w");
  if (!file) return;
  FileGuard file_guard{ file, fclose };
  // File open here.
  say_hello(std::move(file_guard));
  // File closed here.
}

// ---------------------------------------------------------------------------------
// Shared pointer: Transferable, non-exclusive ownership over a single dynamic
// object. Supports same basic operations as unique_ptr (such as move semantics).

// Shared pointers check if any other shared pointer owns the dynamic object when
// trying to release it. This way, only the last owner releases the owned object.

using SharedOathbreakers = std::shared_ptr<DeadMenOfDunharrow>;

TEST_CASE("SharedPtr can be used in copy") {
  auto aragorn = std::make_shared<DeadMenOfDunharrow>();
  SECTION("construction") {
    auto son_of_arathorn{ aragorn };
    REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 1);
  }

  SECTION("assignment") {
    SharedOathbreakers son_of_arathorn;
    son_of_arathorn = aragorn;
    REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 1);
  }

  SECTION("assignment, and original gets discarded") {
    auto son_of_arathorn = std::make_shared<DeadMenOfDunharrow>();
    REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 2);
    // SharedPtr has sole ownership, therefore, the original gets deleted after
    // copy assignment.
    son_of_arathorn = aragorn;
    REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 1);
  }
}

// ---------------------------------------------------------------------------------
// Weak pointer: Has no ownership over the dynamic object, being used with the
// shared_ptr. It allows tracking an
// object and converting it into a shared pointer only if the tracked object
// still exists.

// The lock method gets temporary ownership of their tracked object, creating a
// shared pointer. It's either full or empty depending on the status of the
// tracked object.

TEST_CASE("WeakPtr lock() yields") {
  auto message = "The way is shut.";

  SECTION("a shared pointer when tracked object is alive") {
    auto aragorn = std::make_shared<DeadMenOfDunharrow>(message);
    std::weak_ptr<DeadMenOfDunharrow> legolas{ aragorn };
    auto sh_ptr = legolas.lock();
    REQUIRE(sh_ptr->message == message);
    // The use_count method return how many shared_ptrs are pointing to the dynamic
    // object.
    REQUIRE(sh_ptr.use_count() == 2);
  }

  SECTION("empty when shared pointer is empty") {
    std::weak_ptr<DeadMenOfDunharrow> legolas;
    {
      auto aragorn = std::make_shared<DeadMenOfDunharrow>(message);
      legolas = aragorn;
    }
    auto sh_ptr = legolas.lock();
    REQUIRE(sh_ptr == nullptr);
  }
}

// ---------------------------------------------------------------------------------
// Intrusive pointer: A shared pointer to an object with an embedded reference
// count.
#include <boost/smart_ptr/intrusive_ptr.hpp>
using IntrusivePtr = boost::intrusive_ptr<DeadMenOfDunharrow>;
size_t ref_count{};

void intrusive_ptr_add_ref(DeadMenOfDunharrow* d) {
  ref_count++;
}

void intrusive_ptr_release(DeadMenOfDunharrow* d) {
  ref_count--;
  if (ref_count == 0) delete d;
}

TEST_CASE("IntrusivePtr uses an embedded reference counter.") {
  REQUIRE(ref_count == 0);
  IntrusivePtr aragorn{ new DeadMenOfDunharrow{} };
  REQUIRE(ref_count == 1);
  {
    IntrusivePtr legolas{ aragorn };
    REQUIRE(ref_count == 2);
  }
  REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 1);
}

// ---------------------------------------------------------------------------------
// Allocator: Low-level objects for servicing requests for memory. Used when the
// default allocation isn't appropriate for the application.

#include <new>

static size_t n_allocated, n_deallocated;

template<typename T>
struct MyAllocator {
  using value_type = T;

  MyAllocator() noexcept{}

  template<typename U>
  MyAllocator(const MyAllocator<U>&) noexcept {}

  T* allocate(size_t n) {
    auto p = operator new(sizeof(T) * n);
    ++n_allocated;
    return static_cast<T*>(p);
  }

  void deallocate(T* p, size_t n) {
    operator delete(p);
    ++n_deallocated;
  }
};

template<typename T1, typename T2>
bool operator==(const MyAllocator<T1>&, const MyAllocator<T2>&) {
  return true;
}

template<typename T1, typename T2>
bool operator!=(const MyAllocator<T1>&, const MyAllocator<T2>&) {
  return false;
}

TEST_CASE("Allocator") {
  auto message = "The way is shut.";
  MyAllocator<DeadMenOfDunharrow> alloc;
  {
    auto aragorn = std::allocate_shared<DeadMenOfDunharrow>(alloc, message);
    
    REQUIRE(aragorn->message == message);
    REQUIRE(n_allocated == 1);
    REQUIRE(n_deallocated == 0);
  }
  REQUIRE(n_allocated == 1);
  REQUIRE(n_deallocated == 1);
}

// ---------------------------------------------------------------------------------
// Exercises

// Exercise 11-1
TEST_CASE("Exercise 11-1") {
  using FileGuard = std::shared_ptr<FILE>;
  auto say_hello = [](FileGuard file) {
    fprintf(file.get(), "HELLO DAVE");
  };

  auto file = fopen("HAL9000", "w");
  if (!file) return;
  FileGuard file_guard{ file, fclose };
  say_hello(std::move(file_guard));
}

// Exercise 11-2
TEST_CASE("Exercise 11-2") {
  using FileGuard = std::shared_ptr<FILE>;
  auto say_hello = [](FileGuard file) {
    fprintf(file.get(), "HELLO DAVE");
  };

  auto file = fopen("HAL9000", "w");
  if (!file) return;
  FileGuard file_guard{ file, fclose };
  say_hello(file_guard);
  say_hello(file_guard);
}

// Exercise 11-3

struct Hal {
  Hal(std::shared_ptr<FILE> file) : file{ file } {}
  ~Hal() {
    fprintf(file.get(), "Stop, Dave.\n");
  }
  void write_status() {
    fprintf(file.get(), "I'm completely operational.\n");
  }
  std::shared_ptr<FILE> file;
};

// Exercise 11-4

TEST_CASE("Exercise 11-4") {
  auto f1 = fopen("h1", "w");
  auto f2 = fopen("h2", "w");
  auto f3 = fopen("h3", "w");
  if (!(f1 && f2 && f3)) return;

  auto h1 = Hal{ std::shared_ptr<FILE>{ f1, fclose } };
  auto h2 = Hal{ std::shared_ptr<FILE>{ f2, fclose } };
  auto h3 = Hal{ std::shared_ptr<FILE>{ f3, fclose } };

  h1.write_status();
  h2.write_status();
  h3.write_status();
}
