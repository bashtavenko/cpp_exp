#include "sudoku/digit_detector.h"
#include <filesystem>
#include <opencv2/opencv.hpp>
#include "glog/logging.h"

namespace sudoku {

void DigitDetector::Init(absl::string_view model_path) {
  model_ = cv::ml::KNearest::load(std::string(model_path));
  CHECK(model_ != nullptr) << "Failed to load model from: " << model_path;
}

absl::optional<int32_t> DigitDetector::Detect(const cv::Mat& image) {
  if (image.empty()) return std::nullopt;
  if (!model_->isTrained()) {
    LOG(ERROR) << "Model is not trained";
    return std::nullopt;
  }

  cv::Mat gray_image;
  if (image.channels() == 3) {
    cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);
  } else {
    gray_image = image;
  }

  // Invert the image - white digit on black background due to MNIST examples.
  cv::Mat inverted_image;
  cv::bitwise_not(gray_image, inverted_image);

  // Resize the input image to 28x28 MNIST input
  cv::Mat resized_image;
  cv::resize(inverted_image, resized_image, cv::Size(28, 28));

  // Preprocess the image (e.g., flatten and normalize)
  cv::Mat processed_image =
      resized_image.reshape(1, 1);  // Flatten to a row vector
  processed_image.convertTo(processed_image, CV_32F,
                            1.0 / 255.0);  // Ensure correct type

  // Predict the digit
  auto knn_model = GetModelAs<cv::ml::KNearest>();
  if (!knn_model) {
    LOG(ERROR) << "Model is not KNearest instance";
    return std::nullopt;
  }
  cv::Mat results;
  float response = knn_model->findNearest(processed_image,
                                          knn_model->getDefaultK(), results);
  if (response < 0) return std::nullopt;

  return static_cast<int32_t>(response);
}

bool DigitDetector::Train(absl::string_view mnist_directory,
                          absl::string_view model_path) {
  auto knn_model = cv::ml::KNearest::create();

  // Load MNIST data and preprocess
  cv::Mat training_data;
  cv::Mat labels;
  std::vector<cv::Mat> images;
  std::vector<int> label_list;

  auto synthetic_nine = [&]() {
    constexpr char text[] = "9";
    constexpr int32_t font_face =  cv::FONT_HERSHEY_SIMPLEX;
    constexpr double font_scale = 1.0;
    constexpr int32_t thickness = 2;
    const auto color = cv::Scalar(255); // white

    for (int i = 0; i < 1000 ; ++i) {
      cv::Mat digit_image = cv::Mat::zeros(28, 28, CV_8U);  // Start with blank black image

      // Calculate text size to center the digit
      int baseline = 0;
      cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);
      cv::Point text_org((28 - text_size.width) / 2, (28 + text_size.height) / 2);

      // Write the digit '9' on the image (white digit)
      cv::putText(digit_image, text, text_org, font_face, font_scale, color, thickness);

      // Apply slight rotation
      double angle = -15 + (std::rand() % 30);  // Random angle between -15 and 15
      cv::Mat rotation_mat = cv::getRotationMatrix2D(cv::Point(14, 14), angle, 1.0);
      cv::warpAffine(digit_image, digit_image, rotation_mat, digit_image.size());

      // Flatten and normalize
      digit_image = digit_image.reshape(1, 1);
      digit_image.convertTo(digit_image, CV_32F, 1.0 / 255.0);

      // Add to training data
      images.push_back(digit_image);
      label_list.push_back(9);
    }
  };

  for (int digit = 0; digit <= 9; ++digit) {
    std::string digit_dir =
        std::string(mnist_directory.data()) + "/" + std::to_string(digit);
    for (const auto& entry : std::filesystem::directory_iterator(digit_dir)) {
      cv::Mat img = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);
      if (img.empty()) {
        LOG(ERROR) << "Failed to load image: " << entry.path();
        return false;
      }
      images.push_back(img.reshape(1, 1));  // Flatten to a row
      label_list.push_back(digit);
    }
  }

  // Add blank samples and label them as 0
  for (int i = 0; i < 100; ++i) {
    cv::Mat blank_image = cv::Mat::ones(28, 28, CV_8U) * 255;  // All white
    blank_image = blank_image.reshape(1, 1);                   // Flatten
    blank_image.convertTo(blank_image, CV_32F, 1.0 / 255.0);
    images.push_back(blank_image);
    label_list.push_back(0);
  }

  // Add digits with 9
  synthetic_nine();

  // Create training data and labels as matrices
  training_data = cv::Mat(images.size(), images[0].cols, CV_32F);
  labels = cv::Mat(label_list).reshape(1, label_list.size());

  for (size_t i = 1; i < images.size(); ++i) {
    images[i].convertTo(training_data.row(static_cast<int>(i)), CV_32F,
                        1.0 / 255.0);
  }

  // Train the model
  knn_model->setDefaultK(3);  // Looks for at most k training examples
  if (!knn_model->train(training_data, cv::ml::ROW_SAMPLE, labels)) {
    LOG(ERROR) << "Training failed";
  }
  model_ = knn_model;
  model_->save(std::string(model_path));
  return true;
}

}  // namespace sudoku