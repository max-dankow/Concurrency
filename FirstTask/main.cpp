#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

using std::vector;
using std::cout;
using std::endl;
//void processG(size_t id) {
//    std::cout << "Hello world " << id << std::endl;
//}
//
//int main() {
//    const size_t threadsCount = 10;
//    std::vector<std::thread> threads;
//    for (size_t i = 0; i < threadsCount; ++i) {
//        threads.push_back(std::thread(processG, i));
//    }
//    for (size_t i = 0; i < threads.size(); ++i) {
//        threads[i].join();
//    }
//    return 0;
//}

class TBufferSize {
public:
    TBufferSize(long long int bytesAmount) : bytesAmount(bytesAmount) { }

    const long long toKB() {
        return bytesAmount / 1024;
    }

    const long long toMB() {
        return this->toKB() / 1024;
    }

    const long long toGB() {
        return this->toMB() / 1024;
    }

private:
    long long bytesAmount;
};

vector<int> generateData(size_t amount) {
    int currentX;
    std::vector<int> data;
    data.reserve(amount);
    std::back_insert_iterator<std::vector<int>> dataInsertIterator(data);
    std::generate_n(dataInsertIterator, amount, [&]() -> int {
        currentX = currentX * 1664525 + 1013904223;
        return currentX;
    });
    return data;
}

void slave(/*const vector<int>::const_iterator rangeBegin, const vector<int>::const_iterator rangeEnd, int *result*/) {
    /*if (rangeBegin == rangeEnd) {
        *result = std::numeric_limits<int>::min();
        return;
    }
    int max = *rangeBegin;
    for (auto it = rangeBegin + 1; it != rangeEnd; ++it) {
        if (*it > max) {
            max = *it;
        }
    }
    cout << "Done. Max = " << max << std::endl;
    *result = max;*/
}

int main() {
    vector<int> data = generateData(1024 * 1024 * 500);
    TBufferSize bufferSize(data.size() * sizeof(data.front()));
    cout << "Data buffer (" << bufferSize.toGB() << " MB) has been successfuly generated." << std::endl;
//    for (int a : data) {
//        cout << a << std::endl;
//    }

    const size_t threadsNumber = std::thread::hardware_concurrency();
    size_t range = data.size() / threadsNumber;
    vector<std::thread> threads;
    vector<int> results(threadsNumber);
    cout << "Subtasks are:\n";
    for (size_t i = 0; i < threadsNumber; ++i) {
        vector<int>::const_iterator beginRange = data.begin() + i * range;
        vector<int>::const_iterator endRange = beginRange + range;
        if (i + 1 == threadsNumber) {
            endRange = data.end();
        }
        threads.push_back(std::thread(slave/*, beginRange, endRange, &results[i])*/));
        cout << "From " << beginRange - data.begin() << " to " << endRange - data.begin() << std::endl;
    }

    for (size_t i = 0; i < threads.size(); ++i) {
        threads[i].join();
    }

    int globalMax = results.front();
    for (int a : results) {
        if (globalMax < a) {
            globalMax = a;
        }
        cout << a << std::endl;
    }
    cout << "ALL DONE. GLOBAL MAXIMUM = " << globalMax << " computed by " << threadsNumber << " threads." << std::endl;
    return 0;
}