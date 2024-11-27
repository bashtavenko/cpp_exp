#include <gflags/gflags.h>
#include <glog/logging.h>
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "sudoku/digit_detector.h"

ABSL_FLAG(std::string, mnist_dir, "/home/s15o/mnist_data",
          "Directory for MNIST dataset");
ABSL_FLAG(std::string, model_path, "/tmp/model.yml", "Model output path");

int main(int argc, char** argv) {
  google::InitGoogleLogging(*argv);

  gflags::ParseCommandLineFlags(&argc, &argv, /*remove_flags=*/true);
  // TODO: Cannot parse both Abseil and Glog flags
  sudoku::DigitDetector detector;
  if (!detector.Train(absl::GetFlag(FLAGS_mnist_dir),
                      absl::GetFlag(FLAGS_model_path))) {
    LOG(ERROR) << "Training failed";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}