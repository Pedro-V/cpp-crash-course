// ---------------------------------------------------------------------------------
// Containers.

/*
 * The Standard Template Library is a portion of the stdlib that comprises
 * containers, algorithms and iterators, which compose nicely in modern C++.
 */

/*
 * Containers are data structures that stores objects in a way that follows
 * specific access rules. All of them are RAII wrappers around the contained
 * object.
 */

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

// ---------------------------------------------------------------------------------
// Sequence containers: Sequential member access.

// array: Fixed-size, contiguous series of elements.

#include <array>

std::array<int, 10> static_array;

TEST_CASE("std::array") {
  REQUIRE(static_array[0] == 0);

  SECTION("uninitialized without braced initializers") {
    std::array<int, 10> local_array;
    REQUIRE(local_array[0] != 0);
  }

  SECTION("initialized with braced initializers") {
    std::array<int, 10> local_array{ 1, 1, 2, 3 };
    REQUIRE(local_array[0] == 1);
  }
}

TEST_CASE("std::array access") {
  std::array<int, 4> fib{ 1, 1, 0, 3 };

  SECTION("operator[] can get and set elements, with possible UB") {
    fib[2] = 2;
    REQUIRE(fib[2] == 2);
    // Undefined Behavior:
    // fib[4] = 5;
  }

  SECTION("at() can get and set elements, with exception-throw") {
    fib.at(2) = 2;
    REQUIRE(fib.at(2) == 2);
    REQUIRE_THROWS_AS(fib.at(4), std::out_of_range);
  }

  SECTION("get can get and set elements, with compile-time bounds checking") {
    std::get<2>(fib) = 2;
    REQUIRE(std::get<2>(fib) == 2);
    // Compile error
    // std::get<4>(fib);
  }
}

TEST_CASE("std::array has convenience methods") {
  std::array<int, 4> fib{ 0, 1, 2, 0 };

  SECTION("front") {
    fib.front() = 1;
    REQUIRE(fib.front() == 1);
    REQUIRE(fib.front() == fib[0]);
  }

  SECTION("back") {
    fib.back() = 3;
    REQUIRE(fib.back() == 3);
    REQUIRE(fib.back() == fib[3]);
  }
  // Some containers don't expose an operator[], so it's best to use them.
}

TEST_CASE("we can obtain a pointer to the first element using") {
  std::array<char, 9> color{ 'o', 'c', 't', 'a', 'r', 'i', 'n', 'e' };
  const auto* color_ptr = color.data();

  SECTION("data") {
    // Prefer using data(), since it avoid UB on empty arrays.
    REQUIRE(*color_ptr == 'o');
  }

  SECTION("address-of the first element") {
    REQUIRE(&color.front() == color_ptr);
    REQUIRE(&color.at(0) == color_ptr);
    REQUIRE(&color[0] == color_ptr);
  }
}

TEST_CASE("std::array's size is static") {
  std::array<int, 2> bin{ 0, 1 };

  constexpr auto size = bin.size();
  REQUIRE(size == 2);
}

TEST_CASE("std::array begin/end form a half-open range") {
  // A half-open range is one where the end points after the last element.
  // Iterators use half-open range, because they are easier to verify that the
  // container has been traversed.
  std::array<int, 0> e{};
  REQUIRE(e.begin() == e.end());
}

TEST_CASE("std::array iterators are pointer-like") {
  std::array<int, 3> easy_as{ 1, 2, 3 };
  auto iter = easy_as.begin();
  REQUIRE(*iter == 1);
  ++iter;
  REQUIRE(*iter == 2);
  ++iter;
  REQUIRE(*iter == 3);
  ++iter;
  REQUIRE(iter == easy_as.end());
}

TEST_CASE("std::array (and any other STL container) can be used as a range expr") {
  std::array<int, 5> fib{ 1, 1, 2, 3, 5 };
  int sum{};
  for (const auto element : fib) {
    sum += element;
  }
  REQUIRE(sum == 12);
}

