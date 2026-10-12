/*
 * Copyright (C) 2026 Aditya Suri
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef UNIT_TESTER_HPP
#define UNIT_TESTER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <limits>
#include <utility>
#include <algorithm>

class UnitTester {
 private:
  inline static const std::string RED_ = "\033[31m";
  inline static const std::string GREEN_ = "\033[32m";
  inline static const std::string YELLOW_ = "\033[33m";
  inline static const std::string BLUE_ = "\033[34m";
  inline static const std::string BOLD_ = "\033[1m";
  inline static const std::string RESET_ = "\033[0m";

  struct AssertionFailure_ {
    std::string file;
    std::string name;
    std::string msg;
    int line;
  };

  friend class FatalAssertion_;

  inline static std::vector<AssertionFailure_> current_assertion_failures_;
  friend std::ostream &operator<<(std::ostream &os, const AssertionFailure_ &failure);

  class FatalAssertion_ : public std::exception {
   private:
    UnitTester::AssertionFailure_ failure_;

    explicit FatalAssertion_(const UnitTester::AssertionFailure_ &failure)
      : failure_(failure) {}

    const char* what() const noexcept override { return failure_.msg.c_str(); }

    friend class UnitTester;
  };

 public:
  struct Test {
    std::string name;
    void (*function)();
  };

  enum class FilterAlgorithm : char {
    ALL,
    ANY,
    NONE
  };

  inline static int argc = 0;
  inline static char** argv = nullptr;

  inline static std::vector<Test> all_tests{};

  class NearEqualComparator {
   private:
    double epsilon;
  
   public:
    NearEqualComparator()
      : epsilon(0.00001) {}

    NearEqualComparator(double epsilon_)
      : epsilon(epsilon_) {}

    bool operator()(double a, double b) const {
      return std::abs(a - b) <= epsilon;
    }
  };

  static bool register_test(const char* name, void (*function)()) {
    all_tests.push_back({name, function});
    return true;
  }

  static int run_all(int argc_, char **argv_) {
    argc = argc_;
    argv= argv_;

    size_t successes = 0;

    for (const auto& test : all_tests) {
      std::cout << "Running test: " << BLUE_ << test.name << RESET_ << '\n';
      try {
        test.function();
      } catch (const FatalAssertion_ &e) {
        std::cout << e.failure_ << std::endl;
        std::cout << RED_ << "Fatal assertion failed, ending test..." << std::endl;

        current_assertion_failures_.clear();
        continue;
      } catch (const std::exception &e) {
        std::cout << RED_ << "Unhandled exception happened within test " << test.name << RESET_ << std::endl
          << e.what() << std::endl << std::endl;
        
        current_assertion_failures_.clear();
        continue;
      }

      if (!current_assertion_failures_.empty()) {
        for (const auto &failure : current_assertion_failures_) { std::cout << failure << std::endl; }
        std::cout << RED_ << "Test " << test.name << " failed." << RESET_ << std::endl << std::endl;
      } else {
        std::cout << GREEN_ << "Test " << test.name << " passed!" << RESET_ << std::endl << std::endl;
        ++successes;
      }

      current_assertion_failures_.clear();
    }

    size_t n = all_tests.size();
    std::cout << "Finished " << n << " test(s). " << successes << "/" << n
      << " tests were successful." << std::endl;

    return successes == all_tests.size() ? 0 : 1;
  }

  template <typename Comp, typename T>
  static void expect_numerical(const std::string &file, int line, const std::string &name, const T& a, const T& b, std::string operation, Comp &&comp, bool fatal) {
    if (!comp(a, b)) {
      std::ostringstream msg;
      msg << std::setprecision(std::numeric_limits<T>::max_digits10) << "Expected " << a << ' '
        << operation << ' ' << b << ", but the condition was false.";

      if (fatal) { throw FatalAssertion_{{file, name, msg.str(), line}}; }

      current_assertion_failures_.push_back({file, name, msg.str(), line});
    }
  }

  static void expect_bool(const std::string &file, int line, const std::string &name, bool a, bool target, bool fatal) {
    if (a != target) {
      std::ostringstream msg;
      msg << "Expected statement to be " << (target ? "true" : "false") << ", but statement was "
        << (a ? "true." : "false.");

      if (fatal) { throw FatalAssertion_{{file, name, msg.str(), line}}; }

      current_assertion_failures_.push_back({file, name, msg.str(), line});
    }
  }

  template <typename ExceptionType, typename FuncType>
  static void expect_throws(const std::string &file, int line, const std::string &name, FuncType &&func) {
    try {
      std::forward<FuncType>(func)();
    } catch (const ExceptionType &_) {
      return;
    } catch (...) {
      std::ostringstream msg;
      msg << "Function threw an unexpected exception.";
      current_assertion_failures_.push_back({file, name, msg.str(), line});
      return;
    }

    std::ostringstream msg;
    msg << "Function did not throw any exception.";
    current_assertion_failures_.push_back({file, name, msg.str(), line});
  }

  template <typename FuncType>
  static void expect_no_throw(const std::string &file, int line, const std::string &name, FuncType &&func) {
    try {
      std::forward<FuncType>(func)();
    } catch (const std::exception& e) {
      std::ostringstream msg;
      msg << "Function threw an unexpected exception:\n" << e.what();
      current_assertion_failures_.push_back({file, name, msg.str(), line});
      return;
    } catch (...) {
      std::ostringstream msg;
      msg << "Unexpected non-standard exception.";
      current_assertion_failures_.push_back({file, name, msg.str(), line});
    }
  }

  template <typename Ptr>
  static void expect_ptr(const std::string &file, int line, const std::string &name, const Ptr &ptr, bool expect_null, bool fatal) {
    if ((ptr == nullptr) != expect_null) {
      std::ostringstream msg;
      msg << "Expected pointer to be " << (expect_null ? "null" : "non-null")
        << ", but the condition was false.";

      if (fatal) { throw FatalAssertion_{{file, name, msg.str(), line}}; }

      current_assertion_failures_.push_back({file, name, msg.str(), line});
    }
  }

  template <typename Container>
  static void expect_size(const std::string &file, int line, const std::string &name, const Container &container, size_t size) {
    if (container.size() != size) {
      std::ostringstream msg;
      msg << "Expected container size to be " << size << ", but size was " << container.size();
      current_assertion_failures_.push_back({file, name, msg.str(), line});
    }
  }

  template <typename Container, typename Pred>
  static void expect_filter(const std::string &file, int line, const std::string &name, std::string alg_str, const Container &container, Pred &&pred, FilterAlgorithm alg) {
    bool result;

    switch (alg) {
      case FilterAlgorithm::ALL:
        result = std::all_of(container.begin(), container.end(), std::forward<Pred>(pred));
        break;

      case FilterAlgorithm::ANY:
        result = std::any_of(container.begin(), container.end(), std::forward<Pred>(pred));
        break;

      case FilterAlgorithm::NONE:
        result = std::none_of(container.begin(), container.end(), std::forward<Pred>(pred));
        break;

      default:
        return;
    }

    if (!result) {
      std::ostringstream msg;
      msg << "Expected " << alg_str << " in container to satisfy predicate, but the condition was false.";
      current_assertion_failures_.push_back({file, name, msg.str(), line});
    }
  }
};

inline std::ostream &operator<<(std::ostream &os, const UnitTester::AssertionFailure_ &failure) {
  os << UnitTester::BOLD_ << failure.file << ":" << failure.line << UnitTester::RESET_ << ": "
    << failure.msg;
  return os;
}

#define TEST(name) \
  static void test_##name(); \
  [[maybe_unused]] static bool registered_##name = UnitTester::register_test(#name, test_##name); \
  static void test_##name()

#define TEST_MAIN() \
  int main (int argc_, char **argv_) { return UnitTester::run_all(argc_, argv_); } \

#define EXPECT_EQUAL(a, b) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_EQUAL("#a", "#b")", (a), (b), "==", std::equal_to<>{}, false)

#define EXPECT_EQUAL_CUSTOM(a, b, comp) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_EQUAL_CUSTOM("#a", "#b", "#comp")", (a), (b), "==", (comp), false)

#define ASSERT_EQUAL(a, b) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "ASSERT_EQUAL("#a", "#b")", (a), (b), "==", std::equal_to<>{}, true)

#define ASSERT_EQUAL_CUSTOM(a, b, comp) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "ASSERT_EQUAL_CUSTOM("#a", "#b", "#comp")", (a), (b), "==", (comp), true)


#define EXPECT_NEAR(a, b) \
 UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_NEAR("#a", "#b")", (a), (b), "~=", UnitTester::NearEqualComparator(), false)

#define EXPECT_NEAR_EPSILON(a, b, e) \
 UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_NEAR_EPSILON("#a", "#b", "#e")", (a), (b), "~=", UnitTester::NearEqualComparator(e), false)


#define EXPECT_NOT_EQUAL(a, b) \
 UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_NOT_EQUAL("#a", "#b")", (a), (b), "!=", std::not_equal_to<>{}, false)

#define EXPECT_NOT_EQUAL_CUSTOM(a, b, comp) \
 UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_NOT_EQUAL_CUSTOM("#a", "#b", "#comp")", (a), (b), "!=", (comp), false)

 #define ASSERT_NOT_EQUAL(a, b) \
 UnitTester::expect_numerical(__FILE__, __LINE__, "ASSERT_NOT_EQUAL("#a", "#b")", (a), (b), "!=", std::not_equal_to<>{}, true)

#define ASSERT_NOT_EQUAL_CUSTOM(a, b, comp) \
 UnitTester::expect_numerical(__FILE__, __LINE__, "ASSERT_NOT_EQUAL_CUSTOM("#a", "#b", "#comp")", (a), (b), "!=", (comp), true)



#define EXPECT_LT(a, b) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_LT("#a", "#b")", (a), (b), "<", std::less<>{}, false)

#define EXPECT_LT_CUSTOM(a, b, comp) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_LT_CUSTOM("#a", "#b", "#comp")", (a), (b), "<", (comp), false)


#define EXPECT_LT_EQUAL(a, b) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_LT_EQUAL("#a", "#b")", (a), (b), "<=", std::less_equal<>{}, false)

#define EXPECT_LT_EQUAL_CUSTOM(a, b, comp) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_LT_EQUAL_CUSTOM("#a", "#b", "#comp")", (a), (b), "<=", (comp), false)


#define EXPECT_GT(a, b) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_GT("#a", "#b")", (a), (b), ">", std::greater<>{}, false)

#define EXPECT_GT_CUSTOM(a, b, comp) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_GT_CUSTOM("#a", "#b", "#comp")", (a), (b), ">", (comp), false)


#define EXPECT_GT_EQUAL(a, b) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_GT_EQUAL("#a", "#b")", (a), (b), ">=", std::greater_equal<>{}, false)

#define EXPECT_GT_EQUAL_CUSTOM(a, b, comp) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_GT_EQUAL_CUSTOM("#a", "#b", "#comp")", (a), (b), ">=", (comp), false)


#define EXPECT_TRUE(a) \
  UnitTester::expect_bool(__FILE__, __LINE__, "EXPECT_TRUE("#a")", (a), true, false)

#define ASSERT_TRUE(a) \
  UnitTester::expect_bool(__FILE__, __LINE__, "ASSERT_TRUE("#a")", (a), true, true)

#define EXPECT_FALSE(a) \
  UnitTester::expect_bool(__FILE__, __LINE__, "EXPECT_FALSE("#a")", (a), false, false)

#define ASSERT_FALSE(a) \
  UnitTester::expect_bool(__FILE__, __LINE__, "ASSERT_FALSE("#a")", (a), false, true)


#define EXPECT_THROWS(func, e) \
  UnitTester::expect_throws<e>(__FILE__, __LINE__, "EXPECT_THROWS("#func", "#e")", (func))

#define EXPECT_NO_THROW(func) \
  UnitTester::expect_no_throw(__FILE__, __LINE__, "EXPECT_NO_THROW("#func")", (func))


#define EXPECT_NULL(ptr) \
  UnitTester::expect_ptr(__FILE__, __LINE__, "EXPECT_NULL("#ptr")", (ptr), true, false)

#define ASSERT_NULL(ptr) \
  UnitTester::expect_ptr(__FILE__, __LINE__, "ASSERT_NULL("#ptr")", (ptr), true, true)

#define EXPECT_NON_NULL(ptr) \
  UnitTester::expect_ptr(__FILE__, __LINE__, "EXPECT_NON_NULL("#ptr")", (ptr), false, false)

#define ASSERT_NON_NULL(ptr) \
  UnitTester::expect_ptr(__FILE__, __LINE__, "ASSERT_NON_NULL("#ptr")", (ptr), false, true)



#define EXPECT_EMPTY(container) \
  UnitTester::expect_size(__FILE__, __LINE__, "EXPECT_EMPTY("#container")", (container), 0)

#define EXPECT_SIZE(container, n) \
  UnitTester::expect_size(__FILE__, __LINE__, "EXPECT_SIZE("#container", "#n")", (container), (n))


#define EXPECT_ALL(container, predicate) \
  UnitTester::expect_filter(__FILE__, __LINE__, "EXPECT_ALL("#container", "#predicate")", "all values", (container), (predicate), UnitTester::FilterAlgorithm::ALL)

#define EXPECT_ANY(container, predicate) \
  UnitTester::expect_filter(__FILE__, __LINE__, "EXPECT_ANY("#container", "#predicate")", ">= 1 values", (container), (predicate), UnitTester::FilterAlgorithm::ANY)

#define EXPECT_NONE(container, predicate) \
  UnitTester::expect_filter(__FILE__, __LINE__, "EXPECT_NONE("#container", "#predicate")", "no values", (container), (predicate), UnitTester::FilterAlgorithm::NONE)

#endif