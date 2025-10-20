#include <cstdio>

#include "tfcoro.h"

tfcoro::task<> exampleCoroutine() {
    co_return;
}

int main() {
    printf("Hello, World!\n");
}