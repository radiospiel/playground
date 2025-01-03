#include <benchmark/benchmark.h>
#include <cstdlib>
#include <cstdio>

typedef char * (*UtoaImpl)(unsigned long number, char *buf, size_t buflen);

extern char *loop_utoa(unsigned long number, char *buf, size_t buflen);
extern char *sprintf_utoa(unsigned long number, char *buf, size_t buflen);
extern char *table_utoa(unsigned long number, char *buf, size_t buflen);
	
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
    do_work(state, size, loop_utoa);
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
        verifyCorrectness(sprintf_utoa, loop_utoa);
        printf("*** verifying sprintf_utoa vs table_utoa\n");
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
