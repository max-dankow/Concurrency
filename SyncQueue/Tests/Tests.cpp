#include <gtest/gtest.h>
#include <algorithm>
#include <queue>
#include <stack>
#include <list>
#include <thread>
#include "../SyncQueue.h"

template<class C>
void sender(std::vector<int> &source, SyncQueue<C> &taskQueue, size_t offset, size_t size) {
    auto upperBound = std::min(offset + size, source.size());
    for (size_t i = offset; i < upperBound; ++i) {
        taskQueue.push(source[i]);
    }
}

template<class C>
void receiver(std::vector<int> &dest, SyncQueue<C> &taskQueue, size_t offset, size_t size) {
    auto upperBound = std::min(offset + size, dest.size());
    for (size_t i = offset; i < upperBound; ++i) {
        Optional<int> optionalValue = taskQueue.popOrWait();
        optionalValue.some(dest[i]);
    }
}

auto engine = std::default_random_engine{};

template<class C>
bool master(size_t senderNumber, size_t receiverNumber) {
    using std::vector;

    const size_t DATA_SIZE = 1000000;
    vector<int> source(DATA_SIZE);
    std::uniform_int_distribution<int> valuesRandom(-1000, 1000);
    for (size_t i = 0; i < DATA_SIZE; ++i) {
        source[i] = valuesRandom(engine);
    }

    SyncQueue<C> syncQueue;

    // Создаем отправителей.
    vector<std::thread> senders;
    size_t readBlockSize = (DATA_SIZE + senderNumber - 1) / senderNumber;
    for (size_t i = 0; i < senderNumber; ++i) {
        senders.push_back(
                std::thread(sender<C>, std::ref(source), std::ref(syncQueue), i * readBlockSize, readBlockSize));
    }

    // Создаем принимающие потоки.
    vector<int> dist(DATA_SIZE, 0);
    vector<std::thread> receivers;
    size_t writeBlockSize = (DATA_SIZE + receiverNumber - 1) / receiverNumber;
    for (size_t i = 0; i < receiverNumber; ++i) {
        receivers.push_back(
                std::thread(receiver<C>, std::ref(dist), std::ref(syncQueue), i * writeBlockSize, writeBlockSize));
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
    return source == dist && syncQueue.empty();
}

TEST(SyncQueue, General) {
    ASSERT_TRUE(master<std::queue<int>>(2, 7));
    ASSERT_TRUE(master<std::deque<int>>(2, 7));
    ASSERT_TRUE(master<std::vector<int>>(2, 7));
    ASSERT_TRUE(master<std::list<int>>(2, 7));
    ASSERT_TRUE(master<std::stack<int>>(2, 7));
}

TEST(SyncQueue, ClosedExceptons) {
    SyncQueue<std::queue<int>> syncQueue;
    syncQueue.close();
    ASSERT_THROW(syncQueue.close(), std::logic_error);
    ASSERT_THROW(syncQueue.push(0), std::logic_error);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}