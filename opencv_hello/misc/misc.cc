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
      LOG(INFO) << "RunCalibration mode, run = " << run;
    }

    if (c == 27) break;
  }
  return absl::OkStatus();
}
} // namespace global
absl::Status ShowPictureBlurring() {
  cv::Mat img = cv::imread(path(kTestDataPath) / "starry_night.jpg");
  if (img.empty()) return absl::InternalError("No image");
  cv::namedWindow("Example 2-5-in", cv::WINDOW_AUTOSIZE);
  cv::namedWindow("Example 2-5-out", cv::WINDOW_AUTOSIZE);
  cv::imshow("Example 2-5-in", img);
  cv::Mat out;
  cv::GaussianBlur(img, out, cv::Size(5, 5), 3, 3);
  cv::GaussianBlur(out, out, cv::Size(5, 5), 3, 3);
  cv::imshow("Example 2-5-out", out);
  cv::waitKey(0);
  return absl::OkStatus();
}
absl::Status ShowPicturePyrDown() {
  cv::Mat img = cv::imread(path(kTestDataPath) / "starry_night.jpg");
  if (img.empty()) return absl::InternalError("No image");
  cv::namedWindow("Example 2-6-in", cv::WINDOW_AUTOSIZE);
  cv::namedWindow("Example 2-6-out", cv::WINDOW_AUTOSIZE);
  cv::imshow("Example 2-6-in", img);
  cv::Mat out;
  cv::imshow("Example 2-6-in", img);
  cv::pyrDown(img, out);
  cv::imshow("Example 2-6-out", out);
  cv::waitKey(0);
  return absl::OkStatus();
}
absl::Status ShowPictureCanny() {
  cv::Mat img_rgb = cv::imread(path(kTestDataPath) / "HappyFish.jpg");
  if (img_rgb.empty()) return absl::InternalError("No image");
  cv::namedWindow("Example Gray", cv::WINDOW_AUTOSIZE);
  cv::namedWindow("Example Canny", cv::WINDOW_AUTOSIZE);
  cv::Mat img_gry;
  cv::cvtColor(img_rgb, img_gry, cv::COLOR_BGR2GRAY);
  cv::imshow("Example Gray", img_gry);
  cv::Mat img_cny;
  cv::Canny(img_gry, img_cny, 10, 100, 3, true);
  cv::imshow("Example Canny", img_cny);
  cv::waitKey(/*delay=*/0);
  return absl::OkStatus();
}
absl::Status ShowVideoCanny() {
  cv::VideoCapture cap;
  cap.open(path(kTestDataPath) / "Megamind.avi");
  double rate = cap.get(cv::CAP_PROP_FPS);
  cv::Mat MatFrame;
  cv::Mat MatGray;
  cv::Mat MatCanny;
  int delay = 1000 / rate;
  LOG(INFO) << "rate = " << rate << ", delay = " << delay;
  while (1) {
    cap >> MatFrame;
    if (!MatFrame.data)break;
    //(1)
    imshow("Raw Video", MatFrame);
    //(2)
    cv::cvtColor(MatFrame, MatGray, cv::COLOR_BGR2GRAY);
    imshow("Gray Video", MatGray);
    //(3)
    Canny(MatGray, MatCanny, 100, 255);
    imshow("Canny Video", MatCanny);
    //question a
    cv::Mat
        MatAll(MatFrame.rows, 3 * MatFrame.cols, CV_8UC3, cv::Scalar::all(0));
    cv::cvtColor(MatGray, MatGray, cv::COLOR_GRAY2BGR);
    cv::cvtColor(MatCanny, MatCanny, cv::COLOR_GRAY2BGR);
    cv::Mat MatSub = MatAll.colRange(0, MatFrame.cols);
    MatFrame.copyTo(MatSub);
    MatSub = MatAll.colRange(MatFrame.cols, 2 * MatFrame.cols);
    MatGray.copyTo(MatSub);
    MatSub = MatAll.colRange(2 * MatFrame.cols, 3 * MatFrame.cols);
    MatCanny.copyTo(MatSub);
    //question b
    cv::Scalar color = CV_RGB(255, 0, 0);
    cv::putText(MatAll,
                "raw video",
                cv::Point(50, 30),
                cv::FONT_HERSHEY_DUPLEX,
                1.0f,
                color);
    putText(MatAll,
            "gray video",
            cv::Point(50 + MatFrame.cols, 30),
            cv::FONT_HERSHEY_DUPLEX,
            1.0f,
            color);
    putText(MatAll,
            "canny video",
            cv::Point(50 + 2 * MatFrame.cols, 30),
            cv::FONT_HERSHEY_DUPLEX,
            1.0f,
            color);
    imshow("all Video", MatAll);

    if ((cv::waitKey(delay) & 255) == 27)
      break;
  }
  cv::waitKey();
  cap.release();
  return absl::OkStatus();
}
} // namespace hello::misc
