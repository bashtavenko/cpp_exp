#include "calibration.h"
#include <glog/logging.h>

namespace hello::calibration {

cv::Mat MakeHomographyMatrix(const cv::Mat& rvec, const cv::Mat& tvec) {
  CHECK_EQ(rvec.rows, 3);
  CHECK_EQ(rvec.cols, 1);
  CHECK_EQ(tvec.rows, 3);
  CHECK_EQ(tvec.cols, 1);
  cv::Mat rotation(3, 3, cv::DataType<double>::type);
  cv::Rodrigues(rvec, rotation);
  // This essentially makes 3x3 homography matrix because the last column
  // is not needed.
  rotation.at<double>(0, 2) = tvec.at<double>(0, 0);
  rotation.at<double>(1, 2) = tvec.at<double>(1, 0);
  rotation.at<double>(2, 2) = tvec.at<double>(2, 0);
  return rotation;
}

cv::Mat PixelToPoint(const cv::Mat& pixel, const cv::Mat& camera_matrix,
                     const cv::Mat& homography_matrix) {
  // q = M * H * Q
  // Q = (M * H)-inv * q
  return (camera_matrix * homography_matrix).inv() * pixel;
}

cv::Mat NormalizeCoordinate(const cv::Mat& x) {
  CHECK_EQ(x.rows, 3);
  CHECK_EQ(x.cols, 1);
  CHECK_GT(x.at<double>(2, 0), 0);
  return cv::Mat({3, 1}, {x.at<double>(0, 0) / x.at<double>(2, 0),
                          x.at<double>(1, 0) / x.at<double>(2, 0), 1.0});
}

} // namespace hello::calibration