//
// Created by V4kst1z.
//

#include <iostream>
#include "../include/stack-threadsafe.h"

constexpr int NUM_WORKERS = 4;
constexpr int INCR = 3;

using SafeStackInt = std::threadsafe::SafeSatck<int>;

void task(std::shared_ptr<SafeStackInt> &stack) {
    for (unsigned idx = 0; idx < INCR; idx++) {
        int n;
        stack->pop(n);
        stack->push(n + 1);
    }
}


int main() {
    std::shared_ptr<SafeStackInt> stack = std::make_shared<SafeStackInt>();
    stack->push(0);
    std::thread ts[NUM_WORKERS];
    for (unsigned idx = 0; idx < NUM_WORKERS; idx++) {
        ts[idx] = std::thread(task, std::ref(stack));
        ts[idx].join();
    }

    std::shared_ptr<int> n = stack->pop();
    std::cout << *n  << std::endl;
    return 0;
}

