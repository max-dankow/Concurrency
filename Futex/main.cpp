#include <atomic>
#include <thread>
#include <vector>
#include <iostream>
#include <mutex>
#include <chrono>
#include <assert.h>

using std::thread;
using std::atomic;
using std::vector;
using std::cout;
using std::mutex;

class Futex {
public:

    Futex() {
        owner.store(NO_OWNER);
    }

    Futex(const Futex &) = delete;

    void lock() {
        int desired = NO_OWNER;
        // Ждем пока освободится.
        while (!owner.compare_exchange_strong(desired, (int) pthread_self())) {
            desired = NO_OWNER;
        }
    }

    void unlock() {
        int desired = (int) pthread_self();
        while (!owner.compare_exchange_strong(desired, NO_OWNER)) {
            desired = (int) pthread_self();
        }
    }

private:
    const int NO_OWNER = 0;
    atomic<int> owner;
};

const size_t THREAD_NUMBER = 4;
const int MAX_GLOBAL = 100000000;

void incrementer(mutex &mutex, size_t &count, int &value, int maxValue) {
    while (true) {
        mutex.lock();
        if (value < maxValue) {
            value++;
            ++count;
        } else {
            mutex.unlock();
            break;
        }
        mutex.unlock();
    }
}

size_t threadsCountToN(size_t threadNumber, int maxValue) {
    vector<std::thread> threads;
    vector<size_t> results(threadNumber);
    mutex mutex;
    int global = 0;

    for (size_t i = 0; i < threadNumber; ++i) {
        threads.push_back(
                std::thread(incrementer, std::ref(mutex), std::ref(results[i]), std::ref(global), maxValue)
        );
    }

    for (size_t i = 0; i < threads.size(); ++i) {
        threads[i].join();
    }
    size_t operationCount = 0;
    for (size_t c : results) {
        operationCount += c;
    }
    return operationCount;
}

std::chrono::milliseconds measureSingleRun(size_t threadNumber) {
    auto timeStart = std::chrono::steady_clock::now();

    size_t  operationCount = threadsCountToN(threadNumber, MAX_GLOBAL);

    auto timeStop = std::chrono::steady_clock::now();
    auto workTime = std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(timeStop - timeStart).count());
    cout << threadNumber << " threads " << operationCount << " in " << workTime.count() << "ms." << std::endl;
    return workTime;
}

std::chrono::milliseconds measureAverageTime(size_t threadNumber, size_t count) {
    assert(count > 0);
    std::chrono::milliseconds sum(0);
    for (size_t i = 0; i < count; ++i) {
        sum += measureSingleRun(threadNumber);
    }
    auto averageTime = sum / count;
    cout << "Average time for " << threadNumber << " threads is " << averageTime.count() << std::endl;
}

int main() {
    measureAverageTime(thread::hardware_concurrency() / 4, 10); // 7601 ms (4275 ms)
    measureAverageTime(thread::hardware_concurrency() / 2, 10); // 17724 ms (21920 ms)
    measureAverageTime(thread::hardware_concurrency(), 5); // 28691 ms (18039)
    measureAverageTime(thread::hardware_concurrency() * 2, 3); // 53792 ms (20773 ms)
    return 0;
}
