#ifndef CPU_CACHEPINGPONG_H
#define CPU_CACHEPINGPONG_H

#include <thread>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <chrono>
#include <assert.h>

// Данные: 4 ядра, размер кеш-линии L1 64 байта.
class CachePingPong {
public:
    static size_t getCacheLineSize() {
        return (size_t) sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
    }

    static void worker(size_t position, std::vector<int> &data) {
        for (size_t i = 0; i < ITERATION_NUMBER; ++i) {
            data[position] += 7;
        }
    }

    static long performPingPong(size_t distance) {
        auto startTime = std::chrono::steady_clock::now();

        std::vector<std::thread> threads;
        const size_t threadNumber = std::thread::hardware_concurrency();
        std::vector<int> data(distance * threadNumber);

        for (size_t i = 0; i < threadNumber; ++i) {
            threads.emplace_back(worker, i * distance, std::ref(data));
        }

        for(std::thread &thread : threads) {
            thread.join();
        }

        auto endTime = std::chrono::steady_clock::now();
        auto workTime = std::chrono::milliseconds(
                std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
        return workTime.count();
    }

    static double getAverageExecutionTime(size_t distance) {
        assert(distance >= 1);
        double sum = 0;
        const size_t ATTEMPTS = 10;

        for (int j = 0; j < ATTEMPTS; ++j) {
            auto time = CachePingPong::performPingPong(distance);
            sum += time;
        }
        return sum / ATTEMPTS;
    }
private:
//    static constexpr long CACHE_LINE_SIZE;
    static const size_t ITERATION_NUMBER = 20000000;
};


#endif //CPU_CACHEPINGPONG_H
