#include <gtest/gtest.h>
#include <queue>
#include <stack>
#include <list>
#include <thread>
#include "../SyncQueue.h"
#include "../Detector.h"
#include <algorithm>

TEST(Detector, HaveMethods) {
    // hasPush
    ASSERT_TRUE(Detector<std::queue<Optional<int> > >::hasPush);
    ASSERT_TRUE(Detector<std::stack<Optional<int> > >::hasPush);
    ASSERT_FALSE(Detector<std::vector<Optional<int> > >::hasPush);
    ASSERT_FALSE(Detector<std::list<Optional<int> > >::hasPush);
    ASSERT_FALSE(Detector<std::deque<Optional<int> > >::hasPush);

    // hasPop
    ASSERT_TRUE(Detector<std::queue<Optional<int> > >::hasPop);
    ASSERT_TRUE(Detector<std::stack<Optional<int> > >::hasPop);
    ASSERT_FALSE(Detector<std::vector<Optional<int> > >::hasPop);
    ASSERT_FALSE(Detector<std::list<Optional<int> > >::hasPop);
    ASSERT_FALSE(Detector<std::deque<Optional<int> > >::hasPop);

    // hasPushBack
    ASSERT_FALSE(Detector<std::queue<Optional<int> > >::hasPushBack);
    ASSERT_FALSE(Detector<std::stack<Optional<int> > >::hasPushBack);
    ASSERT_TRUE(Detector<std::vector<Optional<int> > >::hasPushBack);
    ASSERT_TRUE(Detector<std::deque<Optional<int> > >::hasPushBack);
    ASSERT_TRUE(Detector<std::deque<Optional<int> > >::hasPushBack);

    // hasPopBack
    ASSERT_FALSE(Detector<std::queue<Optional<int> > >::hasPopBack);
    ASSERT_FALSE(Detector<std::stack<Optional<int> > >::hasPopBack);
    ASSERT_TRUE(Detector<std::vector<Optional<int> > >::hasPopBack);
    ASSERT_TRUE(Detector<std::deque<Optional<int> > >::hasPopBack);
    ASSERT_TRUE(Detector<std::deque<Optional<int> > >::hasPopBack);
}

TEST(Detector, Suitability) {
    // push() and pop()
    ASSERT_TRUE(Detector<std::queue<Optional<int> > >::pushPopSuitable);
    ASSERT_TRUE(Detector<std::stack<Optional<int> > >::pushPopSuitable);
    ASSERT_FALSE(Detector<std::vector<Optional<int> > >::pushPopSuitable);
    ASSERT_FALSE(Detector<std::list<Optional<int> > >::pushPopSuitable);
    ASSERT_FALSE(Detector<std::deque<Optional<int> > >::pushPopSuitable);

    // push_back() and pop_back()
    ASSERT_FALSE(Detector<std::queue<Optional<int> > >::pushBackPopBackSuitable);
    ASSERT_FALSE(Detector<std::stack<Optional<int> > >::pushBackPopBackSuitable);
    ASSERT_TRUE(Detector<std::vector<Optional<int> > >::pushBackPopBackSuitable);
    ASSERT_TRUE(Detector<std::list<Optional<int> > >::pushBackPopBackSuitable);
    ASSERT_TRUE(Detector<std::deque<Optional<int> > >::pushBackPopBackSuitable);
}

TEST(Detector, Unsuitability) {
    ASSERT_FALSE(Detector<std::set<Optional<int> > >::pushPopSuitable);
    ASSERT_FALSE(Detector<std::set<Optional<int> > >::pushBackPopBackSuitable);
}

template<class C>
void worker(std::vector<int> &values, SyncQueue<C> &taskQueue, std::mutex &mutex) {
    while (true) {
        Optional<size_t> optionalTask = taskQueue.popOrWait();
        size_t index;
        if (optionalTask.some(index)) {
            mutex.lock();
            values[index]++;
            mutex.unlock();
        } else {
            break;
        }
    }
}

auto engine = std::default_random_engine{};

template<class C>
bool master(size_t threadNumber) {
    const size_t DATA_SIZE = 10000;
    const int VALUE = 50;
    std::vector<int> values(DATA_SIZE);
    SyncQueue<C> syncQueue;
    std::mutex mutex;
    std::vector<std::thread> threads;

    for (size_t i = 0; i < threadNumber; ++i) {
        threads.push_back(
                std::thread(worker<C>, std::ref(values), std::ref(syncQueue), std::ref(mutex)));
    }

    // Формируем задачи.
    std::vector<size_t> tasks;
    tasks.reserve(DATA_SIZE);
    for (size_t i = 0; i < DATA_SIZE; ++i) {
        tasks.push_back(i);
    }
    std::shuffle(tasks.begin(), tasks.end(), engine);

    for (size_t task : tasks) {
        for (int i = 0; i < VALUE; ++i) {
            syncQueue.push(task);
        }
    }
    syncQueue.close();

    for (size_t i = 0; i < threads.size(); ++i) {
        threads[i].join();
    }

    // Проверяем результат.
    for (int i : values) {
        if (i != VALUE) {
            return false;
        }
    }
    return true;
}

TEST(SyncQueue, General) {
    ASSERT_TRUE(master<std::queue<size_t >>(2));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}