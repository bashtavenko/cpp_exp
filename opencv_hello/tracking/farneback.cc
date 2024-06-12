#include <filesystem>
#include "tracking.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include "absl/strings/str_cat.h"

namespace hello::tracking {
constexpr absl::string_view kTestDataPath = "testdata";

constexpr double pyr_scale = 0.85;  // Scale between pyramid levels (< 1.0)
constexpr int levels = 7;           // Number of pyramid levels
constexpr int win_size = 13;         // Size of window for pre-smoothing pass
constexpr int iterations = 6;      // Iterations for each pyramid level
constexpr int poly_n = 5;           // Area over which polynomial will be fit
constexpr double poly_sigma = 1.1;  // Width of fit polygon

cv::Mat GetOptFlowImage(cv::Mat& optflow, cv::Mat& img) {
  cv::Scalar arrow_color(0, 0, 255);
  cv::Mat res = img.clone();
  res /= 2;  // making image darker
  int rows = res.rows;
  int cols = res.cols;
  const int step = 12;
  for (int x = (step >> 1); x < rows; x += step)
    for (int y = (step >> 1); y < cols; y += step) {
      float vx = optflow.at<cv::Vec2f>(x, y)[0];
      float vy = optflow.at<cv::Vec2f>(x, y)[1];
      cv::Point pt1(y, x);
      cv::Point pt2(y + vx, x + vy);
      cv::arrowedLine(res, pt1, pt2, arrow_color, 1);
    }
  return res;
}

absl::Status Farneback(absl::string_view file_name) {
  using std::filesystem::path;
  std::string file_path = path(kTestDataPath) / file_name;

  cv::VideoCapture capture(file_path);
  if (!capture.isOpened())
    return absl::InternalError(absl::StrCat("No video - ", file_path));

  cv::Mat optflow;  // optical flow result
  cv::Mat optflow_image;  // optical flow visualization
  cv::Mat prev_frame;  // previous frame grayscale image
  cv::Mat frame;  // current frame grayscale image
  cv::Mat colored_frame;  // current frame RGB-image
  constexpr char kWindowName[] = "Farneback";

  cv::namedWindow(kWindowName, cv::WND_PROP_FULLSCREEN);

  while ((cv::waitKey(10) & 255) != 27) {
    capture >> colored_frame;
    if (!colored_frame.rows || !colored_frame.cols) {
      break;
    }
    if (colored_frame.type() == CV_8UC3) {
      cvtColor(colored_frame, frame, cv::COLOR_BGR2GRAY);
    }
    if (prev_frame.rows) {
      calcOpticalFlowFarneback(prev_frame,
                               frame,
                               optflow,
                               pyr_scale,
                               levels,
                               win_size,
                               iterations,
                               poly_n,
                               poly_sigma,
                               cv::OPTFLOW_FARNEBACK_GAUSSIAN);
      optflow_image = GetOptFlowImage(optflow, colored_frame);
      cv::imshow(kWindowName, optflow_image);
    }
    prev_frame = frame.clone();
  }
  cv::destroyAllWindows();

  return absl::OkStatus();
}

} // namespace hello::tracking