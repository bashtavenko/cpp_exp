#include "sudoku/digit_detector.h"
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <filesystem>
#include "opencv2/opencv.hpp"
#include "tools/cpp/runfiles/runfiles.h"

namespace sudoku {
namespace {
using ::bazel::tools::cpp::runfiles::Runfiles;
using ::testing::NotNull;
using ::testing::Optional;

constexpr absl::string_view kModelPath = "opencv_hello/sudoku/model.yml";
constexpr absl::string_view kTestDataPath = "opencv_hello/sudoku/testdata";

TEST(DigitDetector, Works) {
  auto files = Runfiles::CreateForTest();

  auto get_image = [&](absl::string_view image_name) {
    cv::Mat image = cv::imread(
        files->Rlocation(std::filesystem::path(kTestDataPath) / image_name));
    return image;
  };
  ASSERT_THAT(files, NotNull());
  DigitDetector detector;
  detector.Init(files->Rlocation(kModelPath.data()));
  EXPECT_THAT(detector.Detect(get_image("digit-1.png")), Optional(1));
  EXPECT_THAT(detector.Detect(get_image("digit-2.png")), Optional(2));
  EXPECT_THAT(detector.Detect(get_image("digit-3.png")), Optional(3));
  EXPECT_THAT(detector.Detect(get_image("digit-4.png")), Optional(4));
  EXPECT_THAT(detector.Detect(get_image("digit-5.png")), Optional(5));
  EXPECT_THAT(detector.Detect(get_image("digit-6.png")), Optional(6));
  EXPECT_THAT(detector.Detect(get_image("digit-7.png")), Optional(7));
  EXPECT_THAT(detector.Detect(get_image("digit-8.png")), Optional(8));
  EXPECT_THAT(detector.Detect(get_image("digit-9.png")), Optional(9));
  EXPECT_THAT(detector.Detect(get_image("digit-blank.png")), Optional(0));
}

}  // namespace
}  // namespace sudoku