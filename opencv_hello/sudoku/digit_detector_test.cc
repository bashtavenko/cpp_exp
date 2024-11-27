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
  ASSERT_THAT(detector.Detect(get_image("digit-3.png")), Optional(3));
  ASSERT_THAT(detector.Detect(get_image("digit-5.png")), Optional(5));
  ASSERT_THAT(detector.Detect(get_image("digit-7.png")), Optional(7));
  ASSERT_THAT(detector.Detect(get_image("digit-blank.png")), Optional(0));
}

}  // namespace
}  // namespace sudoku