TEST_CASE("Other std::array operations") {
  SECTION("empty convenience") {
    std::array<int, 0> e{};
    REQUIRE(e.empty());
  }

  SECTION("swap") {
    // Only works when the size template parameters are equal.
    std::array<int, 3> a1 { 1, 2, 3 };
    std::array<int, 3> a2 { 4, 5, 6 };
    std::swap(a1, a2);

    REQUIRE(a1[0] == 4);
    REQUIRE(a1[1] == 5);
    REQUIRE(a1[2] == 6);

    REQUIRE(a2[0] == 1);
    REQUIRE(a2[1] == 2);
    REQUIRE(a2[2] == 3);
  }

  SECTION("filling an array") {
    std::array<int, 4> grades;
    // Copy-assigns the argument to every element of a.
    grades.fill(0);
    
    REQUIRE(grades[0] == 0);
    REQUIRE(grades[1] == 0);
    REQUIRE(grades[2] == 0);
    REQUIRE(grades[3] == 0);
  }
}

// vector: Dynamic-sized, contiguous series of elements.

#include <vector>

TEST_CASE("std::vector supports default construction") {
  // A std::vector optionally can receive a second template parameter, an
  // allocator, for managing its resizing operations.
  std::vector<const char*> vec;
  REQUIRE(vec.empty());
}

TEST_CASE("std::vector supports braced initialization") {
  std::vector<int> fib{ 1, 1, 2, 3, 5 };
  REQUIRE(fib[4] == 5);
}

TEST_CASE("std::vector supports") {
  SECTION("braced initialization") {
    std::vector<int> five_nine{ 5, 9 };
    REQUIRE(five_nine[0] == 5);
    REQUIRE(five_nine[1] == 9);
  }

  SECTION("fill constructor") {
    std::vector<int> five_nines(5, 9);
    REQUIRE(five_nines[0] == 9);
    REQUIRE(five_nines[4] == 9);
  }
}

TEST_CASE("std::vector supports construction from iterators") {
  std::array<int, 5> fib_arr{ 1, 1, 2, 3, 5 };
  std::vector<int> fib_vec(fib_arr.begin(), fib_arr.end());

  REQUIRE(fib_vec[4] == 5);
  REQUIRE(fib_vec.size() == fib_arr.size());
  
  // std::vector supports the some of the operations of std::array, such as
  // operator[], at, get, front, back, data, size, empty and swap.
}

TEST_CASE("std::vector assign replaces existing elements") {
  std::vector<int> message{ 13, 80, 110, 114, 102, 110, 101 };
  REQUIRE(message.size() == 7);

  message.assign({ 67, 97, 101, 115, 97, 114 });
  REQUIRE(message.size() == 6);
}

TEST_CASE("std::vector insert places new elements") {
  std::vector<int> zeros(3, 0);
  auto third_element = zeros.begin() + 2;
  // Inserts just before the element pointed to by the iterator.
  zeros.insert(third_element, 10);
  REQUIRE(zeros[2] == 10);
  REQUIRE(zeros.size() == 4);

  // Inserts invalidates all iterators. That is, third_element isn't valid
  // anymore, since it could be changed after being resized and relocated.
  // This is a major failure in C++'s memory model.
}

TEST_CASE("std::vector push_back places new elements at the end") {
  std::vector<int> zeros(3, 0);
  zeros.push_back(10);
  REQUIRE(zeros[3] == 10);
}

TEST_CASE("std::vector emplace methods forward arguments") {
  std::vector<std::pair<int, int>> factors;

  factors.emplace_back(2, 30);
  factors.emplace_back(3, 20);
  factors.emplace_back(4, 15);
  factors.emplace(factors.begin(), 1, 60);

  REQUIRE(factors[0].first == 1);
  REQUIRE(factors[0].second == 60);

  REQUIRE(factors.size() == 4);
}

#include <cstdint>

TEST_CASE("std::vector exposes size management methods") {
  // std::vector's capacity is how much it can grow until another reallocation
  // is necessary, and its size is how many elements it holds.
  std::vector<std::array<uint8_t, 1024>> kb_store;
  // Returns the maximum size the vector can grow to.
  REQUIRE(kb_store.max_size() > 0);
  REQUIRE(kb_store.empty());

  size_t elements{ 1024 };
  // Reserves at least that much of capacity for std::vector
  kb_store.reserve(elements);
  REQUIRE(kb_store.empty());
  REQUIRE(kb_store.capacity() >= elements);

  kb_store.emplace_back();
  kb_store.emplace_back();
  kb_store.emplace_back();
  REQUIRE(kb_store.size() == 3);

  // Tries to make the capacity just as big enough to hold its size. It's
  // a non-binding call, since it isn't guaranteed to have effect.
  kb_store.shrink_to_fit();
  REQUIRE(kb_store.capacity() >= 3);

  // Deletes all elements and sets the std::vector's size to 0.
  kb_store.clear();
  REQUIRE(kb_store.empty());
  REQUIRE(kb_store.capacity() >= 3);
}

