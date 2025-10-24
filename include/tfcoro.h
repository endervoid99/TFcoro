#pragma once

#include <coroutine>
#include <exception>
#include <thread>
#include <utility>

#ifndef _WIN32
#include <cppcoro/task.hpp>
#include <cppcoro/sync_wait.hpp>
#endif

namespace tfcoro {
#ifndef _WIN32
    using namespace ::cppcoro;
#else
    // Basic task implementation for Linux
    template<typename T = void>
    struct task {
        struct promise_type {
            std::exception_ptr exception;
            
            task get_return_object() {
                return task{std::coroutine_handle<promise_type>::from_promise(*this)};
            }
            
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }
            
            void unhandled_exception() {
                exception = std::current_exception();
            }
            
            void return_void() {}
        };
        
        using handle_type = std::coroutine_handle<promise_type>;
        handle_type coro;
        
        task(handle_type h) : coro(h) {}
        task(task&& other) noexcept : coro(std::exchange(other.coro, {})) {}
        
        ~task() {
            if (coro) {
                coro.destroy();
            }
        }
        
        task(const task&) = delete;
        task& operator=(const task&) = delete;
        task& operator=(task&& other) noexcept {
            if (this != &other) {
                if (coro) {
                    coro.destroy();
                }
                coro = std::exchange(other.coro, {});
            }
            return *this;
        }
        
        bool await_ready() const noexcept {
            return coro.done();
        }
        
        void await_suspend(std::coroutine_handle<> h) const noexcept {
            // Simple implementation - could be improved with proper scheduling
        }
        
        void await_resume() const {
            if (coro.promise().exception) {
                std::rethrow_exception(coro.promise().exception);
            }
        }
    };
    
    // Basic sync_wait implementation for Linux
    template<typename Awaitable>
    auto sync_wait(Awaitable&& awaitable) {
        if constexpr (std::is_same_v<Awaitable, task<>>) {
            // For task<>, just wait for completion
            while (!awaitable.coro.done()) {
                std::this_thread::yield();
            }
            awaitable.await_resume();
        } else {
            // For other awaitables, we need a more sophisticated approach
            // This is a simplified version
            struct sync_task {
                Awaitable awaitable;
                bool completed = false;
                std::exception_ptr exception;
                
                auto run() -> task<> {
                    try {
                        co_await std::forward<Awaitable>(awaitable);
                        completed = true;
                    } catch (...) {
                        exception = std::current_exception();
                        completed = true;
                    }
                }
            };
            
            sync_task st{std::forward<Awaitable>(awaitable)};
            auto t = st.run();
            
            while (!st.completed) {
                std::this_thread::yield();
            }
            
            if (st.exception) {
                std::rethrow_exception(st.exception);
            }
        }
    }
#endif
}