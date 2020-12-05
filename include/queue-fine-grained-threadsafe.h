//
// Created by V4kst1z.
//

#ifndef SAFESTL_QUEUE_FINE_GRAINED_THREADSAFE_H
#define SAFESTL_QUEUE_FINE_GRAINED_THREADSAFE_H

#include <mutex>
#include <condition_variable>

namespace std {
    namespace threadsafe {
        template<typename T>
        class SafeQueueFineGrained {
        public:
            SafeQueueFineGrained(): head(new node), tail(head.get()) {}
            SafeQueueFineGrained(const SafeQueueFineGrained &other) = delete;
            SafeQueueFineGrained& operator=(const SafeQueueFineGrained &other) = delete;

            void push(T &new_value) {
                std::unique_ptr<T> data(std::make_unique<T>(new_value));
                std::unique_ptr<node> new_node(new node);
                std::lock_guard<std::mutex> tail_lock(tail_mutex);
                tail->data = std::move(data);
                tail->next = std::move(new_node);
                tail = tail->next.get();
                data_cond.notify_one();
            }

            void push(T &&new_value) {
                std::unique_ptr<T> data(std::make_unique<T>(std::move(new_value)));
                std::unique_ptr<node> new_node(new node);
                std::lock_guard<std::mutex> tail_lock(tail_mutex);
                tail->data = std::move(data);
                tail->next = std::move(new_node);
                tail = tail->next.get();
                data_cond.notify_one();
            }

            bool tryPop(T &value) {
                std::lock_guard<std::mutex> head_lock(head_mutex);
                node *tailP = getTail();
                if(head.get() == tailP) {
                    return false;
                } else {
                    value = std::move(*head->data);
                    pop_head();
                }
            }

            void waitPop(T &value) {
                std::unique_lock<std::mutex> head_lock(head_mutex);
                data_cond.wait(head_lock, [this]() { return head.get() != getTail(); });
                value = std::move(*head->data);
                pop_head();
            }

            std::unique_ptr<T> waitPop() {
                std::unique_lock<std::mutex> head_lock(head_mutex);
                data_cond.wait(head_lock, [this]() { return head.get() != getTail(); });
                std::unique_ptr<T> result(std::move(head->data));
                pop_head();
                return result;
            }

            std::unique_ptr<T> tryPop() {
                std::lock_guard<std::mutex> head_lock(head_mutex);
                node *tailP = getTail();
                if(head.get() == tailP) {
                    return std::unique_ptr<T>();
                } else {
                    std::unique_ptr<T> result(std::move(head->data));
                    pop_head();
                    return result;
                }
            }

            bool empty() const {
                std::lock(head_mutex, tail_mutex);
                std::lock_guard<std::mutex> headLk(head_mutex, std::adopt_lock);
                std::lock_guard<std::mutex> tailLk(tail_mutex, std::adopt_lock);
                return head.get() == tail;
            }

        private:
            struct node {
                std::unique_ptr<T> data;
                std::unique_ptr<node> next;
            };

            std::unique_ptr<node> head;
            mutable std::mutex head_mutex;
            node *tail;
            mutable std::mutex tail_mutex;
            std::condition_variable data_cond;

            node* getTail() {
                std::lock_guard<std::mutex> tail_lock(tail_mutex);
                return tail;
            }

            std::unique_ptr<node> pop_head() {
                std::unique_ptr<node> old_head = std::move(head);
                head = std::move(old_head->next);
                return old_head;
            }
        };
    }
}
#endif //SAFESTL_QUEUE_FINE_GRAINED_THREADSAFE_H