// deque: Double-Ended Queue, with fast insert and remove operations from front
// and back. Its interface is similar to a vector, but its storage model is like a
// hybrid between vector (contiguous) and list (non-contiguous).

#include <deque>

TEST_CASE("std::deque supports front insertion") {
  // All vector methods except capacity, reserve and data are supported.
  std::deque<char> deckard;

  deckard.push_front('a');
  deckard.push_back('i');
  deckard.push_front('c');
  deckard.push_back('n');

  REQUIRE(deckard[0] == 'c');
  REQUIRE(deckard[1] == 'a');
  REQUIRE(deckard[2] == 'i');
  REQUIRE(deckard[3] == 'n');
}

// list: A doubly-linked list.
// No random access (such as operator[] and at()), but fast insert/remove operations
// due to non-contiguous storage model.

#include <list>

TEST_CASE("std::list supports") {
  // All deque methods are supported except random access and shrink_to_fit;
  std::list<int> odds{ 11, 22, 33, 44, 55 };

  SECTION("filtering") {
    odds.remove_if([](int x) { return x % 2 == 0; });
    auto odds_iter = odds.begin();
    REQUIRE(*odds_iter == 11);
    ++odds_iter;
    REQUIRE(*odds_iter == 33);
    ++odds_iter;
    REQUIRE(*odds_iter == 55);
    ++odds_iter;
    REQUIRE(odds_iter == odds.end());
  }

  SECTION("splicing") {
    std::list<int> list2{ 3, 10, 9 };

    // Last two arguments are optional.
    auto odds_iter = odds.begin();
    ++odds_iter;

    odds.splice(odds_iter, list2, list2.begin(), list2.end());
    odds_iter = odds.begin();

    ++odds_iter;
    REQUIRE(*odds_iter == 3);
    ++odds_iter;
    REQUIRE(*odds_iter == 10);
    ++odds_iter;
    REQUIRE(*odds_iter == 9);

    REQUIRE(odds.size() == 8);
  }

  SECTION("removing all equal to a parameter") {
    odds.push_front(11);
    odds.push_front(11);
    
    REQUIRE(odds.size() == 7);
    odds.remove(11);
    REQUIRE(odds.size() == 4);
  }

  SECTION("sorting") {
    odds.sort([](auto x, auto y) { return x > y; });
    REQUIRE(*odds.begin() == 55);
  }

  SECTION("reversing") {
    odds.reverse();
    REQUIRE(*odds.begin() == 55);
  }

  SECTION("consecutive unique") {
    odds.push_back(55);
    odds.push_back(55);
    odds.push_back(55);

    REQUIRE(odds.size() == 8);
    odds.unique();
    REQUIRE(odds.size() == 5);
  }

  SECTION("merging") {
    std::list<int> list2{ 66, 77, 88 };
    odds.merge(list2);
    auto odds_iter = odds.end();
    --odds_iter;

    REQUIRE(odds.size() == 8);
    REQUIRE(*odds_iter == 88);
  }
}

// stack: A Last-In, First-Out (LIFO) data structure.
// It is one of the three container adapters, which encapsulate other containers,
// translating operations to the underlying container's methods, while
// mantaining its conceptual arrangement.
// By default, a stack encapsulates a deque.

#include <stack>

TEST_CASE("std::stack supports push/pop/top operations") {
  std::vector<int> vec{ 1, 3 };
  std::stack<int, decltype(vec)> easy_as(vec);

  REQUIRE(easy_as.top() == 3);
  easy_as.pop();
  easy_as.push(2);
  REQUIRE(easy_as.top() == 2);
  easy_as.pop();
  easy_as.pop();
  REQUIRE(easy_as.empty());
}

// queue: A First-in, First-Out (FIFO) data structure.
// A vector can't be used as a queue because popping from its front is
// inefficient.

#include <queue>

