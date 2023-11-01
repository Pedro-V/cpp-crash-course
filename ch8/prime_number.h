#include <cstdint>

namespace PrimeNumber {
    struct PrimeNumberIterator {
        bool operator!=(uint64_t) const;
        PrimeNumberIterator& operator++();
        uint64_t operator*() const;

    private:
        uint64_t current{ 2 };
        static bool isPrime(uint64_t);
    };

    struct PrimeNumberRange {
        explicit PrimeNumberRange(uint64_t);
        PrimeNumberIterator begin() const;
        uint64_t end() const;
    private:
        const uint64_t max;
    };
}
