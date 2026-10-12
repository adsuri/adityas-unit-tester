#include "UnitTester.hpp"
#include <exception>
#include <stdexcept>
#include <vector>
#include <string>
#include <cmath>

// passing tests

TEST(test_equal) {
  EXPECT_EQUAL(2 + 3, 5);
  EXPECT_EQUAL(std::string("hello"), std::string("hello"));
}

TEST(test_equal_custom) {
  auto func = [](int a, int b) { return std::abs(b - a) <= 1; };

  EXPECT_EQUAL_CUSTOM(5, 5, std::equal_to<>{});
  EXPECT_EQUAL_CUSTOM(5, 6, func);
}

TEST(test_near) {
  EXPECT_NEAR(0.1 + 0.2, 0.3);
  EXPECT_NEAR(1.0, 1.000001);
}

TEST(test_near_epsilon) {
  EXPECT_NEAR_EPSILON(1.0, 1.0001, 0.001);
  EXPECT_NEAR_EPSILON(100.0, 100.01, 0.02);
}

TEST(test_not_equal) {
  EXPECT_NOT_EQUAL(5, 6);
  EXPECT_NOT_EQUAL(2 + 2, 5);
}

TEST(test_not_equal_custom) {
  auto func = [](int a, int b) { return a != b; };

  EXPECT_NOT_EQUAL_CUSTOM(5, 6, std::not_equal_to<>{});
  EXPECT_NOT_EQUAL_CUSTOM(5, 6, func);
}

TEST(test_less_than) {
  EXPECT_LT(2, 3);
  EXPECT_LT(-5, 0);
}

TEST(test_less_than_custom) {
  EXPECT_LT_CUSTOM(2, 3, std::less<>{});
}

TEST(test_less_than_equal) {
  EXPECT_LT_EQUAL(2, 3);
  EXPECT_LT_EQUAL(3, 3);
}

TEST(test_less_than_equal_custom) {
  EXPECT_LT_EQUAL_CUSTOM(3, 3, std::less_equal<>{});
}

TEST(test_greater_than) {
  EXPECT_GT(5, 3);
  EXPECT_GT(1, -1);
}

TEST(test_greater_than_custom) {
  EXPECT_GT_CUSTOM(5, 3, std::greater<>{});
}

TEST(test_greater_than_equal) {
  EXPECT_GT_EQUAL(5, 3);
  EXPECT_GT_EQUAL(3, 3);
}

TEST(test_greater_than_equal_custom) {
  EXPECT_GT_EQUAL_CUSTOM(3, 3, std::greater_equal<>{});
}

TEST(test_true) {
  EXPECT_TRUE(true);
  EXPECT_TRUE(2 + 3 == 5);
}

TEST(test_false) {
  EXPECT_FALSE(false);
  EXPECT_FALSE(2 + 3 == 4);
}

TEST(test_throws) {
  auto func1 = [] { throw std::runtime_error("expected"); };
  auto func2 = [] { throw std::invalid_argument("invalid"); };

  EXPECT_THROWS(func1, std::runtime_error);
  EXPECT_THROWS(func2, std::exception);
}

TEST(test_no_throw) {
  auto func1 = [] {
    int value = 42;
    return value;
  };

  EXPECT_NO_THROW(func1);
}

TEST(test_null) {
  int* ptr = nullptr;
  EXPECT_NULL(ptr);
}

TEST(test_non_null) {
  int value = 42;
  int *ptr = nullptr;
  ptr = &value;
  EXPECT_NON_NULL(ptr);
}

TEST(test_empty) {
  std::vector<int> values;
  EXPECT_EMPTY(values);

  std::string text;
  EXPECT_EMPTY(text);
}

TEST(test_size) {
  std::vector<int> values{1, 2, 3};
  EXPECT_SIZE(values, 3);

  std::string text = "hello";
  EXPECT_SIZE(text, 5);
}

TEST(test_all) {
  std::vector<int> values{2, 4, 6, 8};

  auto pred = [](int n) { return n % 2 == 0; };

  EXPECT_ALL(values, pred);
}

TEST(test_any) {
  std::vector<int> values{1, 3, 4, 7};

  auto pred = [](int n) { return n % 2 == 0; };

  EXPECT_ANY(values, pred);
}