TEST_CASE("std::queue supports push/pop/front/back") {
  std::deque<int> deq{ 1, 2 };
  std::queue<int> easy_as{ deq };

  REQUIRE(easy_as.front() == 1);
  REQUIRE(easy_as.back() == 2);
  easy_as.pop();
  easy_as.push(3);
  REQUIRE(easy_as.front() == 2);
  REQUIRE(easy_as.back() == 3);
  easy_as.pop();
  easy_as.pop();
  REQUIRE(easy_as.empty());
}

// priority queue: A data structure that supports push/pop and keeps a certain
// priority via a user-defined comparator object.
// The wrapped container defaults to vector, and the comparator object defaults
// to std::less, from <functional>.

#include <queue>

TEST_CASE("std::priority_queue supports push/pop") {
  std::priority_queue<double> pq;
  pq.push(1.0);
  pq.push(2.0);
  pq.push(1.5);
  REQUIRE(pq.top() == Approx(2.0));
  pq.pop();
  pq.push(1.0);
  REQUIRE(pq.top() == Approx(1.5));
  pq.pop();
  REQUIRE(pq.top() == Approx(1.0));
  pq.pop();
  pq.pop();
  REQUIRE(pq.empty());
}

// bitset: A data structure that stores a manipulable, fixed-size bit sequence.
// It's optimized for space and offers special convenient operations.

#include <bitset>

TEST_CASE("std::bitset supports integer initialization") {
  std::bitset<4> bs{ 0b0101 };
  REQUIRE(bs[0]);
  REQUIRE_FALSE(bs[1]);
  REQUIRE(bs[2]);
  REQUIRE_FALSE(bs[3]);
}

TEST_CASE("std::bitset supports string initialization") {
  std::bitset<4> bs1{ 0b0110 }, bs2{ "0110" };
  REQUIRE(bs1 == bs2);
}

TEST_CASE("std::bitset supports") {
  std::bitset<4> bs1{ 0b0101 }, bs2{ "1010" };

  SECTION("analysis of bits") {
    REQUIRE(bs1.test(0));
    REQUIRE(bs1.size() == 4);

    REQUIRE_FALSE(bs1.all());
    REQUIRE_FALSE(bs1.none());
    REQUIRE(bs1.any());

    bs1.flip();
    REQUIRE(bs1 == bs2);
  }

  SECTION("setting bits") {
    bs1.set(3, 1);
    REQUIRE(bs1[3] == 1);
    // Bounds-checking.
    REQUIRE_THROWS_AS(bs1.set(4, 1), std::out_of_range);

    bs2.reset();
    REQUIRE(bs2[0] == 0);
    REQUIRE(bs2[1] == 0);
    REQUIRE(bs2[2] == 0);
    REQUIRE(bs2[3] == 0);
  }

  SECTION("conversion") {
    REQUIRE(bs1.to_string() == "0101");
    REQUIRE(bs1.to_ulong() == 0b0101UL);
  }
}

// Boost has other interesting sequential containers.

// ---------------------------------------------------------------------------------
// Associative containers: Very fast element search.

// set: Sorted, unique elements called keys. Implemented via red-black trees.

#include <set>

TEST_CASE("std::set supports") {
  std::set<int> emp;
  std::set<int> fib{ 1, 1, 2, 3, 5 };

  SECTION("default construction") {
    REQUIRE(emp.empty());
  }

  SECTION("braced initialization") {
    // Since it's a set, duplicates are discarde.
    REQUIRE(fib.size() == 4);
  }

  SECTION("copy construction") {
    auto fib_copy{ fib };
    REQUIRE(fib.size() == 4);
    REQUIRE(fib_copy.size() == 4);
  }

  SECTION("move construction") {
    auto fib_moved{ std::move(fib) };
    REQUIRE(fib.empty());
    REQUIRE(fib_moved.size() == 4);
  }

  SECTION("range construction") {
    std::array<int, 5> fib_array{ 1, 1, 2, 3, 5 };
    std::set<int> fib_set(fib_array.cbegin(), fib_array.cend());
    REQUIRE(fib_set.size() == 4);
  }
}

