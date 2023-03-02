#ifndef CALIBRATION_CALIBRATION_H_
#define CALIBRATION_CALIBRATION_H_

#include <opencv2/calib3d.hpp>
#include <opencv2/core/utility.hpp>

namespace hello::calibration {

// Returns 3x3 homography matrix based on rotation and translation vectors.
// * rvec - 3x1 rotation vector
// * tvec - 3x1 translation vector
cv::Mat MakeHomographyMatrix(const cv::Mat& rvec, const cv::Mat& tvec);

// Returns world point for the given pixel screen coordinate
// * pixel - 3 x 1 matrix for screen coordinate in [x, y, 1]
// * camera_matrix - 3 x 3 camera matrix
// * homography_matrix - 3 x 3 homography matrix
cv::Mat PixelToPoint(const cv::Mat& pixel, const cv::Mat& camera_matrix,
                     const cv::Mat& homography_matrix);

// Normalize homogeneous coordinates.
cv::Mat NormalizeCoordinate(const cv::Mat& x);

} // namespace hello::calibration
#endif //CALIBRATION_CALIBRATION_H_
