#include <gtest/gtest.h>
#include <random>
#include <thread>
#include <algorithm>
#include "../../Optional.h"
#include "../LockFreeStack.h"

void sender(const std::vector<int> &source, LockFreeStack<int> &taskQueue, size_t offset, size_t size) {
    auto upperBound = std::min(offset + size, source.size());
    for (size_t i = offset; i < upperBound; ++i) {
        taskQueue.push(source[i]);
    }
}

void receiver(std::vector<int> &dest, LockFreeStack<int> &taskQueue, size_t offset, size_t size) {
    auto upperBound = std::min(offset + size, dest.size());
    for (size_t i = offset; i < upperBound; ++i) {
        Optional<int> optionalValue;
        do {
            optionalValue = taskQueue.pop();
        } while (optionalValue.none());
        optionalValue.some(dest[i]);
    }
}

auto engine = std::default_random_engine{};

bool master(size_t senderNumber, size_t receiverNumber) {
    using std::vector;

    const size_t DATA_SIZE = 10000;
    vector<int> source(DATA_SIZE);
    std::uniform_int_distribution<int> valuesRandom(-1000, 1000);
    for (size_t i = 0; i < DATA_SIZE; ++i) {
        source[i] = valuesRandom(engine);
    }

    LockFreeStack<int> lockFreeStack;

    // Создаем отправителей.
    vector<std::thread> senders;
    size_t readBlockSize = (DATA_SIZE + senderNumber - 1) / senderNumber;
    for (size_t i = 0; i < senderNumber; ++i) {
        senders.push_back(
                std::thread(sender, std::ref(source), std::ref(lockFreeStack), i * readBlockSize, readBlockSize));
    }

    // Создаем принимающие потоки.
    vector<int> dist(DATA_SIZE, 0);
    vector<std::thread> receivers;
    size_t writeBlockSize = (DATA_SIZE + receiverNumber - 1) / receiverNumber;
    for (size_t i = 0; i < receiverNumber; ++i) {
        receivers.push_back(
                std::thread(receiver, std::ref(dist), std::ref(lockFreeStack), i * writeBlockSize, writeBlockSize));
    }

    for (size_t i = 0; i < senders.size(); ++i) {
        senders[i].join();
    }

    for (size_t i = 0; i < receivers.size(); ++i) {
        receivers[i].join();
    }
    // Проверяем результат.
    std::sort(source.begin(), source.end());
    std::sort(dist.begin(), dist.end());
    return source == dist && lockFreeStack.empty();
}

TEST(LockFreeStack, General) {
    ASSERT_TRUE(master(2, 4));
    ASSERT_TRUE(master(4, 2));
    ASSERT_TRUE(master(2, 2));
    ASSERT_TRUE(master(8, 2));
    ASSERT_TRUE(master(2, 8));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}