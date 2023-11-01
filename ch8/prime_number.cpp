#include <cstdint>
#include "prime_number.h"

using namespace PrimeNumber;

// PrimeNumberIterator
bool PrimeNumberIterator::operator!=(uint64_t x) const {
    return x >= current;
}

bool PrimeNumberIterator::isPrime(uint64_t x) {
    for (uint64_t i = x - 1; i > 1; i--) {
        if (x % i == 0) {
            return false;
        }
    }
    return true;
}

PrimeNumberIterator& PrimeNumberIterator::operator++() {
    auto tmp = current + 1;
    while (!PrimeNumberIterator::isPrime(tmp)) {
        tmp++;
    }
    current = tmp;
    return *this;
}

uint64_t PrimeNumberIterator::operator*() const {
    return current;
}

// PrimeNumberRange

PrimeNumberRange::PrimeNumberRange(uint64_t max) : max{ max } {}

PrimeNumberIterator PrimeNumberRange::begin() const {
    return PrimeNumberIterator{};
}

uint64_t PrimeNumberRange::end() const {
    return max;
}
