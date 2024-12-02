#include "sudoku/digit_detector.h"
#include <opencv2/opencv.hpp>
#include "glog/logging.h"
#include <filesystem>

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
  std::vector<cv::Mat> images;
  std::vector<int> label_list;

  // Make samples from actual Sudoku image 9.
  auto synthetic_nine = [&]() {
    cv::Mat actual_nine = cv::imread("sudoku/testdata/digit-9.png", cv::IMREAD_GRAYSCALE);
    CHECK(!actual_nine.empty()) << "failed to load real Sudoku sample.";

    // Invert the sample
    cv::Mat inverted_image;
    cv::bitwise_not(actual_nine, inverted_image);

    cv::Mat base_image;
    cv::resize(inverted_image, base_image, cv::Size(28, 28));

    for (int i = 0; i < 7000 ; ++i) {
      cv::Mat nine_sample;
      base_image.copyTo(nine_sample);

      // Apply random rotation
      double angle = -10 + (std::rand() % 31);  // Random angle between -15° and +15°
      cv::Mat rotation_matrix = cv::getRotationMatrix2D(cv::Point(14, 14), angle, 1.0);
      cv::warpAffine(nine_sample, nine_sample, rotation_matrix, nine_sample.size());

      // Apply random Gaussian blur
      int kernel_size = 1 + 2 * (std::rand() % 3);  // Random kernel size: 1, 3, or 5
      cv::GaussianBlur(nine_sample, nine_sample, cv::Size(kernel_size, kernel_size), 0);

      // Noise
      cv::Mat noise = cv::Mat::zeros(nine_sample.size(), nine_sample.type());
      cv::randn(noise, 0, 10);  // Mean 0, standard deviation 10
      nine_sample += noise;

      images.push_back(nine_sample);
      label_list.push_back(9);
    }
  };

  // Ignore 0
  for (int digit = 1; digit <= 9; ++digit) {
    std::string digit_dir =
        std::string(mnist_directory.data()) + "/" + std::to_string(digit);
    for (const auto& entry : std::filesystem::directory_iterator(digit_dir)) {
      cv::Mat img = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);
      if (img.empty()) {
        LOG(ERROR) << "Failed to load image: " << entry.path();
        return false;
      }
      CHECK(img.size() == cv::Size(28, 28));
      images.push_back(img);
      label_list.push_back(digit);
    }
  }

  // Add blank samples and label them as 0
  for (int i = 0; i < 100; ++i) {
    cv::Mat blank_image = cv::Mat::zeros(28, 28, CV_8U) * 255;  // All black
    images.push_back(blank_image);
    label_list.push_back(0);
  }

  // Create training data and labels as matrices
  cv::Mat training_data(images.size(), 28 * 28, CV_32F);
  cv::Mat labels = cv::Mat(label_list).reshape(1, label_list.size());

  for (size_t i = 0; i < images.size(); ++i) {
    cv::Mat flattened_image = images[i].reshape(1, 1);  // Flatten to single row
    flattened_image.convertTo(training_data.row(static_cast<int>(i)),
                              CV_32F, 1.0 / 255.0);  // Normalize to [0, 1]
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