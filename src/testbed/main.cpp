#include <cstdio>
#include <thread>
#include <chrono>

#include "tfcoro.h"
#include "sync.h"




tfcoro::awaitable_event event;

tfcoro::task<> coro1() {
    printf("coro1\n");
    co_return;
}

tfcoro::task<> exampleCoroutine() {
    //co_await coro1();        
    co_await event;
    printf("event received\n");
    co_return;
}

int main() {
    
    // Start a thread that will set the event after 2 seconds
    std::thread eventSetter([&]() {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        printf("Setting event from background thread\n");
        event.set();
    });
       
    std::thread secondWaiter([&]() {     
        printf("Waiting for event on other thread\n");
        tfcoro::sync_wait(exampleCoroutine());        
    });
 
    printf("Waiting for event on main thread\n");
    tfcoro::sync_wait(exampleCoroutine());
    //printf("Hello, World!\n");
    
    // Wait for the background thread to complete
    eventSetter.join();
    secondWaiter.join();
}