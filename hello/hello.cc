#include <string>

#include "absl/strings/str_cat.h"
#include <string_view>

namespace hello {
    std::string Greet(std::string_view person) {
        return absl::StrCat("Hello ", person);
    }
}  // namespace hello