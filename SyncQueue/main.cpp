#include <iostream>
#include <queue>
#include "SyncQueue.h"

int main() {
    SyncQueue<std::queue<int>> queue;
    int a = 10;
    queue.push(10);
    return 0; 
}