TEST_CASE("std::set allows access") {
  std::set<int> fib{ 1, 1, 2, 3, 5 };
  
  SECTION("with find") {
    REQUIRE(*fib.find(3) == 3);
    REQUIRE(fib.find(100) == fib.end());
  }

  SECTION("with count") {
    REQUIRE(fib.count(3) == 1);
    REQUIRE(fib.count(100) == 0);
  }

  SECTION("with lower_bound") {
    auto itr = fib.lower_bound(3);
    REQUIRE(*itr == 3);
  }
  
  SECTION("with upper_bound") {
    auto itr = fib.upper_bound(3);
    REQUIRE(*itr == 5);
  }

  SECTION("with equal_range") {
    auto pair_itr = fib.equal_range(3);
    REQUIRE(*pair_itr.first == 3);
    REQUIRE(*pair_itr.second == 5);
  }
}

TEST_CASE("std::set allow insertion") {
  std::set<int> fib{ 1, 1, 2, 3, 5 };
  SECTION("with insert") {
    // Copies an existing element into the set.
    fib.insert(8);
    REQUIRE(fib.find(8) != fib.end());
    // Inserting an already-present element has no effect.
    auto [itr, success] = fib.insert(8);
    REQUIRE_FALSE(success);
  }
  
  SECTION("with emplace") {
    // Construct a new element in place.
    fib.emplace(8);
    REQUIRE(fib.find(8) != fib.end());
  }

  SECTION("with emplace_hint") {
    // Gets at hint of where to start from.
    fib.emplace_hint(fib.end(), 8);
    REQUIRE(fib.find(8) != fib.end());
  }
}

TEST_CASE("std::set allows removal") {
  std::set<int> fib { 1, 1, 2, 3, 5 };

  SECTION("with erase") {
    fib.erase(3);
    REQUIRE(fib.find(3) == fib.end());
  }

  SECTION("with clear") {
    fib.clear();
    REQUIRE(fib.empty());
  }
}

// multiset: A set, but with non-unique keys.

#include <set>

TEST_CASE("std::multiset handles non-unique elements") {
  std::multiset<int> fib{ 1, 1, 2, 3, 5 };
  
  SECTION("as reflected by size") {
    REQUIRE(fib.size() == 5);
  }

  SECTION("and count returns values greater than 1") {
    REQUIRE(fib.count(1) == 2);
  }

  SECTION("and equal_range returns non-trivial ranges") {
    auto [begin, end] = fib.equal_range(1);
    REQUIRE(*begin == 1);
    ++begin;
    REQUIRE(*begin == 1);
    ++begin;
    REQUIRE(begin == end);
  }
}

// unordered_set: Unsorted, unique keys. Implemented as a hash table.
// Used whenever there's no natural ordering for the keys and no iteration over
// such an order will be done.

// unordered_multiset: Unsorted, non-unique keys.

#include <unordered_set>
#include <functional>

TEST_CASE("std::hash<long> returns") {
  // A hash function is necessary for operating a hash table.
  // A hasher class template is in <functional>.
  std::hash<long> hasher;
  auto hash_code_42 = hasher(42);

  SECTION("equal hash codes for equal keys") {
    REQUIRE(hash_code_42 == hasher(42));
  }

  SECTION("unequal hash codes for unequal keys") {
    REQUIRE(hash_code_42 != hasher(43));
  }
}

TEST_CASE("std::equal_to<long> returns") {
  // After obtaining a bucket from indexing the hash table with the hash code,
  // it's necessary to determine equality between a key and a bucket element.
  std::equal_to<long> long_equal_to;

  SECTION("true when arguments equal") {
    REQUIRE(long_equal_to(42, 42));
  }

  SECTION("false when arguments unequal") {
    REQUIRE_FALSE(long_equal_to(42, 43));
  }
}

