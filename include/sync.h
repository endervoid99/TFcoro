#pragma once

#include <coroutine>
#include <memory>
#include <mutex>
namespace tfcoro
{
 
        // 1. Shared Ownership & Lifetime Management
        //      Multiple awaitable_event objects can reference the same underlying event state, which is essential for:

        //      Passing events between functions/threads
        //      Storing events in containers
        //      Having multiple references to the same synchronization primitive
        // 2. Thread Safety
        //     The shared_ptr ensures the state object stays alive even if
        //     The original awaitable_event is destroyed while coroutines are still waiting
        //     Multiple threads are accessing the event simultaneously
        //     Coroutines are suspended and the event object goes out of scope struct awaitable_event

        // stack allocated linked list of waiting coroutines
        //         Coroutine 1 Stack:     Coroutine 2 Stack:     Coroutine 3 Stack:
        // ┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
        // │ awaiter         │    │ awaiter         │    │ awaiter         │
        // │ ┌─────────────┐ │    │ ┌─────────────┐ │    │ ┌─────────────┐ │
        // │ │ node n      │ │    │ │ node n      │ │    │ │ node n      │ │
        // │ │ ├─next──────┼─┼────┼→│ ├─next──────┼─┼────┼→│ ├─next=null │ │
        // │ │ └─handle    │ │    │ │ └─handle    │ │    │ │ └─handle    │ │
        // │ └─────────────┘ │    │ └─────────────┘ │    │ └─────────────┘ │
        // └─────────────────┘    └─────────────────┘    └─────────────────┘
        //         ▲                      ▲                      ▲
        //         │                      │                      │
        //         └──────────────────────┴──────────────────────┘
        //                                │
        //  state.head points here

        // std::memory_order_relaxed

        // When you use std::memory_order_relaxed, you’re telling the compiler and CPU:

        // “Perform this atomic operation atomically (no data race), but don’t impose any synchronization or ordering constraints.”

        // That means:

        // The operation itself is atomic — no data race.

        // But it does not establish any happens-before relationship with any other operation.

        // Other threads may see the results in any order.

        template <typename T>
        struct relaxed_atomic : std::atomic<T>
        {
            using atomic = std::atomic<T>;
            using atomic::atomic;
            using atomic::load;
            using atomic::store;

            T load() const volatile noexcept
            {
                return atomic::load(std::memory_order_relaxed);
            }
            void store(T value) volatile noexcept
            {
                atomic::store(value, std::memory_order_relaxed);
            }

            operator T() const noexcept { return load(); }
            relaxed_atomic &operator=(T value) noexcept
            {
                store(value);
                return *this;
            }
        };

        struct awaitable_event
        {
            void set() const
            {
                shared->set();
            }

            auto operator co_await() const noexcept
            {
                return awaiter{*shared};
            }

        private:
            struct node
            {
                node *next;
                std::coroutine_handle<> handle;
            };

            struct state
            {
                // std::atomic_bool signaled =false;
                // winrt::slim_mutex mutex;
                std::mutex mutex;
                node *head = nullptr;
                relaxed_atomic<node **> last = &head; // new

                void set()
                {
                    node *rest = nullptr;
                    {
                        auto guard = std::lock_guard(mutex);
                        // auto guard = winrt::slim_lock_guard(mutex);
                        last.store(nullptr); //, std::memory_order_relaxed); new
                        rest = std::exchange(head, nullptr);
                    }
                    // while (lifo) {
                    //     auto n = lifo;
                    //     lifo = std::exchange(n->next, fifo);
                    //     fifo = n;
                    // }
                    while (rest)
                    {
                        auto handle = rest->handle;
                        rest = rest->next;
                        handle();
                    }
                }

                bool await_ready() const noexcept
                {
                    return !last.load(); // new
                }

                bool await_suspend(node &n) noexcept
                {
                    // auto guard = winrt::slim_lock_guard(mutex);
                    auto guard = std::lock_guard(mutex);
                    auto p = last.load();
                    if (!p)
                        return false;
                    *p = &n;
                    last = &n.next;
                    n.next = nullptr;
                    return true;
                }

                void await_resume() const noexcept {}
            };

            struct awaiter
            {
                state &s;
                node n;

                bool await_ready() const noexcept { return s.await_ready(); }
                bool await_suspend(
                    std::coroutine_handle<> handle) noexcept
                {
                    n.handle = handle;
                    return s.await_suspend(n);
                }

                void await_resume() const noexcept { return s.await_resume(); }
            };

            std::shared_ptr<state> shared = std::make_shared<state>();
        };

        // However, it did perform memory allocations, and that could result in low-memory exceptions.
        // Furthermore, it used a std::vector, and pushing a value onto the vector could take a long time
        // if the vector needs to be reallocated.
        // struct awaitable_event_vector
        // {
        //     void set() const { shared->set(); }

        //     auto await_ready() const noexcept
        //     {
        //         return shared->await_ready();
        //     }

        //     auto await_suspend(
        //         std::experimental::coroutine_handle<> handle) const
        //     {
        //         return shared->await_suspend(handle);
        //     }

        //     auto await_resume() const noexcept
        //     {
        //         return shared->await_resume();
        //     }

        // private:
        //     struct state
        //     {
        //         // Atomic serves as a lightweight flag for reading (set is protected by mutex)
        //         std::atomic<bool> signaled = false;
        //         winrt::slim_mutex mutex;
        //         std::vector<std::experimental::coroutine_handle<>> waiting;

        //         void set()
        //         {
        //             std::vector<std::experimental::coroutine_handle<>> ready;
        //             {
        //                 auto guard = winrt::slim_lock_guard(mutex);
        //                 // relaxed ordering is sufficient as mutex provides synchronization
        //                 // and there ar no dependent operations on 'signaled' outside mutex
        //                 signaled.store(true, std::memory_order_relaxed);
        //                 // swap in o(1) instead of copy
        //                 std::swap(waiting, ready);
        //             }
        //             for (auto &&handle : ready)
        //                 handle();
        //         }

        //         bool await_ready() const noexcept
        //         {
        //             return signaled.load(std::memory_order_relaxed);
        //         }

        //         bool await_suspend(
        //             std::experimental::coroutine_handle<> handle)
        //         {
        //             auto guard = winrt::slim_lock_guard(mutex);
        //             if (signaled.load(std::memory_order_relaxed))
        //                 return false;
        //             waiting.push_back(handle);
        //             return true;
        //         }

        //         void await_resume() const noexcept {}
        //     };

        //     std::shared_ptr<state> shared = std::make_shared<state>();
        // }    
}