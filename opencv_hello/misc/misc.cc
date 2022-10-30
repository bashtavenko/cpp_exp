#include <filesystem>

#include "misc.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

namespace hello::misc {
using ::std::filesystem::path;

constexpr absl::string_view kTestDataPath = "testdata";

absl::Status ShowPicture() {
  cv::Mat img = cv::imread(path(kTestDataPath) / "starry_night.jpg");
  if (img.empty()) return absl::InternalError("No image");
  cv::namedWindow( "Example 2-1", cv::WINDOW_AUTOSIZE );
  cv::imshow( "Example 2-1", img );
  cv::waitKey( 0 );
  cv::destroyWindow( "Example 2-1" );
  return absl::OkStatus();
}

absl::Status ShowVideo() {
  // TODO: Show videos
  // cv::VideoCapture cap;
  return absl::OkStatus();
}

} // namespace hello::misc
