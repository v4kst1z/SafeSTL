//
// Created by V4kst1z.
//


#include <iostream>
#include <thread>
#include "../include/map-threadsafe.h"

constexpr int NUM_WORKERS = 4;

using SafeUnorderedMapStr = std::threadsafe::SafeUnorderedMap<std::string, int>;

void task(std::shared_ptr<SafeUnorderedMapStr> &map, int idxStart) {
    for (unsigned idx = idxStart; idx < 10 + idxStart; idx++) {
        map->addOrUpdate("v4kst1z" , idx);
        std::cout << map->valueFor("v4kst1z") << std::endl;

    }
}


int main() {
    std::thread threads[NUM_WORKERS];

    std::shared_ptr<SafeUnorderedMapStr> map1 = std::make_shared<SafeUnorderedMapStr>();

    for (unsigned idx = 0; idx < NUM_WORKERS; idx++) {
        threads[idx] = std::thread(task, std::ref(map1), idx * 10);
        threads[idx].join();
    }
    std::cout << "result is " << map1->valueFor("v4kst1z") << std::endl;
    return 0;
}
