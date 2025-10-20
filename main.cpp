#include <cstdio>

#include "tfcoro.h"
#include "sync.h"

tfcoro::task<> exampleCoroutine() {
    co_return;
}

int main() {
    printf("Hello, World!\n");
}