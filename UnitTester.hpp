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

class UnitTester {
 private:
  inline static std::string current_assertion_failures_;
  inline static const std::string RED = "\033[31m";
  inline static const std::string GREEN = "\033[32m";
  inline static const std::string YELLOW = "\033[33m";
  inline static const std::string BLUE = "\033[34m";
  inline static const std::string BOLD = "\033[1m";
  inline static const std::string RESET = "\033[0m";

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

    NearEqualComparator(int epsilon_)
      : epsilon(epsilon_) {}

    bool operator()(double a, double b) {
      return std::abs(a - b) <= epsilon;
    }
  };

  static bool register_test(const char* name, void (*function)()) {
    all_tests.push_back({name, function});
    return true;
  }

  static void run_all(int argc_, char **argv_) {
    argc = argc_;
    argv= argv_;

    size_t successes = 0;

    for (const auto& test : all_tests) {
      std::cout << "Running test: " << BLUE << test.name << RESET << '\n';
      try {
        test.function();
      } catch (std::exception &e) {
        std::cout << RED << "Unhandled exception happened within test " << test.name << RESET << std::endl
          << e.what() << std::endl << std::endl;
        continue;
      }

      if (current_assertion_failures_ != "") {
        std::cout << current_assertion_failures_;
        std::cout << "Test " << test.name << " failed." << std::endl << std::endl;
      } else {
        std::cout << "Test " << test.name << " passed!" << std::endl << std::endl;
        ++successes;
      }

      current_assertion_failures_ = "";
    }

    size_t n = all_tests.size();
    std::cout << "Finished " << n << " test(s). " << successes << "/" << n
      << " tests were successful." << std::endl;
  }

  template <typename Comp, typename T>
  static void expect(const std::string &file, int line, const std::string &name, const T& a, const T& b, std::string operation, Comp &&comp) {
    if (!comp(a, b)) {
      std::ostringstream msg;
      msg << std::setprecision(std::numeric_limits<T>::max_digits10) << BOLD << file << ":" << line << RESET << ": " << YELLOW << name << RESET
      << ": Expected " << a << ' ' << operation << ' ' << b << ", but the condition was false. \n";
      current_assertion_failures_ += msg.str();
    }
  }

  static void expect_bool(const std::string &file, int line, const std::string &name, bool a, bool target) {
    if (a != target) {
      std::ostringstream msg;
      msg << BOLD << file << ":" << line << RESET << ": " << YELLOW << name << RESET << "\033[0m: Expected statement to be " << (target ? "true" : "false")
        << ", but statement was " << (a ? "true" : "false") << " \n";
      current_assertion_failures_ += msg.str();
    }
  }
};

#define TEST(name) \
  static void test_##name(); \
  [[maybe_unused]] static bool registered_##name = UnitTester::register_test(#name, test_##name); \
  static void test_##name()

#define TEST_MAIN() \
  int main (int argc_, char **argv_) { UnitTester::run_all(argc_, argv_); } \

#define EXPECT_EQUAL(a, b) \
  UnitTester::expect(__FILE__, __LINE__, "EXPECT_EQUAL("#a", "#b")", (a), (b), "==", std::equal_to<>{})

#define EXPECT_EQUAL_CUSTOM(a, b, Comp) \
  UnitTester::expect(__FILE__, __LINE__, "EXPECT_EQUAL_CUSTOM("#a", "#b", "#Comp")", (a), (b), "==", (Comp))


#define EXPECT_NEAR(a, b) \
 UnitTester::expect(__FILE__, __LINE__, "EXPECT_NEAR("#a", "#b")", (a), (b), "~=", UnitTester::NearEqualComparator())

#define EXPECT_NEAR_EPSILON(a, b, e) \
 UnitTester::expect(__FILE__, __LINE__, "EXPECT_NEAR_EPSILON("#a", "#b", "#e")", (a), (b), "~=", UnitTester::NearEqualComparator(e))


#define EXPECT_NOT_EQUAL(a, b) \
 UnitTester::expect(__FILE__, __LINE__, "EXPECT_EQUAL("#a", "#b")", (a), (b), "==", std::not_equal_to<>{})

#define EXPECT_NOT_EQUAL_CUSTOM(a, b, Comp) \
 UnitTester::expect(__FILE__, __LINE__, "EXPECT_EQUAL("#a", "#b", "#Comp")", (a), (b), "!=", (Comp))


#define EXPECT_LT(a, b) \
  UnitTester::expect(__FILE__, __LINE__, "EXPECT_LT("#a", "#b")", (a), (b), "<", std::less<>{})

#define EXPECT_LT_CUSTOM(a, b, Comp) \
  UnitTester::expect(__FILE__, __LINE__, "EXPECT_LT_CUSTOM("#a", "#b", "#Comp")", (a), (b), "<", (Comp))


#define EXPECT_LT_EQUAL(a, b) \
  UnitTester::expect(__FILE__, __LINE__, "EXPECT_LT_EQUAL("#a", "#b")", (a), (b), "<=", std::less_equal<>{})

#define EXPECT_LT_EQUAL_CUSTOM(a, b, Comp) \
  UnitTester::expect(__FILE__, __LINE__, "EXPECT_LT_EQUAL_CUSTOM("#a", "#b", "#Comp")", (a), (b), "<=", (Comp))


#define EXPECT_GT(a, b) \
  UnitTester::expect(__FILE__, __LINE__, "EXPECT_GT("#a", "#b")", (a), (b), ">", std::greater<>{})

#define EXPECT_GT_CUSTOM(a, b, Comp) \
  UnitTester::expect(__FILE__, __LINE__, "EXPECT_GT_CUSTOM("#a", "#b", "#Comp")", (a), (b), ">", (Comp))


#define EXPECT_GT_EQUAL(a, b) \
  UnitTester::expect(__FILE__, __LINE__, "EXPECT_GT_EQUAL("#a", "#b")", (a), (b), ">=", std::greater_equal<>{})

#define EXPECT_GT_EQUAL_CUSTOM(a, b, Comp) \
  UnitTester::expect(__FILE__, __LINE__, "EXPECT_GT_EQUAL_CUSTOM("#a", "#b", "#Comp")", (a), (b), ">"=, (Comp))

#define EXPECT_TRUE(a) \
  UnitTester::expect_bool(__FILE__, __LINE__, "EXPECT_TRUE("#a")", (a), true)

#define EXPECT_FALSE(a) \
  UnitTester::expect_bool(__FILE__, __LINE__, "EXPECT_FALSE("#a")", (a), false)

#endif