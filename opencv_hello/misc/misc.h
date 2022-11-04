#ifndef MISC_MISC_H_
#define MISC_MISC_H_

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "absl/status/status.h"

namespace hello::misc {
namespace global {
absl::Status ShowVideoWithTaskBar();
} // namespace global

absl::Status ShowPicture();
absl::Status ShowVideo();
} // namespace hello::misc
#endif //MISC_MISC_H_
