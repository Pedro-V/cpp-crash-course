// ---------------------------------------------------------------------------------
// Iterators.

/*
 * Can support:
 * - Reading/writing elements
 * - Iterating forward/backward
 * - Reading multiple times
 * - Accessing random elements
 */

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

// ---------------------------------------------------------------------------------
// Iterator categories

// Output iterators: write and increment.
// Insert iterators: output iterators that wraps a container and transform
// writes into insertions.

#include <deque>
#include <iterator>

TEST_CASE("Insert iterators convert writes into container insertions") {
  std::deque<int> dq;
  auto back_instr = std::back_inserter(dq);
  *back_instr = 2;  // 2
  ++back_instr;
  *back_instr = 4;  // 2 4
  ++back_instr;

  auto front_instr = std::front_inserter(dq);
  *front_instr = 1; // 1 2 4
  ++front_instr;
  
  auto instr = std::inserter(dq, dq.begin()+2);
  *instr = 3;       // 1 2 3 4
  instr++;

  REQUIRE(dq[0] == 1);
  REQUIRE(dq[1] == 2);
  REQUIRE(dq[2] == 3);
  REQUIRE(dq[3] == 4);
}

// Input iterators: read, increment and check for equality.
// Can model input streams.

#include <forward_list>
TEST_CASE("std::forwarrd_list begin and end provide input iterators") {
  // Being const guarantees only read operations.
  std::forward_list<int> easy_as{ 1, 2, 3 };
  auto itr = easy_as.begin();
  REQUIRE(*itr == 1);
  itr++;
  REQUIRE(*itr == 2);
  itr++;
  REQUIRE(*itr == 3);
  itr++;
  REQUIRE(itr == easy_as.end());
}

// Forward iterators: traverse forward multiple times, no write,
// default construct and copy assign.

TEST_CASE("std::forward_list begin and end provide forward iterators") {
  const std::forward_list<int> easy_as{ 1, 2, 3 };
  auto itr1 = easy_as.begin();
  auto itr2{ itr1 };
  int double_sum{};
  while (itr1 != easy_as.end()) {
    double_sum += *(itr1++);
  }
  while (itr2 != easy_as.end()) {
    double_sum += *(itr2++);
  }
  REQUIRE(double_sum == 12);

}

// Bidirectional iterators: Forward iterators that can also iterate backward.
// Provided by array, list, deque, vector and all of the ordered associative
// containers.

#include <list>

TEST_CASE("std::list begin and end provide bidirectional iterators") {
  const std::list<int> easy_as{ 1, 2, 3 };
  auto itr = easy_as.begin();
  REQUIRE(*itr == 1);
  itr++;
  REQUIRE(*itr == 2);
  itr--;
  REQUIRE(*itr == 1);
  REQUIRE(itr == easy_as.begin());
}

// Random-Access iterators: Bidirectional iterators that support random element
// access. Provided by arrays, vectors and deques.

#include <vector>

TEST_CASE("std::vector begin and end provide random-access iterators") {
  const std::vector<int> easy_as{ 1, 2, 3 };
  auto itr = easy_as.begin();
  REQUIRE(itr[0] == 1);
  itr++;
  REQUIRE(*(easy_as.cbegin() + 2) == 3);
  REQUIRE(easy_as.cend() - itr == 2);
}

// Contiguous iterators: Random-access iterator with elements adjacent in
// memory. That is, &itr[n] + i == &itr[n+i]. Provided by arrays and vectors.

// Mutable iterators: Any of the previous iterators which support read-and-write mode,
// that is, are not const.

TEST_CASE("Mutable random-access iterators support writing") {
  std::deque<int> easy_as{ 1, 0, 3 };
  auto itr = easy_as.begin();
  itr[1] = 2;
  itr++;
  REQUIRE(*itr == 2);
}

// ---------------------------------------------------------------------------------
// Auxiliary iterator functions.

#include <iterator>

TEST_CASE("Generic and efficient auxiliary iterators functions") {
  std::vector<unsigned char> mission{
    0x9e, 0xc4, 0xc1, 0x29,
    0x49, 0xa4, 0xf3, 0x14,
    0x74, 0xf2, 0x99, 0x05,
    0x8c, 0xe2, 0xb2, 0x2a,
  };

  // Many of these display UB because of no bounds checking.

  SECTION("advance modifies input iterators") {
    auto itr = mission.begin();
    std::advance(itr, 4);
    REQUIRE(*itr == 0x49);
    std::advance(itr, 4);
    REQUIRE(*itr == 0x74);
    std::advance(itr, -8);
    REQUIRE(*itr == 0x9e);
  }

  SECTION("next/prev returns iterators at given offsets from forwards") {
    // Return a new, offset-dislocated iterator without changing the
    // original forward iterator.
    auto itr1 = mission.begin();
    std::advance(itr1, 4);
    REQUIRE(*itr1 == 0x49);

    auto itr2 = std::next(itr1);
    REQUIRE(*itr2 == 0xa4);

    auto itr3 = std::next(itr1, 4);
    REQUIRE(*itr3 == 0x74);

    REQUIRE(*itr1 == 0x49);
  }

  SECTION("distance returns the number of elements between input iterators") {
    auto eight = std::next(mission.begin(), 8);
    auto fifth = std::prev(eight, 3);
    // If the iterators are not random access, the 2nd not being after the 1st
    // can cause UB.
    REQUIRE(std::distance(fifth, eight) == 3);
    REQUIRE(std::distance(eight, fifth) == -3);
  }

  SECTION("iter_swap swaps elements pointed-to by forward iterators") {
    std::vector<int> easy_as{ 3, 2, 1 };
    std::iter_swap(easy_as.begin(), std::next(easy_as.begin(), 2));
    REQUIRE(easy_as[0] == 1);
    REQUIRE(easy_as[1] == 2);
    REQUIRE(easy_as[2] == 3);
  }
}
// ---------------------------------------------------------------------------------
// Additional iterator adapters

