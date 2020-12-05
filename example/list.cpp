//
// Created by V4kst1z.
//


#include <iostream>
#include <thread>
#include "../include/list-threadsafe.h"

constexpr int NUM_WORKERS = 4;

using SafeListInt = std::threadsafe::SafeList<int>;

void task(std::shared_ptr<SafeListInt> &list) {
    list->forEach([&](int &node) { node = node * 2;});
}

int main() {
    std::thread threads[NUM_WORKERS];

    std::shared_ptr<SafeListInt> ls = std::make_shared<SafeListInt>();

    for (int idx = 0; idx < 10; idx++) {
        ls->pushFront(idx);
    }

    for (unsigned idx = 0; idx < NUM_WORKERS; idx++) {
        threads[idx] = std::thread(task, std::ref(ls));
        threads[idx].join();
    }

    ls->forEach([&](int &node) { std::cout << node << "\t"; });

    return 0;
}
