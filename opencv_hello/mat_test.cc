#include <iostream>
#include <filesystem>

#include "include/gtest/gtest.h"
#include "include/gmock/gmock-matchers.h"
#include "opencv2/opencv.hpp"
#include "tools/cpp/runfiles/runfiles.h"
#include "absl/strings/string_view.h"
#include <glog/logging.h>
#include <glog/stl_logging.h>

namespace hello {
namespace {
using ::bazel::tools::cpp::runfiles::Runfiles;
using ::std::filesystem::path;
using ::testing::Eq;
using ::testing::NotNull;
using ::cv::Mat;
using ::cv::Size;

constexpr absl::string_view kTestDataPath = "opencv_hello/testdata";

TEST(Mat, Works) {
  Runfiles* runfiles = Runfiles::CreateForTest();
  ASSERT_THAT(runfiles, NotNull());

  // Read from file
  Mat mat1 = cv::imread(runfiles->Rlocation(
      path(kTestDataPath) / "starry_night.jpg"));
  ASSERT_FALSE(mat1.empty());
  ASSERT_THAT(mat1.dims, Eq(2));
  ASSERT_THAT(mat1.rows, Eq(600));
  ASSERT_THAT(mat1.cols, Eq(752));

  // Create
  Mat mat(2, 2, CV_8UC1, cv::Scalar(0, 0, 255));
  ASSERT_THAT(mat.dims, Eq(2));

  // Read again
  Mat mat2 = cv::imread(runfiles->Rlocation(
      path(kTestDataPath) / "pic3.png"));
  ASSERT_THAT(mat2.channels(), Eq(3));
  // TODO: Getting pixel data doesn't quite work
  auto pixel = mat2.at<cv::Vec3b>(384, 0);
  ASSERT_THAT(pixel[0], Eq(77));

  ASSERT_THAT(mat2.channels(), Eq(3));
  const cv::Mat_<cv::Vec3b> vec3 = mat2;
  ASSERT_THAT(vec3(384, 0).val[0], Eq(0));
  ASSERT_THAT(vec3(384, 0)[1], Eq(110));
  ASSERT_THAT(vec3(384, 0)[2], Eq(116));

  // Operations
  Mat a = Mat::eye(Size(3, 2), CV_32F);
  Mat b = Mat::ones(Size(3, 2), CV_32F);
  Mat c = a + b;
}
} // namespace
} //  namespace opencv_hello

