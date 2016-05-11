#include <atomic>
#include <thread>
#include <vector>
#include <iostream>
#include <mutex>
#include <chrono>
#include <assert.h>
#include <algorithm>

using std::thread;
using std::atomic;
using std::vector;
using std::cout;
using std::mutex;

template<bool relaxed>
class Futex {
public:

    Futex() {
        owner.store(NO_OWNER);
    }

    Futex(const Futex &) = delete;

    void lock() {
        int desired = NO_OWNER;
        int myThreadId = (int) pthread_self();
        while (!owner.compare_exchange_weak(desired, myThreadId)) {
            desired = NO_OWNER;
        }
    }

    void unlock() {
        owner.store(NO_OWNER);
    }

private:
    const int NO_OWNER = 0;
    atomic<int> owner;
};

template<>
class Futex<true> {
public:

    Futex() {
        owner.store(NO_OWNER);
    }

    Futex(const Futex &) = delete;

    void lock() {
        int desired = NO_OWNER;
        int myThreadId = (int) pthread_self();
        // Во первых, необходимо чтобы все что происходило в критической секции после lock стало видимо после unlock.
        // Для этого необходимо использовать упорядочение освобождение-захват
        // (упорядочение освобождение-поглощение по этой причине будет недостаточно).
        // Во вторых, потоки, пытающиеся захватить futex, должны видеть захват(write), если какой-то поток решил, что он захватил
        // и следовательно записал в owner себя. В противном случае, без упорядочения в этом месте несколько потоков могут начать
        // исполнение критической секции. Поэтому необходимо не просто release, а release совмещенный с acquire.
        // P.S. volatile дает acquire-release упорядоченность только в MS VS.
        // P.S.S weak используется потому, что на платформах, где weak может давать false, это позволит избавиться от вложенного цикла,
        // а так как вычисление аргумента в данном случае не занимает значительного (вообще ни какого) времени, то ложные несрабатывания не повредят.

        while (!owner.compare_exchange_weak(desired, myThreadId, std::memory_order_acq_rel)) {
            desired = NO_OWNER;
        }
    }

    void unlock() {
        owner.store(NO_OWNER, std::memory_order_release);
    }

private:
    const int NO_OWNER = 0;
    atomic<int> owner;
};

template<typename T>
void incrementer(T &locker, size_t &myCount, int &globalValue, int maxValue) {
    bool terminate = false;
    while (!terminate) {
        locker.lock();
        if (globalValue < maxValue) {
            ++globalValue;
            ++myCount;
        } else {
            terminate = true;
        }
        locker.unlock();
    }
}

template<typename T>
size_t threadsCountToN(size_t threadNumber, int maxValue) {
    vector<std::thread> threads;
    vector<size_t> results(threadNumber);
    T locker;
    int global = 0;

    for (size_t i = 0; i < threadNumber; ++i) {
        threads.emplace_back(incrementer<T>, std::ref(locker), std::ref(results[i]), std::ref(global), maxValue);
    }

    for (std::thread &thread : threads) {
        thread.join();
    }

    size_t operationCount = 0;
    for (size_t localCount : results) {
        operationCount += localCount;
    }
    return operationCount;
}

template<typename T>
long measureSingleRun(size_t threadNumber, int maxGlobal) {
    auto timeStart = std::chrono::high_resolution_clock::now();

    size_t operationCount = threadsCountToN<T>(threadNumber, maxGlobal);

    auto timeStop = std::chrono::high_resolution_clock::now();

    assert(operationCount == maxGlobal);
    long workTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeStop - timeStart).count();
//    cout << threadNumber << " threads " << operationCount << " in " << workTime << "ms." << std::endl;
    return workTime;
}

template<typename T>
long measureAverageTime(size_t threadNumber, size_t count, int maxValue) {
    assert(count > 0);
    double sum = 0;
    for (size_t i = 0; i < count; ++i) {
        sum += measureSingleRun<T>(threadNumber, maxValue);
    }
    long average = long (sum / count);
    cout << threadNumber << " threads in " << average << " ms." << std::endl;
    return long (sum / count);
}

int main() {
    measureAverageTime<Futex<false>>(2, 10, 30000000);
    measureAverageTime<std::mutex>(2, 10, 30000000);
    measureAverageTime<Futex<true>>(2, 10, 30000000);

    measureAverageTime<Futex<false>>(4, 10, 30000000);
    measureAverageTime<std::mutex>(4, 10, 30000000);
    measureAverageTime<Futex<true>>(4, 10, 30000000);

    measureAverageTime<Futex<false>>(8, 5, 30000000);
    measureAverageTime<std::mutex>(8, 5, 30000000);
    measureAverageTime<Futex<true>>(8, 5, 30000000);

//    measureSingleRun<Futex>(thread::hardware_concurrency() / 2, 400000000);
//    measureSingleRun<Futex>(thread::hardware_concurrency(), 200000000);
//    measureSingleRun<Futex>(thread::hardware_concurrency() * 2, 100000000);
//
//    measureSingleRun<std::mutex>(thread::hardware_concurrency() / 2, 400000000);
//    measureSingleRun<std::mutex>(thread::hardware_concurrency(), 200000000);
//    measureSingleRun<std::mutex>(thread::hardware_concurrency() * 2, 100000000);

    return 0;
}
