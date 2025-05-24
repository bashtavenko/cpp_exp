#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/status/status.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "opencv2/opencv.hpp"

// Largely based from
// https://github.com/oreillymedia/Learning-OpenCV-3_examples/blob/master/example_16-01.cpp

ABSL_FLAG(std::string, previous_image, "testdata/optical_flow/frame_0.jpg",
          "Previous video frame");
ABSL_FLAG(std::string, next_image, "testdata/optical_flow/frame_27.jpg",
          "Current video frame");

absl::Status Run() {
  LOG(INFO) << "Running Lucas-Kanade test";
  cv::Mat img_a =
      cv::imread(absl::GetFlag(FLAGS_previous_image), cv::IMREAD_GRAYSCALE);
  CHECK(!img_a.empty());
  cv::Mat img_b =
      cv::imread(absl::GetFlag(FLAGS_next_image), cv::IMREAD_GRAYSCALE);
  CHECK(!img_b.empty());
  cv::Size img_sz = img_a.size();
  int win_size = 10;  // compute local coherent motion
  cv::Mat img_c = img_b.clone();

  std::vector<cv::Point2f> corners_a;
  std::vector<cv::Point2f> corners_b;
  constexpr int32_t kMaxCorners = 500;
  cv::goodFeaturesToTrack(img_a,          // Image to track
                          corners_a,      // Vector of detected corners (output)
                          kMaxCorners,    // Keep up to this many corners
                          0.01,           // Quality level (percent of maximum)
                          5,              // Min distance between corners
                          cv::noArray(),  // Mask
                          3,              // Block size
                          false,          // true: Harris, false: Shi-Tomasi
                          0.04            // method specific parameter
  );

  cv::cornerSubPix(
      img_a,                         // Input image
      corners_a,                     // Vector of corners (input and output)
      cv::Size(win_size, win_size),  // Half side length of search window
      cv::Size(-1, -1),              // Half side length of dead zone (-1=none)
      cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS,
                       20,   // Maximum number of iterations
                       0.03  // Minimum change per iteration
                       ));

  // Lucas Kanade algorithm
  std::vector<uchar> features_found;
  cv::calcOpticalFlowPyrLK(
      img_a,           // Previous image
      img_b,           // Next image
      corners_a,       // Previous set of corners (from imgA)
      corners_b,       // Next set of corners (from imgB)
      features_found,  // Output vector, each is 1 for tracked
      cv::noArray(),   // Output vector, lists errors (optional)
      cv::Size(win_size * 2 + 1, win_size * 2 + 1),  // Search window size
      5,  // Maximum pyramid level to construct
      cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS,
                       20,  // Maximum number of iterations
                       0.3  // Minimum change per iteration
                       ));

  // Now make some image of what we are looking at:
  // Note that if you want to track cornersB further, i.e.
  // pass them as input to the next calcOpticalFlowPyrLK,
  // you would need to "compress" the vector, i.e., exclude points for which
  // features_found[i] == false.
  for (int i = 0; i < static_cast<int>(corners_a.size()); ++i) {
    if (!features_found[i]) {
      continue;
    }
    line(img_c,                  // Draw onto this image
         corners_a[i],           // Starting here
         corners_b[i],           // Ending here
         cv::Scalar(0, 255, 0),  // This color
         1,                      // This many pixels wide
         cv::LINE_AA             // Draw line in this style
    );
  }

  cv::imshow("LK Optical Flow Example", img_c);
  cv::waitKey(0);
  LOG(INFO) << "Done.";
  return absl::OkStatus();
}

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  absl::ParseCommandLine(argc, argv);
  FLAGS_alsologtostderr = true;
  return Run().ok() ? EXIT_SUCCESS : EXIT_FAILURE;
}