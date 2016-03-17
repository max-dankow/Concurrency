#include <iostream>
#include <queue>
#include <thread>
#include "SyncQueue.h"

void worker(SyncQueue<std::queue<int>> &tasks, size_t id) {
    Optional<int> task = tasks.popNoWait();
    int result;
    if (!task.some(result)) {
        std::cout << "No task by " << id << '\n';
    } else {
        std::cout << "Task " << result << " by " << id << '\n';
    }
}

int main() {
    SyncQueue<std::queue<int>> sQueue;
    sQueue.push(100);
    sQueue.push(101);
    sQueue.push(102);
    std::vector<std::thread> threads;
    for (size_t i = 0; i < 4; ++i) {
        threads.push_back(std::thread(worker, std::ref(sQueue), i));
    }

    for (size_t i = 0; i < threads.size(); ++i) {
        threads[i].join();
    }

    return 0;
}