TEST_CASE("std::unordered_set") {
  // All std::set operations except {lower,upper}_bound are available.
  
  // The accepted template parameters are the key type, the hash function
  // type, the equality function type and the allocator type.
  // These last 3 default to std::{hash,equal_to,allocator}, respectively.
  std::unordered_set<unsigned long> sheep(100);

  SECTION("allows bucket count specification on construction") {
    REQUIRE(sheep.bucket_count() >= 100);
    REQUIRE(sheep.bucket_count() <= sheep.max_bucket_count());
    // Load factor is the average size of a bucket, that is, roughly
    // us.size() / us.bucket_count().
    // A load factor near a certain limit triggers an expensive rehashing
    // operation.
    REQUIRE(sheep.max_load_factor() == Approx(1.0));
  }

  SECTION("allows us to reserve space for elements") {
    sheep.reserve(100'000);
    sheep.insert(0);
    REQUIRE(sheep.load_factor() <= Approx(0.00001));
    
    while (sheep.size() < 100'000) {
      sheep.insert(sheep.size());
    }
    // After many insertions, it requires no rehashing because we reserved
    // space for many such elements.
    REQUIRE(sheep.load_factor() <= Approx(1.0));
  }
}

// maps: Sorted, unique key-value pairs.
// Implemented as a red-black tree.
// Works as an associative array, which takes a key (e.g., float or string) rather than a integer index.

#include <map>

auto colour_of_magic = "Colour of Magic";
auto the_light_fantastic = "The Light Fantastic";
auto equal_rites = "Equal Rites";
auto mort = "Mort";

TEST_CASE("std::map supports") {
  // All set operations are supported.
  SECTION("default construction") {
    std::map<const char*, int> emp;
    REQUIRE(emp.empty());
  }
  
  SECTION("braced initialization") {
    std::map<const char*, int> pub_year {
      { colour_of_magic, 1983 },
      { the_light_fantastic, 1986 },
      { equal_rites, 1987 },
      { mort, 1987 },
    };
    REQUIRE(pub_year.size() == 4);
  }
}

TEST_CASE("std::map is an associative array with") {
  std::map<const char*, int> pub_year {
    { colour_of_magic, 1983 },
    { the_light_fantastic, 1986 },
  };

  SECTION("operator[]") {
    REQUIRE(pub_year[colour_of_magic] == 1983);
    
    // Default-construct a new value with the given key and insert it into the map.
    pub_year[equal_rites] = 1987;
    REQUIRE(pub_year[equal_rites] == 1987);

    REQUIRE(pub_year[mort] == 0);
  }

  SECTION("an at method") {
    REQUIRE(pub_year.at(colour_of_magic) == 1983);
    
    REQUIRE_THROWS_AS(pub_year.at(equal_rites), std::out_of_range);
  }
}

TEST_CASE("std::map supports insert") {
  std::map<const char *, int> pub_year;
  pub_year.insert({ colour_of_magic, 1982 });
  REQUIRE(pub_year.size() == 1);

  std::pair<const char*, int> tlfp{ the_light_fantastic, 1986 };
  pub_year.insert(tlfp);
  REQUIRE(pub_year.size() == 2);

  auto [itr, is_new] = pub_year.emplace(the_light_fantastic, 1986);
  REQUIRE(itr->second == 1986);
  REQUIRE_FALSE(is_new);

  pub_year.insert_or_assign(colour_of_magic, 1983);
  REQUIRE(pub_year[colour_of_magic] == 1983);
}

TEST_CASE("std::map supports removal using") {
  std::map<const char*, int> pub_year {
    { colour_of_magic, 1983 },
    { the_light_fantastic, 1986 },
    { equal_rites, 1987 },
    { mort, 1987 },
  };

  SECTION("erase") {
    pub_year.erase(mort);
    REQUIRE(pub_year.find(mort) == pub_year.end());
  }

  SECTION("clear") {
    pub_year.clear();
    REQUIRE(pub_year.empty());
  }
}

// multimap: Sorted, non-unique key-value pairs.
// operator[] and at() are not supported, because the keys are not unique.

TEST_CASE("std::multimap supports non-unique keys") {
  std::array<char, 64> far_out {
    "Far out in the uncharted backwaters of the unfashionable end..."
  };

  std::multimap<char, size_t> indices;
  for(size_t index{}; index < far_out.size(); index++) {
    indices.emplace(far_out[index], index);
  }

  REQUIRE(indices.count('a') == 6);

  auto [itr, end] = indices.equal_range('d');
  REQUIRE(itr->second == 23);
  itr++;
  REQUIRE(itr->second == 59);
  itr++;
  REQUIRE(itr == end);
}

// unordered_{map,multimap}: Provide unsorted, unique/non-unique key-value
// pairs. They also use hash tables, similar to their set counterpart.

// Boost has other interesting associative containers.

// ---------------------------------------------------------------------------------
// Boost Graph Library (BGL).

#include <set>
#include <boost/graph/adjacency_list.hpp>

TEST_CASE("boost::adjacency_list stores graph data") {
  boost::adjacency_list<> graph{};

  auto vertex_1 = boost::add_vertex(graph);
  auto vertex_2 = boost::add_vertex(graph);
  auto vertex_3 = boost::add_vertex(graph);
  auto vertex_4 = boost::add_vertex(graph);

  auto edge_12 = boost::add_edge(vertex_1, vertex_2, graph);
  auto edge_13 = boost::add_edge(vertex_1, vertex_3, graph);
  auto edge_21 = boost::add_edge(vertex_2, vertex_1, graph);
  auto edge_24 = boost::add_edge(vertex_2, vertex_4, graph);
  auto edge_43 = boost::add_edge(vertex_4, vertex_3, graph);

  REQUIRE(boost::num_vertices(graph) == 4);
  REQUIRE(boost::num_edges(graph) == 5);

  auto [begin, end] = boost::adjacent_vertices(vertex_1, graph);
  std::set<decltype(vertex_1)> neighbors_1 { begin, end };

  REQUIRE(neighbors_1.count(vertex_2) == 1);
  REQUIRE(neighbors_1.count(vertex_3) == 1);
  REQUIRE(neighbors_1.count(vertex_4) == 0);
}

// ---------------------------------------------------------------------------------
// Boost Property Trees

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

TEST_CASE("boost::property_tree::ptree stores tree-structured key-value pairs") {
  using namespace boost::property_tree;
  ptree p;
  // Specifies the root node, with 3 children: name, year and features.
  p.put("name", "finfisher");
  p.put("year", "2014");
  // Specifies the features node, with 3 children: process, driver and arch.
  p.put("features.process", "LSASS");
  p.put("features.driver", "mssoundx.sys");
  p.put("features.arch", "32");

  REQUIRE(p.get_child("year").get_value<int>() == 2014);

  const auto file_name = "rootkit.json";
  write_json(file_name, p);

  ptree p_copy;
  read_json(file_name, p_copy);
  REQUIRE(p_copy == p);
}

// ---------------------------------------------------------------------------------
// Initializer lists.

#include <cmath>
#include <stdexcept>
#include <initializer_list>
#include <vector>

size_t square_root(size_t x) {
  const auto result = static_cast<size_t>(sqrt(x));
  if (result * result != x) throw std::logic_error{ "Not a perfect square." };
  return result;
}

template<typename T>
struct SquareMatrix {
  SquareMatrix(std::initializer_list<T> val)
    : dim{ square_root(val.size()) },
      data(dim, std::vector<T>{}) {
    auto itr = val.begin();
    for (size_t row{}; row < dim; row++) {
      data[row].assign(itr, itr+dim);
      itr += dim;
    }
  }

  T& at(size_t row, size_t col) {
    if (row >= dim || col >= dim) {
      throw std::out_of_range{ "Index invalid." };
    }
    return data[row][col];
  }

  const size_t dim;
private:
  std::vector<std::vector<T>> data;
};

TEST_CASE("SquareMatrix and std::initializer_list") {
  SquareMatrix<int> mat {
     1,  2,  3,  4,
     5,  0,  7,  8,
     9, 10, 11, 12,
    13, 14, 15, 16,
  };

  REQUIRE(mat.dim == 4);
  mat.at(1, 1) = 6;
  REQUIRE(mat.at(1, 1) == 6);
  REQUIRE(mat.at(0, 2) == 3);
}

// ---------------------------------------------------------------------------------
// Exercises

// Exercise 13-1

TEST_CASE("Exercise 13-1") {
  printf("Exercise 13-1\n");

  std::vector<int> fib;
  printf("fib capacity: %d\n", fib.capacity());
  fib.reserve(10);

  auto [f1, f2] = std::pair{ 1, 1 };
  fib.push_back(f1);
  fib.push_back(f2);
  for (size_t i{}; i < 20 - 2; i++) {
    auto temp = f2;
    f2 = f1 + f2;
    f1 = temp;
    fib.push_back(f2);
  }

  printf("fib capacity: %d\n", fib.capacity());
  // fib.capacity() == fib.size() because capacity doubles every time a reallocation is
  // necessary.
  // So, since we reserved 10 elements, when we inserted the 11th
  // element, the capacity doubled: 10 * 2 = 20.
  for (const auto f : fib) {
    printf("%d ", f);
  }
  printf("\n\n");
}

// Exercise 13-2

#include <cstddef>
#include <cstdio>

TEST_CASE("Exercise 13-2") {
  printf("Exercise 13-2\n");

  SECTION("Listing 2-9") {
    printf("Listing 2-9\n");
    std::array arr{ 1, 2, 3, 4 };
    printf("The third element is %d\n", arr[2]);
    arr[2] = 100;
    printf("The third element is %d\n", arr[2]);
    printf("\n");
  }

  SECTION("Listing 2-10") {
    printf("Listing 2-10\n");
    unsigned long maximum = 0;
    std::array<unsigned long, 5> values{ 10, 50, 20, 40, 0 };
    for (size_t i{}; i < values.size(); i++) {
      if (values[i] > maximum) maximum = values[i];
    }
    printf("The maximum value is %lu\n", maximum);
    printf("\n");
  }

  SECTION("Listing 2-11") {
    printf("Listing 2-11\n");
    unsigned long maximum = 0;
    std::array<unsigned long, 5> values{ 10, 50, 20, 40, 0 };
    for (const auto& v : values) {
      if (v > maximum) maximum = v;
    }
    printf("The maximum value is %lu\n", maximum);
    printf("\n");
  }
}

// Exercise 13-3
// Implemented in ex13_3.cpp

// Exercise 13-4
// Duplicate of 13-1.

// Exercise 13-5

#include <chrono>
#include <cstdio>
#include <random>

long fib_sum(size_t n) {
  std::vector<long> fib(n);

  auto [f1, f2] = std::make_pair<long, long>(1, 1);
  fib.push_back(f1);
  fib.push_back(f2);
  for (size_t i{}; i < n - 2; i++) {
    auto temp = f2;
    f2 = f1 + f2;
    f1 = temp;
    fib.push_back(f2);
  }

  long sum = 0;
  for (const auto& f : fib) {
    sum += f;
  }

  return sum;
}

long random() {
  static std::mt19937_64 mt_engine{ 102787 };
  static std::uniform_int_distribution<long> int_d{ 1000, 2000 };
  return int_d(mt_engine);
}

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

long cached_fib_sum(const size_t& n) {
  static std::map<long, long> cache;
  if (cache.find(n) == cache.end()) {
    cache[n] = fib_sum(n);
  }
  return cache[n];
}

TEST_CASE("Exercise 13-5") {
  printf("Exercise 13-5\n");

  size_t samples{ 1'000'000 };
  std::chrono::nanoseconds elapsed;
  {
    Stopwatch stopwatch{ elapsed };
    volatile double answer;
    while (samples--) {
      //answer = fib_sum(random());
      answer = cached_fib_sum(random());
    }
  }
  // Something in the original code is wrong, because this doesn't work. It
  // always print something like 6.95e-310 s.
  printf("Elapsed: %g s.\n", elapsed.count() / 1'000'000'000);
}

// Exercise 13-7

#include <initializer_list>
#include <cmath>

template<typename T>
struct Matrix {
  Matrix(size_t rows, std::initializer_list<T> val)
    : rows{ rows },
      cols{ static_cast<size_t>(std::round(val.size() / rows)) },
      data( rows, std::vector<T>{}) {
    auto itr = val.begin();
    for (size_t row{}; row < rows; row++) {
      data[row].assign(itr, itr+cols);
      itr += cols;
    }
  }

  T& at(size_t row, size_t col) {
    if (row >= rows || col >= cols) {
      throw std::out_of_range{ "Index invalid." };
    }
    return data[row][col];
  }

  const size_t rows, cols;
private:
  std::vector<std::vector<T>> data;
};

TEST_CASE("Matrix") {
  SECTION("square") {
    Matrix<int> mat(4, {
       1,  2,  3,  4,
       5,  0,  7,  8,
       9, 10, 11, 12,
      13, 14, 15, 16,
    });

    REQUIRE(mat.rows == 4);
    REQUIRE(mat.cols == 4);
    mat.at(1, 1) = 6;
    REQUIRE(mat.at(1, 1) == 6);
    REQUIRE(mat.at(0, 2) == 3);
  }

  SECTION("not square") {
    Matrix<int> mat(2, {
      1, 2, 3,
     20, 1, 1,
    });

    REQUIRE(mat.rows == 2);
    REQUIRE(mat.cols == 3);
    REQUIRE(mat.at(0, 1) == 2);
  }
}

