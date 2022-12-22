#include "keypoints.h"

#include <filesystem>
#include <glog/logging.h>
#include "absl/strings/str_format.h"
#include "absl/algorithm/container.h"

#include "util/status_macros.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>

namespace hello::keypoints {
using ::std::filesystem::path;

constexpr absl::string_view kTestDataPath = "testdata";

constexpr double kDistanceCoef = 4.0;
constexpr int kMaxMatchingSize = 50;

inline void detect_and_compute(DescriptorType type,
                               cv::Mat& img,
                               std::vector<cv::KeyPoint>& kpts,
                               cv::Mat& desc) {
  // kFast and kBlob don't work - no matches
  if (type == DescriptorType::kFast) {
    cv::Ptr<cv::FastFeatureDetector>
        detector = cv::FastFeatureDetector::create(10, true);
    detector->detect(img, kpts);
  }
  if (type == DescriptorType::kBlob) {
    cv::Ptr<cv::SimpleBlobDetector>
        detector = cv::SimpleBlobDetector::create();
    detector->detect(img, kpts);
  }
  if (type == DescriptorType::kSift) {
    cv::Ptr<cv::Feature2D> sift = cv::SIFT::create();
    sift->detectAndCompute(img, cv::Mat(), kpts, desc);
  }
  if (type == DescriptorType::kOrb) {
    cv::Ptr<cv::ORB> orb = cv::ORB::create();
    orb->detectAndCompute(img, cv::Mat(), kpts, desc);
  }
  if (type == DescriptorType::kBrisk) {
    cv::Ptr<cv::BRISK> brisk = cv::BRISK::create();
    brisk->detectAndCompute(img, cv::Mat(), kpts, desc);
  }
  if (type == DescriptorType::kKaze) {
    cv::Ptr<cv::KAZE> kaze = cv::KAZE::create();
    kaze->detectAndCompute(img, cv::Mat(), kpts, desc);
  }
  if (type == DescriptorType::kAkaze) {
    cv::Ptr<cv::AKAZE> akaze = cv::AKAZE::create();
    akaze->detectAndCompute(img, cv::Mat(), kpts, desc);
  }
}

absl::Status match(MatchAlgorithm type,
                   cv::Mat& desc1,
                   cv::Mat& desc2,
                   std::vector<cv::DMatch>& matches) {
  matches.clear();
  if (type == MatchAlgorithm::kBf) {
    cv::BFMatcher desc_matcher(cv::NORM_L2, true);
    desc_matcher.match(desc1, desc2, matches, cv::Mat());
  }
  if (type == MatchAlgorithm::kKnn) {
    cv::BFMatcher desc_matcher(cv::NORM_L2, true);
    std::vector<std::vector<cv::DMatch>> vmatches;
    desc_matcher.knnMatch(desc1, desc2, vmatches, 1);
    for (int i = 0; i < static_cast<int>(vmatches.size()); ++i) {
      if (!vmatches[i].size()) {
        continue;
      }
      matches.push_back(vmatches[i][0]);
    }
  }
  if (matches.empty()) return absl::InternalError("No matches");
  std::sort(matches.begin(), matches.end());
  while (matches.front().distance * kDistanceCoef < matches.back().distance) {
    matches.pop_back();
  }
  while (matches.size() > kMaxMatchingSize) {
    matches.pop_back();
  }
  return absl::OkStatus();
}

inline void findKeyPointsHomography(std::vector<cv::KeyPoint>& kpts1,
                                    std::vector<cv::KeyPoint>& kpts2,
                                    std::vector<cv::DMatch>& matches,
                                    std::vector<char>& match_mask) {
  if (static_cast<int>(match_mask.size()) < 3) {
    return;
  }
  std::vector<cv::Point2f> pts1;
  std::vector<cv::Point2f> pts2;
  for (int i = 0; i < static_cast<int>(matches.size()); ++i) {
    pts1.push_back(kpts1[matches[i].queryIdx].pt);
    pts2.push_back(kpts2[matches[i].trainIdx].pt);
  }
  findHomography(pts1, pts2, cv::RANSAC, 4, match_mask);
}

absl::Status Run(DescriptorType descriptor_type,
                 MatchAlgorithm match_algorithm,
                 absl::string_view image_file_name,
                 absl::string_view scene_file_name) {
  cv::Mat img1 = cv::imread(path(kTestDataPath) / image_file_name);
  if (img1.empty())
    return absl::InternalError(absl::StrCat("No image - ",
                                            image_file_name));

  cv::Mat img2 = cv::imread(path(kTestDataPath) / scene_file_name);
  if (img2.empty())
    return absl::InternalError(absl::StrCat("No scene - ",
                                            scene_file_name));

  if (img1.channels() != 1) {
    cvtColor(img1, img1, cv::COLOR_RGB2GRAY);
  }

  if (img2.channels() != 1) {
    cvtColor(img2, img2, cv::COLOR_RGB2GRAY);
  }

  std::vector<cv::KeyPoint> kpts1;
  std::vector<cv::KeyPoint> kpts2;

  cv::Mat desc1;
  cv::Mat desc2;

  std::vector<cv::DMatch> matches;

  detect_and_compute(descriptor_type, img1, kpts1, desc1);
  detect_and_compute(descriptor_type, img2, kpts2, desc2);

  RETURN_IF_ERROR(match(match_algorithm, desc1, desc2, matches));

  std::vector<char> match_mask(matches.size(), 1);
  findKeyPointsHomography(kpts1, kpts2, matches, match_mask);

  cv::Mat res;
  cv::drawMatches(img1,
                  kpts1,
                  img2,
                  kpts2,
                  matches,
                  res,
                  cv::Scalar::all(-1),
                  cv::Scalar::all(-1),
                  match_mask,
                  cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

  cv::imshow("result", res);
  cv::waitKey(0);

  return absl::OkStatus();
}

absl::Status RunCalibration() {
  constexpr absl::string_view kDirectory = "testdata/calibration";
  constexpr int kBoardW = 12;
  constexpr int kBoardH = 12;
  constexpr int kNumberOfBoards = 28;
  constexpr int kDelay = 250;
  constexpr float kScaleFactor = 0.5f;

  using std::filesystem::directory_iterator;

  std::vector<std::string> file_names;

  for (auto const& dir_entry: directory_iterator(kDirectory)) {
    file_names.push_back(dir_entry.path());
  }
  absl::c_sort(file_names);
  int board_n = kBoardW * kBoardH;  // number of corners
  cv::Size
      board_sz = cv::Size(kBoardW, kBoardH); // width and height of the board

  // PROVIDE PPOINT STORAGE
  //
  std::vector<std::vector<cv::Point2f>> image_points;
  std::vector<std::vector<cv::Point3f>> object_points;

  cv::Size image_size;
  int board_count = 0;
  for (size_t i = 0; (i < file_names.size()) && (board_count < kNumberOfBoards);
       ++i) {
    cv::Mat image;
    cv::Mat image0 = cv::imread(file_names[i]);
    board_count += 1;
    if (!image0.data) {  // protect against no file
      LOG(ERROR) << absl::StreamFormat("file #%i is not am image", i);
      continue;
    }
    image_size = image0.size();
    cv::resize(image0,
               image,
               cv::Size(),
               kScaleFactor,
               kScaleFactor,
               cv::INTER_LINEAR);

    // Find the board
    //
    std::vector<cv::Point2f> corners;
    bool found = cv::findChessboardCorners(image, board_sz, corners);

    // Draw it
    //
    drawChessboardCorners(image,
                          board_sz,
                          corners,
                          found);  // will draw only if found

    // If we got a good board, add it to our data
    //
    if (found) {
      image ^= cv::Scalar::all(255);
      cv::Mat mcorners(corners);

      // do not copy the data
      mcorners *= (1.0 / kScaleFactor);

      // scale the corner coordinates
      image_points.push_back(corners);
      object_points.push_back(std::vector<cv::Point3f>());
      std::vector<cv::Point3f>& opts = object_points.back();

      opts.resize(board_n);
      for (int j = 0; j < board_n; j++) {
        opts[j] = cv::Point3f(static_cast<float>(j / kBoardW),
                              static_cast<float>(j % kBoardW), 0.0f);
      }
      LOG(INFO) << absl::StreamFormat(
          "Collected %i total boards. This one from chessboard image %i %s",
          static_cast<int>(image_points.size()), i, file_names[i]);
    }
    cv::imshow("Calibration", image);

    // show in color if we did collect the image
    if ((cv::waitKey(kDelay) & 255) == 27) {
      return absl::InternalError("Cancelled");
    }
  }
  if (image_points.empty()) return absl::InternalError("No image points");

  // END COLLECTION WHILE LOOP.
  cv::destroyWindow("Calibration");
  LOG(INFO) << "CALIBRATING THE CAMERA...";

  /////////// CALIBRATE //////////////////////////////////////////////
  // CALIBRATE THE CAMERA!
  //
  cv::Mat intrinsic_matrix, distortion_coeffs;
  double err = cv::calibrateCamera(
      object_points, image_points, image_size, intrinsic_matrix,
      distortion_coeffs, cv::noArray(), cv::noArray(),
      cv::CALIB_ZERO_TANGENT_DIST | cv::CALIB_FIX_PRINCIPAL_POINT);

  // SAVE THE INTRINSICS AND DISTORTIONS
  LOG(INFO) << absl::StreamFormat("DONE!. Reprojection error is %f ", err);
  cv::FileStorage fs("intrinsics.xml", cv::FileStorage::WRITE);
  fs << "image_width" << image_size.width << "image_height" << image_size.height
     << "camera_matrix" << intrinsic_matrix << "distortion_coefficients"
     << distortion_coeffs;
  fs.release();

  // EXAMPLE OF LOADING THESE MATRICES BACK IN:
  fs.open("intrinsics.xml", cv::FileStorage::READ);
  LOG(INFO) << "image width: " << static_cast<int>(fs["image_width"]);
  LOG(INFO) << "image height: " << static_cast<int>(fs["image_height"]);
  cv::Mat intrinsic_matrix_loaded, distortion_coeffs_loaded;
  fs["camera_matrix"] >> intrinsic_matrix_loaded;
  fs["distortion_coefficients"] >> distortion_coeffs_loaded;
  LOG(INFO) << "intrinsic matrix:" << intrinsic_matrix_loaded;
  LOG(INFO) << "distortion coefficients: " << distortion_coeffs_loaded;

  // Build the undistort map which we will use for all
  // subsequent frames.
  //
  cv::Mat map1, map2;
  cv::initUndistortRectifyMap(intrinsic_matrix_loaded, distortion_coeffs_loaded,
                              cv::Mat(), intrinsic_matrix_loaded, image_size,
                              CV_16SC2, map1, map2);

  return absl::OkStatus();
}

} // namespace hello::keypoints
