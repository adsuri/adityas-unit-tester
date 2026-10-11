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

  inline static std::vector<AssertionFailure_> current_assertion_failures_;
  friend std::ostream &operator<<(std::ostream &os, AssertionFailure_);

 public:
  struct Test {
    std::string name;
    void (*function)();
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

    bool operator()(double a, double b) {
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
  static void expect_numerical(const std::string &file, int line, const std::string &name, const T& a, const T& b, std::string operation, Comp &&comp) {
    if (!comp(a, b)) {
      std::ostringstream msg;
      msg << std::setprecision(std::numeric_limits<T>::max_digits10) << "Expected " << a << ' '
        << operation << ' ' << b << ", but the condition was false.";
      current_assertion_failures_.push_back({file, name, msg.str(), line});
    }
  }

  static void expect_bool(const std::string &file, int line, const std::string &name, bool a, bool target) {
    if (a != target) {
      std::ostringstream msg;
      msg << "Expected statement to be " << (target ? "true" : "false") << ", but statement was "
        << (a ? "true." : "false.");
      current_assertion_failures_.push_back({file, name, msg.str(), line});
    }
  }

  template <typename ExceptionType, typename FuncType>
  static void expect_throws(const std::string &file, int line, const std::string &name, FuncType &&func) {
    try {
      std::forward<FuncType>(func)();
    } catch (const ExceptionType &e) {
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
      msg << BOLD_ << file << ":" << line << RESET_ << ": " << YELLOW_ << name << RESET_
        << ": Function threw an unexpected exception:\n" << e.what();
      current_assertion_failures_.push_back({file, name, msg.str(), line});
      return;
    } catch (...) {
      std::ostringstream msg;
      msg << "Unexpected non-standard exception.";
      current_assertion_failures_.push_back({file, name, msg.str(), line});
    }
  }

  template <typename Ptr>
  static void expect_ptr(const std::string &file, int line, const std::string &name, const Ptr &ptr, bool expect_null) {
    if ((ptr == nullptr) != expect_null) {
      std::ostringstream msg;
      msg << "Expected pointer to be " << (expect_null ? "null" : "non-null")
        << ", but the condition was false.";
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

  template <typename Algorithm, typename Container, typename Pred>
  static void expect_filter(const std::string &file, int line, const std::string &name, std::string alg_str, const Container &container, Pred &&pred, Algorithm alg) {
    if (!alg(container.begin(), container.end(), std::forward<Pred>(pred))) {
      std::ostringstream msg;
      msg << "Expected " << alg_str << " in container to satisfy predicate, but the condition was false.";
      current_assertion_failures_.push_back({file, name, msg.str(), line});
    }
  }
};

std::ostream &operator<<(std::ostream &os, UnitTester::AssertionFailure_ failure) {
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
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_EQUAL("#a", "#b")", (a), (b), "==", std::equal_to<>{})

#define EXPECT_EQUAL_CUSTOM(a, b, comp) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_EQUAL_CUSTOM("#a", "#b", "#comp")", (a), (b), "==", (comp))


#define EXPECT_NEAR(a, b) \
 UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_NEAR("#a", "#b")", (a), (b), "~=", UnitTester::NearEqualComparator())

#define EXPECT_NEAR_EPSILON(a, b, e) \
 UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_NEAR_EPSILON("#a", "#b", "#e")", (a), (b), "~=", UnitTester::NearEqualComparator(e))


#define EXPECT_NOT_EQUAL(a, b) \
 UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_NOT_EQUAL("#a", "#b")", (a), (b), "!=", std::not_equal_to<>{})

#define EXPECT_NOT_EQUAL_CUSTOM(a, b, comp) \
 UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_NOT_EQUAL_CUSOM("#a", "#b", "#comp")", (a), (b), "!=", (comp))


#define EXPECT_LT(a, b) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_LT("#a", "#b")", (a), (b), "<", std::less<>{})

#define EXPECT_LT_CUSTOM(a, b, comp) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_LT_CUSTOM("#a", "#b", "#comp")", (a), (b), "<", (comp))


#define EXPECT_LT_EQUAL(a, b) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_LT_EQUAL("#a", "#b")", (a), (b), "<=", std::less_equal<>{})

#define EXPECT_LT_EQUAL_CUSTOM(a, b, comp) \
  UnitTester::expect(__FILE__, __LINE__, "EXPECT_LT_EQUAL_CUSTOM("#a", "#b", "#comp")", (a), (b), "<=", (comp))


#define EXPECT_GT(a, b) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_GT("#a", "#b")", (a), (b), ">", std::greater<>{})

#define EXPECT_GT_CUSTOM(a, b, comp) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_GT_CUSTOM("#a", "#b", "#comp")", (a), (b), ">", (comp))


#define EXPECT_GT_EQUAL(a, b) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_GT_EQUAL("#a", "#b")", (a), (b), ">=", std::greater_equal<>{})

#define EXPECT_GT_EQUAL_CUSTOM(a, b, comp) \
  UnitTester::expect_numerical(__FILE__, __LINE__, "EXPECT_GT_EQUAL_CUSTOM("#a", "#b", "#comp")", (a), (b), ">=", (comp))


#define EXPECT_TRUE(a) \
  UnitTester::expect_bool(__FILE__, __LINE__, "EXPECT_TRUE("#a")", (a), true)

#define EXPECT_FALSE(a) \
  UnitTester::expect_bool(__FILE__, __LINE__, "EXPECT_FALSE("#a")", (a), false)


#define EXPECT_THROWS(func, e) \
  UnitTester::expect_throws<e>(__FILE__, __LINE__, "EXPECT_THROWS("#func", "#e")", (func))

#define EXPECT_NO_THROW(func) \
  UnitTester::expect_no_throw(__FILE__, __LINE__, "EXPECT_NO_THROW("#func")", (func))


#define EXPECT_NULL(ptr) \
  UnitTester::expect_ptr(__FILE__, __LINE__, "EXPECT_NULL("#ptr")", (ptr), true)

#define EXPECT_NON_NULL(ptr) \
  UnitTester::expect_ptr(__FILE__, __LINE__, "EXPECT_NON_NULL("#ptr")", (ptr), false)


#define EXPECT_EMPTY(container) \
  UnitTester::expect_size(__FILE__, __LINE__, "EXPECT_EMPTY("#container")", (container), 0)

#define EXPECT_SIZE(container, n) \
  UnitTester::expect_size(__FILE__, __LINE__, "EXPECT_SIZE("#container", "#n")", (container), (n))


#define EXPECT_ALL(container, predicate) \
  UnitTester::expect_filter(__FILE__, __LINE__, "EXPECT_ALL("#container", "#predicate")", "all values", (container), (predicate), std::all_of)

#define EXPECT_ANY(container, predicate) \
  UnitTester::expect_filter(__FILE__, __LINE__, "EXPECT_ANY("#container", "#predicate")", "all values", (container), (predicate), std::any_of)

#define EXPECT_NONE(container, predicate) \
  UnitTester::expect_filter(__FILE__, __LINE__, "EXPECT_NONE("#container", "#predicate")", "all values", (container), (predicate), std::none_of)

#endif