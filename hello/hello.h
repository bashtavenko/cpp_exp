#ifndef CPP_EXP_HELLO_H
#define CPP_EXP_HELLO_H
#include <string>

#include "absl/strings/string_view.h"

namespace hello {
    std::string Greet(absl::string_view person);
}

#endif //CPP_EXP_HELLO_H
