// #include <benchmark/benchmark.h>
// #include <iostream>

#include <stdlib.h>

#include <benchmark/benchmark.h>
#include <vector>
#include <algorithm>
#include <cstdlib>


#define MAX_LENGTH 100000
#define MAX_NUMBER_LENGTH 24

// 
// 
struct Payload {
    size_t length;
    unsigned long entries[MAX_LENGTH];
};

// Helper function to generate a Payload
Payload CreatePayload(size_t length) {
    Payload payload;
    payload.length = length;
    for (size_t i = 0; i < length; ++i) {
        payload.entries[i] = std::rand() % 1000; // Random values between 0 and 999
    }
    // explicit test cases
    auto idx = 0;
    payload.entries[idx++] = 950;
    payload.entries[idx++] = 0;
    return payload;
}

typedef char * (*UtoaImpl)(unsigned long number, char *buf, size_t buflen);

static char *ruby_utoa(unsigned long number, char *buf, size_t buflen) {
    static const char digits[] = "0123456789";
    char *tmp = buf + buflen;

    *tmp-- = '\0';
    do {
        *tmp-- = digits[number % 10];
        assert(tmp >= buf);
    } while (number /= 10);
    return tmp + 1;
}

static char *sprintf_utoa(unsigned long number, char *buf, size_t buflen) {
    snprintf(buf, buflen, "%ld", number);
    return buf;
}

// a single number expansion has 10 characters. It is important that this is a "char[]"
#define EXPANSION_SIZE 10

class BcdNumber {
public:
    char bcd[EXPANSION_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char *_to_s = nullptr;

    void set(unsigned long number) {
        char *ptr = bcd + (EXPANSION_SIZE - 1);
        while (ptr > bcd) {
            *ptr = (char) (number % 10);
            number /= 10;
            ptr--;
        }
    }

    inline void add(const BcdNumber &bcdNumber) {
        for (unsigned long i = 0; i < EXPANSION_SIZE; ++i) {
            bcd[i] += (char) bcdNumber.bcd[i];
        }
    }

    std::string _inspect;
    const char* inspect() {
        _inspect= "[";
        for (unsigned long i = 0; i < EXPANSION_SIZE; ++i) {
            if (i > 0) {
                _inspect += ", ";
            }
            _inspect += inspect_digit(bcd[i]);
        }
            _inspect += "]";
        return _inspect.c_str();
    }
    std::string inspect_digit(char ch) {
        if (ch >= '0' && ch <= '9') {
            return "\"" + std::string(1, '0' + ch) + "\"";
        } else if (ch >= 0 && ch <= 9) {
            return std::string(1, '0' + ch);
        } else {
            return "?";
        }
    }

private:
    void applyCarryOver() {
        unsigned idx = EXPANSION_SIZE - 1;

        while (idx > 0) {
            while (bcd[idx] > 9) {
                bcd[idx] -= 10;
                bcd[idx-1] += 1;
            }

            idx--;
        }
    }

public:
    char *to_s(char* buf, unsigned buflen) {
        assert(buflen >= EXPANSION_SIZE);

        applyCarryOver();

        // find the first digit which is not `0`, but stop one char early just in case.
        char *s = bcd;
        while (s < bcd + EXPANSION_SIZE-1 && !*s) {
            ++s;
        }

        // start conversion there.
        char* d = buf;
        while (s < bcd + EXPANSION_SIZE) {
            *d++ = *s++ + (char)'0';
        }
        *d = '\0';
        return buf;
    }
};

// a multiplication table holds expansions calculated by iterating over all numbers between 0..15, and multiplying it with a factor.
struct MultiplicationTable {
    BcdNumber numbers[16];

    void setup(unsigned factor) {
        for (size_t i = 0; i < 16; ++i) {
            numbers[i].set(i * factor);
        }
    }
};

struct MultiplicationTables {
    MultiplicationTable tables[8]{};

