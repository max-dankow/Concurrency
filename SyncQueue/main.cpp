#include <iostream>
#include <queue>
#include <thread>
#include "SyncQueue.h"

void worker(SyncQueue<std::queue<int>> &tasks, size_t id) {
    int task = tasks.popOrWait();
    std::cout << "Task " << task << " by " << id << std::endl;
}

int main() {
    SyncQueue<std::queue<int>> sQueue;
    sQueue.push(100);
    std::vector<std::thread> threads;
    for (size_t i = 0; i < 40; ++i) {
        threads.push_back(std::thread(worker, std::ref(sQueue), i));
    }

    for (size_t i = 0; i < threads.size(); ++i) {
        threads[i].join();
    }

    return 0;
}