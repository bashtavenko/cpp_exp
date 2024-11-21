#include <gflags/gflags.h>
#include <glog/logging.h>
#include <filesystem>
#include "absl/strings/string_view.h"
#include "opencv2/highgui.hpp"
#include "sudoku/detection.h"


int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, /*remove_flags=*/true);

  constexpr absl::string_view kTestDataPath = "sudoku/testdata";
  const std::string kWindow = "Contours";
  const std::string kDigits = "Digits";
  cv::Mat img =
      cv::imread(std::filesystem::path(kTestDataPath) / "sudoku_9_9.png");
  CHECK(!img.empty());
  auto result = sudoku::DetectCells(img);
     cv:imshow(kWindow, img);
  for (int i = 0; i < 9; ++i) {
    cv::imshow(kDigits + "-" + std::to_string(i), result[i]);
  }

  cv::waitKey(0);
  cv::destroyAllWindows();

  return EXIT_SUCCESS;
}