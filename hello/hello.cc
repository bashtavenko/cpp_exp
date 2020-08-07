#include <string>

#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "base/statusor.h"

namespace hello {

    differential_privacy::base::StatusOr<int> s;
    std::string Greet(absl::string_view person) {
        return absl::StrCat("Hello ", person);
    }
}  // namespace hello