    MultiplicationTables() {
        unsigned factor = 1;
        for (auto &table: tables) {
            table.setup(factor);
            factor *= 16;
        }
    }
};

static MultiplicationTables multiplicationTables;

char *table_utoa(unsigned long number, char *buf, size_t buflen) {
    BcdNumber sum;
    const unsigned rounds = 8;
    for (unsigned i = 0; i < rounds; ++i) {
        // get multiplication table for this round
        MultiplicationTable *pTable = multiplicationTables.tables + i;
        auto xdigit = number % 16;
        number /= 16;
        BcdNumber &bcdNumber = pTable->numbers[xdigit];
        sum.add(bcdNumber);
    }

    return sum.to_s(buf, buflen);
}

static void do_work(benchmark::State &state, size_t size, UtoaImpl impl) {
    // Precompute the Payload outside the benchmark loop
    Payload payload = CreatePayload(size);

    for (auto _: state) {
        char buf[MAX_NUMBER_LENGTH + 1];
        auto entries = payload.entries;
        auto length = payload.length;

        for (size_t i = 0; i < length; i++) {
            auto ptr = impl(entries[i], buf, sizeof(buf) - 1);
            benchmark::DoNotOptimize(ptr);
        }
    }
}

// Benchmark function
static void BM_ProcessPayloadViaRubyUtoa(benchmark::State &state) {
    auto size = static_cast<size_t>(state.range(0));
    do_work(state, size, ruby_utoa);
}

static void BM_ProcessPayloadViaSprintf(benchmark::State &state) {
    auto size = static_cast<size_t>(state.range(0));
    do_work(state, size, sprintf_utoa);
}


static void BM_ProcessPayloadViaTable(benchmark::State &state) {
    auto size = static_cast<size_t>(state.range(0));
    do_work(state, size, table_utoa);
}

class CustomValidations {
public:
    CustomValidations() {
        // Print warning that assert is non functional
        checkAssertState();

        // verify correctness
        printf("*** verifying sprintf_utoa vs ruby_utoa\n");
        verifyCorrectness(sprintf_utoa, ruby_utoa);
        verifyCorrectness(sprintf_utoa, table_utoa);
        printf("*** Verified correctness.\n");
    }

private:
    static void checkAssertState() {
#ifdef NDEBUG
        fprintf(stderr, "*** NDEBUG is enabled: assert's are non functional.\n");
#else
        fprintf(stderr, "*** NDEBUG is disabled: assert's are enabled and might affect performance..\n");
#endif
    }

    static void verifyCorrectness(UtoaImpl impl1, UtoaImpl impl2) {
        Payload payload = CreatePayload(1000);
        auto entries = payload.entries;
        auto length = payload.length;

        for (size_t i = 0; i < length; i++) {
            char buf1[MAX_NUMBER_LENGTH + 1];
            char buf2[MAX_NUMBER_LENGTH + 1];

            auto ptr1 = impl1(entries[i], buf1, sizeof(buf1) - 1);
            auto ptr2 = impl2(entries[i], buf2, sizeof(buf2) - 1);
            if (strcmp(ptr1, ptr2) != 0) {
                printf(R"(conversion is incorrect for %lu: "%s" vs "%s")", entries[i], ptr1, ptr2);
                assert(!strcmp(ptr1, ptr2));
            }
        }
    }
};

CustomValidations customValidations;

// Register benchmark with various payload sizes
BENCHMARK(BM_ProcessPayloadViaRubyUtoa)->Arg(1000); // Max payload (assuming MAX_LENGTH == 1000)
BENCHMARK(BM_ProcessPayloadViaSprintf)->Arg(1000); // Max payload (assuming MAX_LENGTH == 1000)
BENCHMARK(BM_ProcessPayloadViaTable)->Arg(1000); // Max payload (assuming MAX_LENGTH == 1000)

BENCHMARK_MAIN();
