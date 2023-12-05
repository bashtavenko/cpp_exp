#ifndef TRACING_TRACKING_H_
#define TRACING_TRACKING_H_

#include "absl/status/status.h"

namespace hello::tracking {

absl::Status Farneback(absl::string_view file_name);
absl::Status Kalman(absl::string_view file_name);

} // namespace hello::kalman

#endif //TRACING_TRACKING_H_
