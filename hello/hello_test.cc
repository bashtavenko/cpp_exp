#include "hello.h"
#include <string>
#include "gtest/gtest.h"

namespace hello {
namespace {

TEST(Greeter, Works) { EXPECT_EQ("Hello tester", Greet("tester")); }

}  // namespace
}  // namespace hello