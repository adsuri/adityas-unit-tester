#include "UnitTester.hpp"
#include <exception>

TEST(test_failed_assertion) {
  EXPECT_EQUAL(2 + 3, 5);
  EXPECT_FALSE(2 + 3 == 4);
  EXPECT_GT_EQUAL(2 + 3, 6);
}

TEST(test_good_assertion) {
  EXPECT_EQUAL(2 + 3, 5);
  EXPECT_FALSE(2 + 3 == 4);
}

TEST(test_error) {
  throw std::runtime_error("error");
}

TEST(test_near) {
  EXPECT_NEAR(0.1 + 0.2, 0.3);
  EXPECT_FALSE(0.1 + 0.2 == 0.3);
}

TEST_MAIN()