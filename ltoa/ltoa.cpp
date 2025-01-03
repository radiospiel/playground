#include <benchmark/benchmark.h>
#include <iostream>

// Example implementation 1: A simple sum function with a payload
void implementation_1(int payload) {
    int sum = 0;
    for (int i = 0; i < 1000; i++) {
        sum += i * payload;  // Modify the behavior based on payload
    }
}

// Example implementation 2: A similar sum function with a different payload influence
void implementation_2(int payload) {
    int sum = 0;
    for (int i = 0; i < 1000; i++) {
        sum += (i + payload);  // Modify the behavior based on payload
    }
}

// Example implementation 3: Another variant of the sum function with payload
void implementation_3(int payload) {
    int sum = 0;
    for (int i = 0; i < 1000; i++) {
        sum += (i - payload);  // Modify the behavior based on payload
    }
}

// Benchmark for implementation 1 with payload
static void BM_implementation_1(benchmark::State& state) {
    int payload = state.range(0);  // Get the payload value from the benchmark range
    for (auto _ : state) {
        implementation_1(payload);
    }
}

// Benchmark for implementation 2 with payload
static void BM_implementation_2(benchmark::State& state) {
    int payload = state.range(0);  // Get the payload value from the benchmark range
    for (auto _ : state) {
        implementation_2(payload);
    }
}

// Benchmark for implementation 3 with payload
static void BM_implementation_3(benchmark::State& state) {
    int payload = state.range(0);  // Get the payload value from the benchmark range
    for (auto _ : state) {
        implementation_3(payload);
    }
}

// Register the benchmarks with specific ranges (payloads)
BENCHMARK(BM_implementation_1)->Arg(1)->Arg(10)->Arg(100);
BENCHMARK(BM_implementation_2)->Arg(1)->Arg(10)->Arg(100);
BENCHMARK(BM_implementation_3)->Arg(1)->Arg(10)->Arg(100);

// Main function to run the benchmarks
BENCHMARK_MAIN();
