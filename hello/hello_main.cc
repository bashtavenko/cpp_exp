#include "hello.h"
#include <glog/logging.h>

int main(int argc, const char** argv) {
    google::InitGoogleLogging(argv[0]);
    CHECK_NE(2, 1) << ": The world must be ending!";
    LOG(INFO) << hello::Greet(argc < 2 ? "world" : argv[1]) << std::endl;
    return 0;
}