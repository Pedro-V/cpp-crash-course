// ---------------------------------------------------------------------------------
// Algorithms

#include <algorithm>
#include <vector>
#include <string>

using namespace std;

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

// ---------------------------------------------------------------------------------
// Execution policies: Some algorithms can have their execution policy specified
// as either sequential, parallel or parallel vectorized. Sequential and
// parallel have their usual meanings, pertaining how many entities work to
// solve a problem. Vectorized execution policy allows entities to perform work
// in unspecified order, even allowing a single entity to work on multiple
// portions of a problem.

// The <execution> exposes std::execution::{seq,par,par_unseq} for these.
// Execution policies are the first argument for stdlib's algorithms, and the
// default is to use a sequential one.

// ---------------------------------------------------------------------------------
// Non-modifying sequence operations

// all_of: determines if all elements in a sequence meets a predicate.

// any_of: determines if any element in a sequence meets a predicate.

// none_of: determines if no element in a sequence meets a predicate.

// for_each: applies a user-defined function to each element in a sequence.

// for_each_n: applies a user-defined function to the first n elements in a sequence.

// find, find_if and find_if_not: finds the first element matching a predicate.

// find_end: finds last occurence occurrence of a subsequence.

// find_first: finds first occurrence of in a sequence equal to some element in
// another sequence.

// adjacent_find: finds the first repeat in a sequence.

// count: counts the elements that meets a predicate.

// mismatch: finds first mismatch in two sequences.

// equal: determines whether two sequences are equal.

// is_permutation: determines whether two sequences are permutations.

// search: locates a subsequence.

// search_n: locates a subsequence containing n identical, consecutive values.

// ---------------------------------------------------------------------------------
// Mutating sequence operations

// copy: copies one sequence into another.

// copy_n: copies n first elements of a sequence into another.

// copy_backwards: copies the reverse of one sequence into another.

// move: moves one sequence into another.

// move_backward: moves the reverse of one sequence into another.

// swap_ranges: exchanges elements from one sequence into another.

// transform: applies an operation in the elements of one sequence and writes
// them into another.

// replace: replaces certain elements with some new element.

// fill: fills a sequence with some value.

// generate: fills a sequence by invoking a function object.

// remove: removes certain elements from a sequence.

// unique: removes redundant elements from a sequence.

// reverse: reverses the order of a sequence.

// sample: generates random, stable sequences.

// shuffle: generates random permutations.

// ---------------------------------------------------------------------------------
// Sorting and relates operations

// sort: unstable sort of a sequence.

// stable_sort: stable sort of a sequence.

// partial_sort: sorts a sequence into two groups.

// is_sorted: determines whether a sequence is sorted.

// nth_element: places a particular element in a sequence into its correct
// sorted position.

// ---------------------------------------------------------------------------------
// Binary search. Assumes that the target sequence is sorted. 

// lower_bound: finds a partition.

// upper_bound: finds a partition.

// equal_range: finds a range of certain element.

// binary_search: finds a particular element.

// ---------------------------------------------------------------------------------
// Partitioning algorithms. A partitioned sequence contains two contiguous,
// distinct groups of elements. The first element of the second groups is called
// the partition point.

// is_partitioned: determines whether a sequence is partitioned.

// partition: partitions a sequence according to a predicate.

// partition_copy: partitions and copies them.

// stable_partition: partitions stably.

// ---------------------------------------------------------------------------------
// Merging algorithms. Merges two sorted sequences such that the result is sorted.

// merge: merges two sorted sequences.

// ---------------------------------------------------------------------------------
// Exteme-value algorithms.

// min and max: determines a sequence's extrema.

// min_element and max_element: determines a sequence's extrema>

// clamp: bounds a value.

// ---------------------------------------------------------------------------------
// Numeric operations.

#include <numeric>

// Some of the operations allow to pass an operator to customize behavior.
// <functional> provides generic class templates for classic operations.

#include <functional>

// iota: fills a sequence with incremental values.

// accumulate: folds a sequence in order.

// reduce: folds a sequence.

// inner_product: computes the inner product of two sequences.

// adjacent_difference: applies an operation to each pair of neighboring elements.

// partial_sum: generates a partal sum.

// ---------------------------------------------------------------------------------
// Other algorithms.

// heap operations

// set operations on sorted ranges

// other numeric algorithms

