//
// Created by V4kst1z.
//

#ifndef SAFESTL_QUEUE_THREADSAFE_H
#define SAFESTL_QUEUE_THREADSAFE_H

#include <queue>
#include <mutex>
#include <condition_variable>

namespace std {
    namespace threadsafe {
        template<typename T>
        class SafeQueue {
        public:
            SafeQueue() {}

            SafeQueue(const SafeQueue &queue) {
                std::lock_guard<std::mutex> lk(queue.mut);
                queueData = queue.queueData;
            }

            SafeQueue& operator=(const SafeQueue &queue) = delete;

            void push(T &value) {
                std::lock_guard<std::mutex> lk(mut);
                queueData.push(std::make_shared<T>(value));
                queueCond.notify_one();
            }

            void push(T &&value) {
                std::lock_guard<std::mutex> lk(mut);
                queueData.push(std::make_shared<T>(std::move(value)));
                queueCond.notify_one();
            }

            void waitPop(T &value) {
                std::unique_lock<std::mutex> lk(mut);
                queueCond.wait(lk, [this]() { return !queueData.empty(); });
                value = std::move(*queueData.front());
                queueData.pop();
            }

            std::shared_ptr<T> waitPop() {
                std::unique_lock<std::mutex> lk(mut);
                queueCond.wait(lk, [this]() { return !queueData.empty(); });
                std::shared_ptr<T> result = queueData.front();
                queueData.pop();
                return result;
            }

            bool tryPop(T &value) {
                std::lock_guard<std::mutex> lk(mut);
                if(queueData.empty()) return false;
                value = std::move(*queueData.front());
                queueData.pop();
                return true;
            }

            std::shared_ptr<T> tryPop() {
                std::lock_guard<std::mutex> lk(mut);
                if(queueData.empty()) return std::make_shared<T>();
                std::shared_ptr<T> result = queueData.front();
                queueData.pop();
                return result;
            }

            void empty() const {
                std::lock_guard<std::mutex> lk(mut);
                queueData.empty();
            }

        private:
            std::queue<std::shared_ptr<T>> queueData;
            mutable std::mutex mut;
            std::condition_variable queueCond;
        };
    }
}
#endif //SAFESTL_QUEUE_THREADSAFE_H
