#include "sudoku/detection.h"
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <filesystem>
#include "absl/strings/string_view.h"
#include "opencv2/opencv.hpp"
#include "sudoku/digit_detector.h"
#include "tools/cpp/runfiles/runfiles.h"

namespace sudoku {
namespace {
using ::bazel::tools::cpp::runfiles::Runfiles;
using ::testing::Eq;
using ::testing::NotNull;
using ::testing::SizeIs;

constexpr absl::string_view kTestDataPath = "opencv_hello/sudoku/testdata";
constexpr absl::string_view kModelPath = "opencv_hello/sudoku/model.yml";

TEST(DetectCells, Works) {
  auto files = Runfiles::CreateForTest();
  ASSERT_THAT(files, NotNull());
  cv::Mat image = cv::imread(files->Rlocation(
      std::filesystem::path(kTestDataPath) / "sudoku_9_9.png"));
  ASSERT_FALSE(image.empty());
  std::vector<std::vector<SudokuDetection>> result = DetectCells(image);
  EXPECT_THAT(result[0][1].digit_position, cv::Rect(40, 0, 41, 41));
}

TEST(E2ETest, Works) {
  auto files = Runfiles::CreateForTest();
  ASSERT_THAT(files, NotNull());
  cv::Mat image = cv::imread(files->Rlocation(
      std::filesystem::path(kTestDataPath) / "sudoku_9_9.png"));
  ASSERT_FALSE(image.empty());
  std::vector<std::vector<SudokuDetection>> cells = DetectCells(image);
  std::vector<std::vector<int32_t>> result(9, std::vector<int32_t>(9, 0));
  DigitDetector detector;
  detector.Init(files->Rlocation(kModelPath.data()));
  for (size_t row = 0; row < 9; ++row) {
    for (size_t col = 0; col < 9; ++col) {
      auto digit = detector.Detect(cells[row][col].digit_image);
      ASSERT_TRUE(digit.has_value());
      result[row][col] = digit.value();
    }
  }
  EXPECT_THAT(result, Eq(std::vector<std::vector<int32_t>>{
                          {5, 3, 0, 0, 7, 0, 0, 0, 0},
                          {6, 0, 0, 1, 9, 5, 0, 0, 0},
                          {0, 9, 8, 0, 0, 0, 0, 6, 0},
                          {8, 0, 0, 0, 6, 0, 0, 0, 3},
                          {4, 0, 0, 8, 0, 3, 0, 0, 1},
                          {7, 0, 0, 0, 2, 0, 0, 0, 6},
                          {0, 6, 0, 0, 0, 0, 2, 8, 0},
                          {0, 0, 0, 4, 1, 9, 0, 0, 5},
                          {0, 0, 0, 0, 8, 0, 0, 7, 9}}));
}

}  // namespace
}  // namespace sudoku