struct Movable {
  Movable(int id) : id{ id } {}
  Movable(Movable&& m) {
    id = m.id;
    m.id = -1;
  }
  int id;
};

TEST_CASE("move iterators convert accesses into move operations") {
  std::vector<Movable> donor;
  donor.emplace_back(1);
  donor.emplace_back(2);
  donor.emplace_back(3);

  // Uses range constructor of vector with two move iterators, causing every
  // iterator operations into move ones, moving the whole donor vector over to
  // recipient.
  std::vector<Movable> recipient{
    std::make_move_iterator(donor.begin()),
    std::make_move_iterator(donor.end())
  };

  REQUIRE(donor[0].id == -1);
  REQUIRE(donor[1].id == -1);
  REQUIRE(donor[2].id == -1);
  REQUIRE(recipient[0].id == 1);
  REQUIRE(recipient[1].id == 2);
  REQUIRE(recipient[2].id == 3);
}

TEST_CASE("reverse iterators can initialize containers") {
  std::list<int> original{ 3, 2, 1 };
  std::vector<int> easy_as{ original.crbegin(), original.crend() };
  REQUIRE(easy_as[0] == 1);
  REQUIRE(easy_as[1] == 2);
  REQUIRE(easy_as[2] == 3);
}

TEST_CASE("make_reverse_iterator convert a normal iterator") {
  std::list<int> original{ 3, 2, 1 };
  auto begin = std::make_reverse_iterator(original.cend());
  auto end = std::make_reverse_iterator(original.cbegin());
  std::vector<int> easy_as{ begin, end };
  REQUIRE(easy_as[0] == 1);
  REQUIRE(easy_as[1] == 2);
  REQUIRE(easy_as[2] == 3);
}


// ---------------------------------------------------------------------------------
// Exercises

// Exercise 14-1

TEST_CASE("prev returns iterators at given offsets") {
  std::vector<unsigned char> mission{
    0x9e, 0xc4, 0xc1, 0x29,
    0x49, 0xa4, 0xf3, 0x14,
    0x74, 0xf2, 0x99, 0x05,
    0x8c, 0xe2, 0xb2, 0x2a,
  };

  auto itr1 = mission.begin();
  std::advance(itr1, 4);
  REQUIRE(*itr1 == 0x49);

  auto itr2 = std::prev(itr1);
  REQUIRE(*itr2 == 0x29);

  auto itr3 = std::prev(itr1, 4);
  REQUIRE(*itr3 == 0x9e);

  REQUIRE(*itr1 == 0x49);
}

// Exercise 14-2

template <typename Iterator>
int sum(Iterator begin, Iterator end) {
  int result = 0;
  for (Iterator it = begin; it != end; ++it) {
    result += *it;
  }
  return result;
}

// Exercise 14-3

#include <chrono>
#include <tuple>

struct Stopwatch {
  Stopwatch(std::chrono::nanoseconds& result)
    : result{ result },
    start{ std::chrono::system_clock::now() } {} 

  ~Stopwatch() {
    result = std::chrono::system_clock::now() - start;
  }
private:
  std::chrono::nanoseconds& result;
  const std::chrono::time_point<std::chrono::system_clock> start;
};

TEST_CASE("std::advance runtime performance") {
  printf("Exercise 14-3\n");
  const size_t n1{ 100'000 }, n2{ 1'000'000 };
  std::chrono::nanoseconds elapsed;

  std::forward_list<int> ls1(n1, 0), ls2(n2, 0);
  std::vector<int> v1(n1, 0), v2(n2, 0);
  auto [itr1, itr2, itr3, itr4] = std::make_tuple(
      ls1.begin(), ls2.begin(),
       v1.begin(),  v2.begin()
  );
  {
    Stopwatch sw{ elapsed };
    std::advance(itr1, n1);
  }
  printf("Took %gns.\n", elapsed.count());
  {
    Stopwatch sw{ elapsed };
    std::advance(itr2, n2);
  }
  printf("Took %gns.\n", elapsed.count());
  {
    Stopwatch sw{ elapsed };
    std::advance(itr3, n1);
  }
  printf("Took %gns.\n", elapsed.count());
  {
    Stopwatch sw{ elapsed };
    std::advance(itr4, n2);
  }
  printf("Took %gns.\n", elapsed.count());

  printf("\n");
}
