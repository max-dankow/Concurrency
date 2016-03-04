#include <atomic>
#include <thread>
#include <vector>
#include <iostream>
#include <mutex>

using std::thread;
using std::atomic;
using std::vector;
using std::cout;

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
const int MAX_GLOBAL = 10000000;

void incrementer(Futex &futex, size_t &count, int &value, int maxValue) {
    while (true) {
        futex.lock();
        if (value < maxValue) {
            value++;
            ++count;
        } else {
            futex.unlock();
            break;
        }
        futex.unlock();
    }
}

size_t threadsCountToN(size_t threadNumber, int maxValue) {
    vector<std::thread> threads;
    vector<size_t> results(threadNumber);
    Futex futex;
    int global = 0;

    for (size_t i = 0; i < threadNumber; ++i) {
        threads.push_back(
                std::thread(incrementer, std::ref(futex), std::ref(results[i]), std::ref(global), maxValue)
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

int main() {
    threadsCountToN(THREAD_NUMBER, MAX_GLOBAL);
    return 0;
}
