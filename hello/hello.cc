#include <string>

#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"

namespace hello {
    std::string Greet(absl::string_view person) {
        return absl::StrCat("Hello ", person);
    }
}  // namespace hello