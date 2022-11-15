#include "misc/misc.h"
#include "convolution/filters.h"
#include "transformations/transformations.h"
#include <gflags/gflags.h>
#include <glog/logging.h>
#include "absl/status/status.h"

int main(int argc, char** argv) {

  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

//  absl::Status status = hello::misc::ShowVideoCanny();
//  absl::Status status = hello::convolution::AdaptiveThreshold();
  absl::Status status = hello::transformations::PerspectiveTransform();
  LOG(INFO) << status.code();
  return status.ok() ? EXIT_SUCCESS : EXIT_FAILURE;
}