TEST(test_none) {
  std::vector<int> values{1, 3, 5, 7};

  auto pred = [](int n) { return n % 2 == 0; };

  EXPECT_NONE(values, pred);
}

// failing tests

TEST(fail_equal) {
  EXPECT_EQUAL(2 + 3, 6);
}

TEST(fail_equal_custom) {
  EXPECT_EQUAL_CUSTOM(5, 6, std::equal_to<>{});
}

TEST(fail_near) {
  EXPECT_NEAR(1.0, 2.0);
}

TEST(fail_near_epsilon) {
  EXPECT_NEAR_EPSILON(1.0, 1.1, 0.01);
}

TEST(fail_not_equal) {
  EXPECT_NOT_EQUAL(5, 5);
}

TEST(fail_not_equal_custom) {
  EXPECT_NOT_EQUAL_CUSTOM(5, 5, std::equal_to<>{});
}

TEST(fail_less_than) {
  EXPECT_LT(5, 3);
}

TEST(fail_less_than_custom) {
  EXPECT_LT_CUSTOM(5, 3, std::less<>{});
}

TEST(fail_less_than_equal) {
  EXPECT_LT_EQUAL(5, 3);
}

TEST(fail_less_than_equal_custom) {
  EXPECT_LT_EQUAL_CUSTOM(5, 3, std::less_equal<>{});
}

TEST(fail_greater_than) {
  EXPECT_GT(2, 5);
}

TEST(fail_greater_than_custom) {
  EXPECT_GT_CUSTOM(2, 5, std::greater<>{});
}

TEST(fail_greater_than_equal) {
  EXPECT_GT_EQUAL(2, 5);
}

TEST(fail_greater_than_equal_custom) {
  EXPECT_GT_EQUAL_CUSTOM(2, 5, std::greater_equal<>{});
}

TEST(fail_true) {
  EXPECT_TRUE(false);
}

TEST(fail_false) {
  EXPECT_FALSE(true);
}

TEST(fail_throws_wrong_exception) {
  auto func = [] { throw std::runtime_error("wrong type"); };

  EXPECT_THROWS(func, std::invalid_argument);
}

TEST(fail_throws_no_exception) {
  auto func = [] {
    int value = 42;
    return value;
  };

  EXPECT_THROWS(func, std::runtime_error);
}

TEST(fail_no_throw) {
  auto func = [] {throw std::runtime_error("unexpected"); };

  EXPECT_NO_THROW(func);
}

TEST(fail_null) {
  int value = 42;
  int* ptr = &value;

  EXPECT_NULL(ptr);
}

TEST(fail_non_null) {
  int* ptr = nullptr;

  EXPECT_NON_NULL(ptr);
}

TEST(fail_empty) {
  std::vector<int> values{1};

  EXPECT_EMPTY(values);
}

TEST(fail_size) {
  std::vector<int> values{1, 2, 3};

  EXPECT_SIZE(values, 4);
}

TEST(fail_all) {
  std::vector<int> values{2, 4, 5, 8};

  auto pred = [](int n) { return n % 2 == 0; };

  EXPECT_ALL(values, pred);
}

TEST(fail_any) {
  std::vector<int> values{1, 3, 5, 7};

  auto pred = [](int n) { return n % 2 == 0; };

  EXPECT_ANY(values, pred);
}

TEST(fail_none) {
  std::vector<int> values{1, 2, 3, 5};

  auto pred = [](int n) { return n % 2 == 0; };

  EXPECT_NONE(values, pred);
}

// misc behavior tests

TEST(test_multiple_failures) {
  EXPECT_EQUAL(1, 2);
  EXPECT_TRUE(false);
  EXPECT_SIZE((std::vector<int>{1, 2}), 3);
}

TEST(test_error) {
  throw std::runtime_error("intentional unhandled test exception");
}

TEST(test_after_error) {
  EXPECT_EQUAL(2 + 3, 5);
}

TEST(test_near_vs_exact_equality) {
  EXPECT_NEAR(0.1 + 0.2, 0.3);
  EXPECT_EQUAL(0.1 + 0.2, 0.3);
}

TEST_MAIN()