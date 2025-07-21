#include <gflags/gflags.h>
#include <glog/logging.h>
#include <glog/stl_logging.h>
#include "hello.h"

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  // Requires --logtostderr=1 flag or next line
  gflags::SetCommandLineOption("logtostderr", "1");

  CHECK_NE(1, 2) << ": The world must be ending!";

  LOG(INFO) << hello::Greet(argc < 2 ? "world" : argv[1]) << std::endl;

  std::vector<int> x;
  x.push_back(1);
  x.push_back(2);
  x.push_back(3);
  LOG(INFO) << "ABC, it's easy as " << x;

  return EXIT_SUCCESS;
}