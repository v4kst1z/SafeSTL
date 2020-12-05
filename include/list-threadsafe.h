//
// Created by V4kst1z.
//

#ifndef SAFESTL_LIST_THREADSAFE_H
#define SAFESTL_LIST_THREADSAFE_H

#include <mutex>

namespace std {
    namespace threadsafe {
        template <typename T>
        class SafeList{
        private:
            class node {
            private:
                std::mutex mut;
                std::unique_ptr<node> next;
                std::shared_ptr<T> data;
            public:
                friend class SafeList<T>;
                node(): next() {}
                node(const T& value) : data(std::make_shared<T>(value)) {}
            };
            node head;
        public:
            SafeList() {}

            SafeList(const SafeList& other) = delete;
            SafeList& operator=(const SafeList& other) = delete;

            void pushFront(const T& value) {
                std::lock_guard<std::mutex> lk(head.mut);
                std::unique_ptr<node> new_node = std::make_unique<node>(value);
                new_node->next = std::move(head.next);
                head.next = std::move(new_node);
            }

            template <typename Function>
            void forEach(Function func) {
                std::unique_lock<std::mutex> lk(head.mut);
                node* curr = &head;
                while (node* next = curr->next.get()) {
                    std::unique_lock<std::mutex> nextLk(next->mut);
                    lk.unlock();
                    func(*next->data);
                    curr = next;
                    lk = std::move(nextLk);
                }
            }

            template <typename Predicate>
            std::shared_ptr<T> findFirstIf(Predicate pre) {
                std::unique_lock<std::mutex> lk(head.mut);
                node *curr = &head;
                while (node* next = curr->next.get()) {
                    std::unique_lock<std::mutex> nextLk(next->mut);
                    lk.unlock();
                    if(pre(*next->data)) {
                        return next->data;
                    } else {
                        curr = next;
                        lk = std::move(nextLk);
                    }
                }
                return std::shared_ptr<T>();
            }

            template <typename Predicate>
            void removeIf(Predicate pre) {
                std::unique_lock<std::mutex> lk(head.mut);
                node *curr = &head;
                while (node *next = curr->next.get()) {
                    std::unique_lock<std::mutex> nextLk(next->mut);
                    if (pre(*next->data)) {
                        std::unique_ptr<node> old_next=std::move(curr->next); //
                        curr->next = std::move(next->next);
                        nextLk.unlock();
                    } else {
                        lk.unlock();
                        curr = next;
                        lk = std::move(nextLk);
                    }
                }
            }
        };
    }
}

#endif //SAFESTL_LIST_THREADSAFE_H
