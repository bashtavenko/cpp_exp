#include "absl/flags/parse.h"
#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/str_format.h"
#include "hello.h"

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  CHECK_NE(1, 2) << ": The world must be ending!";
  LOG(INFO) << hello::Greet(argc < 2 ? "world" : argv[1]);
  std::vector<int> x;
  x.push_back(1);
  x.push_back(2);
  x.push_back(3);
  LOG(INFO) << absl::StreamFormat("ABC, it has %i elements.", x.size());
  return EXIT_SUCCESS;
}