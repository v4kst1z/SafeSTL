//
// Created by V4kst1z.
//

#ifndef SAFESTL_STACK_THREADSAFE_H
#define SAFESTL_STACK_THREADSAFE_H

#include <mutex>
#include <stack>
#include <exception>
#include <memory>

namespace std {
    namespace threadsafe {
        struct EmptyStack : public exception {
            const char *what() const throw() {
                return "Stack is empty~";
            }
        };

        template <typename T>
        class SafeSatck {
        public:
            SafeSatck() {}

            SafeSatck(const SafeSatck &stack) {
                std::lock_guard<std::mutex> lk(stack.mut);
                stackData = stack.stackData;
            }

            SafeSatck& operator=(const SafeSatck &stack) = delete;

            void push(T &value) {
                std::lock_guard<std::mutex> lk(mut);
                stackData.push(value);
            }

            void push(T &&value) {
                std::lock_guard<std::mutex> lk(mut);
                stackData.push(std::move(value));
            }

            void pop(T &value) {
                std::lock_guard<std::mutex> lk(mut);
                if (stackData.empty()) throw EmptyStack();
                value = std::move(stackData.top());
                stackData.pop();
            }

            std::shared_ptr<T> pop() {
                std::lock_guard<std::mutex> lk(mut);
                if (stackData.empty()) throw EmptyStack();
                std::shared_ptr<T> result(std::make_shared<T>(std::move(stackData.top())));
                stackData.pop();
                return result;
            }

            bool empty() const {
                std::lock_guard<std::mutex> lk(mut);
                return stackData.empty();
            }

        private:
            std::stack<T> stackData;
            mutable std::mutex mut;
        };
    }
}

#endif //SAFESTL_STACK_THREADSAFE_H
