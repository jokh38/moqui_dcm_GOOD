#ifndef TEST_FRAMEWORK_HPP
#define TEST_FRAMEWORK_HPP

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <sstream>

namespace mqi_test {

struct TestResult {
    std::string name;
    bool passed;
    std::string message;
};

class TestRunner {
public:
    static TestRunner& instance() {
        static TestRunner runner;
        return runner;
    }

    void add_test(const std::string& name, std::function<void()> test_fn) {
        tests_.push_back({name, test_fn});
    }

    int run_all() {
        std::cout << "\n=== Running Tests ===\n" << std::endl;
        int passed = 0;
        int failed = 0;

        for (const auto& test : tests_) {
            std::cout << "Running: " << test.name << " ... ";
            try {
                test.fn();
                std::cout << "\033[32mPASSED\033[0m" << std::endl;
                passed++;
            } catch (const std::exception& e) {
                std::cout << "\033[31mFAILED\033[0m" << std::endl;
                std::cout << "  Error: " << e.what() << std::endl;
                failed++;
            }
        }

        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Total:  " << (passed + failed) << std::endl;

        return failed;
    }

private:
    struct Test {
        std::string name;
        std::function<void()> fn;
    };

    std::vector<Test> tests_;
};

// Helper macros
#define TEST(test_name) \
    void test_##test_name(); \
    namespace { \
        struct TestRegistrar_##test_name { \
            TestRegistrar_##test_name() { \
                mqi_test::TestRunner::instance().add_test(#test_name, test_##test_name); \
            } \
        }; \
        static TestRegistrar_##test_name registrar_##test_name; \
    } \
    void test_##test_name()

#define ASSERT_EQ(a, b) \
    do { \
        auto val_a = (a); \
        auto val_b = (b); \
        if (val_a != val_b) { \
            std::ostringstream oss; \
            oss << "Assertion failed: " << #a << " == " << #b \
                << " (expected: " << val_b << ", got: " << val_a << ")"; \
            throw std::runtime_error(oss.str()); \
        } \
    } while(0)

#define ASSERT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            throw std::runtime_error("Assertion failed: " #cond); \
        } \
    } while(0)

#define ASSERT_FALSE(cond) \
    do { \
        if ((cond)) { \
            throw std::runtime_error("Assertion failed: !" #cond); \
        } \
    } while(0)

#define ASSERT_NEAR(a, b, epsilon) \
    do { \
        auto val_a = (a); \
        auto val_b = (b); \
        if (std::abs(val_a - val_b) > (epsilon)) { \
            std::ostringstream oss; \
            oss << "Assertion failed: " << #a << " ~= " << #b \
                << " (expected: " << val_b << ", got: " << val_a << ")"; \
            throw std::runtime_error(oss.str()); \
        } \
    } while(0)

} // namespace mqi_test

#endif // TEST_FRAMEWORK_HPP
