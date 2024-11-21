// Sudoku detection and recognition
#ifndef OPENCV_HELLO_SUDOKU_DETECTION_H_
#define OPENCV_HELLO_SUDOKU_DETECTION_H_
#include <vector>
#include "opencv2/core.hpp"

namespace sudoku {

std::vector<cv::Mat> DetectCells(const cv::Mat& image);

// This is possible but noisy.
std::vector<cv::Mat> DetectCellsWithContours(const cv::Mat& image);

} // namespace sudoku

#endif  // OPENCV_HELLO_SUDOKU_DETECTION_H_
