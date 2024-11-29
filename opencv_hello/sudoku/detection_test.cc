#include "sudoku/detection.h"
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <filesystem>
#include "absl/strings/string_view.h"
#include "opencv2/opencv.hpp"
#include "tools/cpp/runfiles/runfiles.h"

namespace sudoku {
namespace {
using ::bazel::tools::cpp::runfiles::Runfiles;
using ::testing::NotNull;
using ::testing::SizeIs;

constexpr absl::string_view kTestDataPath = "opencv_hello/sudoku/testdata";

TEST(DetectCells, Works) {
  auto files = Runfiles::CreateForTest();
  ASSERT_THAT(files, NotNull());
  cv::Mat image = cv::imread(files->Rlocation(
      std::filesystem::path(kTestDataPath) / "sudoku_9_9.png"));
  ASSERT_FALSE(image.empty());
  std::vector<std::vector<SudokuDetection>> result = DetectCells(image);
  EXPECT_THAT(result[0][1].digit_position, cv::Rect(40, 0, 41, 41));
}

}  // namespace
} // namespace sudoku