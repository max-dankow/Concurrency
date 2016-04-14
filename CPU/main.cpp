#include <iostream>
#include "CachePingPong.h"

int main() {
    for (int i = 12; i < 24; i += 1) {
        std::cout << i * 4 << ' ' << CachePingPong::getAverageExecutionTime(i) << '\n';

    }
    return 0;
}