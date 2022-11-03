#include <filesystem>

#include "misc.h"
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
  cv::namedWindow( "Example 2-3", cv::WINDOW_AUTOSIZE );
  cv::VideoCapture cap;
  cap.open(path(kTestDataPath) / "Megamind.avi");
  cv::Mat frame;
  for(;;) {
    cap >> frame;
    if (frame.empty()) break;
    cv::imshow("Example 2-3", frame);
    if ((char) cv::waitKey(/*delay=*/33) >= 0) break;
  }
  return absl::OkStatus();
}
} // namespace hello::misc
