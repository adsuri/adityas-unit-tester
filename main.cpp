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

TEST_MAIN()