#include "include/gtest/gtest.h"
#include "opencv/opencv_cmake/include/opencv2/core.hpp"

namespace opencv_hello {
namespace {
using cv::Mat;
using cv::Scalar;

TEST(Mat, Works) {
  Mat mat(2, 2, CV_8UC1, Scalar(0, 0, 255));
  EXPECT_EQ("Hello", "Hello");
}
} // namespace
} //  namespace opencv_hello

