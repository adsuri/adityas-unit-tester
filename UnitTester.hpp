#ifndef UNIT_TESTER_HPP
#define UNIT_TESTER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <sstream>

class UnitTester {
 private:
  inline static std::string current_expect_failures_;

 public:
  struct TestType {
    std::string name;
    void (*function)();
  };

  inline static int argc = 0;
  inline static char** argv = nullptr;

  inline static double epsilon = 0.00001;

  inline static std::vector<TestType> all_tests{};

  static bool register_test(const char* name, void (*function)()) {
    all_tests.push_back({name, function});
    return true;
  }

  static void run_all(int argc_, char **argv_) {
    argc = argc_;
    argv= argv_;

    for (const auto& test : all_tests) {
      std::cout << "Running: " << test.name << '\n';
      try {
        test.function();
      } catch (std::exception &e) {
        std::cout << "Unhandled exception happened within test " << test.name << ":" << std::endl
          << e.what() << std::endl;
        break;
      }

      current_expect_failures_ = "";
    }

    std::cout << "Finished " << all_tests.size()
      << " tests.\n";
  }

  template <typename Comp, typename T>
  static void expect(const std::string &name, const T& a, const T& b, std::string operation, Comp &comp) {
    if (!comp(a, b)) {
      std::ostringstream msg;
      msg << name << ": Expected " << a << ' ' << operation << ' ' << b
        << ", but the condition was false." << std::endl;

      current_expect_failures_ += msg.str();
      current_expect_failures_ += '\n';
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
  UnitTester::expect("EXPECT_EQUAL("#a", "#b")", (a), (b), "==", std::equal_to<>{})

#define EXPECT_EQUAL_CUSTOM(a, b, Comp) \
  UnitTester::expect("EXPECT_EQUAL_CUSTOM("#a", "#b")", (a), (b), "==", (Comp))


#define EXPECT_LT(a, b) \
  UnitTester::expect("EXPECT_LT("#a", "#b")", (a), (b), "<", std::less<>{})

#define EXPECT_LT_CUSTOM(a, b, Comp) \
  UnitTester::expect("EXPECT_LT_CUSTOM("#a", "#b")", (a), (b), "<", (Comp))


#define EXPECT_LT_EQUAL(a, b) \
  UnitTester::expect("EXPECT_LT_EQUAL("#a", "#b")", (a), (b), "<=", std::less_equal<>{})

#define EXPECT_LT_EQUAL_CUSTOM(a, b, Comp) \
  UnitTester::expect("EXPECT_LT_EQUAL_CUSTOM("#a", "#b")", (a), (b), "<=", (Comp))


#define EXPECT_GT(a, b) \
  UnitTester::expect("EXPECT_GT("#a", "#b")", (a), (b), ">", std::greater<>{})

#define EXPECT_GT_CUSTOM(a, b, Comp) \
  UnitTester::expect("EXPECT_GT_CUSTOM("#a", "#b")", (a), (b), ">", (Comp))


#define EXPECT_GT_EQUAL(a, b) \
  UnitTester::expect("EXPECT_GT_EQUAL("#a", "#b")", (a), (b), ">", std::greater<>{})

#define EXPECT_GT_EQUAL_CUSTOM(a, b, Comp) \
  UnitTester::expect("EXPECT_GT_EQUAL_CUSTOM("#a", "#b")", (a), (b), ">", (Comp))

#endif