#include "hello.h"
#include <iostream>

int main(int argc, const char** argv) {
    std::cout << hello::Greet(argc < 2 ? "world" : argv[1]) << std::endl;
    return 0;
}