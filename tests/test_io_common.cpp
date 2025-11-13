#include "test_framework.hpp"
#include "../base/io/mqi_io_common.hpp"
#include "../base/mqi_common.hpp"
#include <ctime>

using namespace mqi::io;

// Test 1: Date generation
TEST(IOCommon_GenerateCurrentDate) {
    std::string date = get_current_date();

    // Format should be YYYYMMDD (8 characters)
    ASSERT_EQ(date.length(), 8);

    // Should be numeric
    for (char c : date) {
        ASSERT_TRUE(c >= '0' && c <= '9');
    }
}

// Test 2: Time generation
TEST(IOCommon_GenerateCurrentTime) {
    std::string time = get_current_time();

    // Format should be HHMMSS (6 characters)
    ASSERT_EQ(time.length(), 6);

    // Should be numeric
    for (char c : time) {
        ASSERT_TRUE(c >= '0' && c <= '9');
    }
}

// Test 3: UID generation
TEST(IOCommon_GenerateUID) {
    std::string uid1 = generate_uid();
    std::string uid2 = generate_uid();

    // UIDs should not be empty
    ASSERT_TRUE(!uid1.empty());
    ASSERT_TRUE(!uid2.empty());

    // UIDs should be different
    ASSERT_TRUE(uid1 != uid2);
}

// Test 4: File path concatenation
TEST(IOCommon_BuildFilePath) {
    std::string path = build_file_path("/test/path", "file", "txt");
    ASSERT_EQ(path, std::string("/test/path/file.txt"));

    std::string path2 = build_file_path("/test/path/", "file", "bin");
    ASSERT_EQ(path2, std::string("/test/path/file.bin"));
}

// Test 5: Extract sparse indices
TEST(IOCommon_ExtractSparseIndices) {
    // Mock data: simple key-value pairs
    std::vector<std::pair<uint32_t, double>> test_data = {
        {0, 1.5}, {5, 2.3}, {10, 3.7}, {15, 0.0}, {20, 4.2}
    };

    auto result = extract_nonzero_indices(test_data);

    // Should have 4 non-zero entries (index 15 has value 0.0)
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result[0].first, 0);
    ASSERT_EQ(result[1].first, 5);
    ASSERT_EQ(result[2].first, 10);
    ASSERT_EQ(result[3].first, 20);
}

// Test 6: Apply scaling
TEST(IOCommon_ApplyScaling) {
    std::vector<double> data = {1.0, 2.0, 3.0, 4.0};
    double scale = 2.5;

    apply_scaling(data, scale);

    ASSERT_NEAR(data[0], 2.5, 1e-6);
    ASSERT_NEAR(data[1], 5.0, 1e-6);
    ASSERT_NEAR(data[2], 7.5, 1e-6);
    ASSERT_NEAR(data[3], 10.0, 1e-6);
}

// Test 7: Metadata structure
TEST(IOCommon_FileMetadata) {
    FileMetadata meta;
    meta.dimensions = {100, 100, 50};
    meta.spacing = {1.0f, 1.0f, 2.0f};
    meta.origin = {-50.0f, -50.0f, -100.0f};

    ASSERT_EQ(meta.dimensions.x, 100);
    ASSERT_EQ(meta.dimensions.y, 100);
    ASSERT_EQ(meta.dimensions.z, 50);
    ASSERT_NEAR(meta.spacing.z, 2.0f, 1e-6);
}

int main() {
    return mqi_test::TestRunner::instance().run_all();
}
