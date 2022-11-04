#include <filesystem>

#include "misc.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <glog/logging.h>
#include <glog/stl_logging.h>
#include "absl/strings/str_format.h"

namespace hello::misc {
using ::std::filesystem::path;

constexpr absl::string_view kTestDataPath = "testdata";

absl::Status ShowPicture() {
  cv::Mat img = cv::imread(path(kTestDataPath) / "starry_night.jpg");
  if (img.empty()) return absl::InternalError("No image");
  cv::namedWindow("Example 2-1", cv::WINDOW_AUTOSIZE);
  cv::imshow("Example 2-1", img);
  cv::waitKey(0);
  cv::destroyWindow("Example 2-1");
  return absl::OkStatus();
}

absl::Status ShowVideo() {
  cv::namedWindow("Example 2-3", cv::WINDOW_AUTOSIZE);
  cv::VideoCapture cap;
  cap.open(path(kTestDataPath) / "Megamind.avi");
  cv::Mat frame;
  for (;;) {
    cap >> frame;
    if (frame.empty()) break;
    cv::imshow("Example 2-3", frame);
    if ((char) cv::waitKey(/*delay=*/33) >= 0) break;
  }
  return absl::OkStatus();
}

namespace global {
int run;
int dont_set;
int current_pos;
cv::VideoCapture cap;

void OnTrackbarSlide(int, void*) {
  cap.set(cv::CAP_PROP_POS_FRAMES, current_pos);
  if (!dont_set) run = 1;
  dont_set = 0;
}

absl::Status ShowVideoWithTaskBar() {
  int slider_position = 0;
  run = 1;
  dont_set = 0; //start out in single step mode
  cv::namedWindow("Example 2-4", cv::WINDOW_AUTOSIZE);
  cap.open(path(kTestDataPath) / "Megamind.avi");
  int frames = (int) cap.get(cv::CAP_PROP_FRAME_COUNT);
  int width = (int) cap.get(cv::CAP_PROP_FRAME_WIDTH);
  int height = (int) cap.get(cv::CAP_PROP_FRAME_HEIGHT);

  LOG(INFO) << absl::StreamFormat("Video has %d frames of %d x %d",
                                  frames,
                                  width,
                                  height);
  cv::createTrackbar(
      "Position",
      "Example 2-4",
      &slider_position,
      frames, OnTrackbarSlide);

  cv::Mat frame;
  for (;;) {
    if (run != 0) {
      cap >> frame;
      if (frame.empty()) break;
      current_pos = (int) cap.get(cv::CAP_PROP_POS_FRAMES);
      dont_set = 1;

      cv::setTrackbarPos("Position", "Example 2-4", current_pos);
      cv::imshow("Example 2-4", frame);
      run -= 1;
    }
    char c = (char) cv::waitKey(10);
    if (c == 's') { // single step
      run = 1;
      LOG(INFO) << "Single step, run = " << run;
    }

    if (c == 'r') { // run mode
      run = -1;
      LOG(INFO) << "Run mode, run = " << run;
    }

    if (c == 27) break;
  }
  return absl::OkStatus();
}
}
} // namespace hello::misc
