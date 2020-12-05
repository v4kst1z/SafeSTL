//
// Created by V4kst1z.
//


#include <iostream>
#include <thread>
#include "../include/queue-threadsafe.h"

constexpr int NUM_WORKERS = 4;

using SafeQueueInt = std::threadsafe::SafeQueue<int>;

void task(std::shared_ptr<SafeQueueInt> &queue) {
    int result = *queue->tryPop();
    while (result != 0) {
        result = *queue->tryPop();
    }
    std::cout << "thread exit " << result << std::endl;
}

int main() {
    std::thread threads[NUM_WORKERS];
    std::shared_ptr<SafeQueueInt> queue = std::make_shared<SafeQueueInt>();

    for (unsigned idx = 1; idx < 40; idx++) {
        queue->push(idx * idx);
    }

    for (unsigned idx = 0; idx < NUM_WORKERS; idx++) {
        queue->push(0);
    }

    for (unsigned idx = 0; idx < NUM_WORKERS; idx++) {
        threads[idx] = std::thread(task, std::ref(queue));
        threads[idx].join();
    }

    return 0;
}
