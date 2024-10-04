#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <Eigen/Eigenvalues>
#include "glog/logging.h"

namespace {
using ::Eigen::MatrixXcd;
using ::Eigen::MatrixXd;
using ::Eigen::Vector2d;
using ::Eigen::VectorXcd;
using ::testing::DoubleNear;

constexpr double kMaxAbsError = 0.1;

// --test_output=all
TEST(Eigen, Works) {
  MatrixXd a{{3, 1}, {1, 3}};

  // Random column vectors that are produced by "a" go to different directions:
  Vector2d v10{1, 2};
  auto b1 = a * v10;
  EXPECT_THAT(b1[0], 5);
  EXPECT_THAT(b1[1], 7);

  Vector2d v20{2, 6};
  auto b2 = a * v20;
  EXPECT_THAT(b2[0], 12);
  EXPECT_THAT(b2[1], 20);

  // Find eigenvalues and eigenvectors
  Eigen::EigenSolver<Eigen::MatrixXd> solver(a);

  VectorXcd ei_vals = solver.eigenvalues();
  MatrixXcd ei_vecs = solver.eigenvectors();
  auto l1 = ei_vals(0);
  auto l2 = ei_vals(1);
  EXPECT_THAT(l1.real(), DoubleNear(4, kMaxAbsError));
  EXPECT_THAT(l2.real(), DoubleNear(2, kMaxAbsError));

  // Now those eigenvectors (normalized) [4, 4] and [-2, 2] go
  // to the same direction
  VectorXcd v1 = ei_vecs.col(0);
  EXPECT_THAT(v1(0).real(), DoubleNear(0.7071, kMaxAbsError));
  EXPECT_THAT(v1(1).real(), DoubleNear(0.7071, kMaxAbsError));

  VectorXcd v2 = ei_vecs.col(1);
  EXPECT_THAT(v2(0).real(), DoubleNear(-0.7071, kMaxAbsError));
  EXPECT_THAT(v2(1).real(), DoubleNear(0.7071, kMaxAbsError));
}

}  // namespace