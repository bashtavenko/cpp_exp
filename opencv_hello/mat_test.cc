#include "include/gtest/gtest.h"
#include "opencv2/opencv.hpp"
#include "tools/cpp/runfiles/runfiles.h"

namespace opencv_hello {
namespace {
using bazel::tools::cpp::runfiles::Runfiles;
using cv::Mat;
using cv::Scalar;
using cv::imread;

TEST(Mat, Works) {
  Runfiles* runfiles = Runfiles::CreateForTest();
  ASSERT_NE(runfiles, nullptr);

  // Read from file
  Mat mat1 = imread(runfiles->Rlocation("opencv_hello/testdata/starry_night.jpg"));
  ASSERT_FALSE(mat1.empty());
  ASSERT_EQ(mat1.dims, 2);
  ASSERT_EQ(mat1.rows, 600);
  ASSERT_EQ(mat1.cols, 752);

  // Create
  Mat mat(2, 2, CV_8UC1, Scalar(0, 0, 255));
  ASSERT_EQ(mat.dims, 2);
}
} // namespace
} //  namespace opencv_hello

