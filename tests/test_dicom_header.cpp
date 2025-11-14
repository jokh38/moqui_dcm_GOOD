#include "test_framework.hpp"
#include "../base/io/mqi_dicom_header.hpp"

using namespace mqi::io;

// Test 1: Default construction
TEST(DicomHeaderInfo_DefaultConstruction) {
    dcm_header_info header;

    ASSERT_TRUE(header.patient_name.empty());
    ASSERT_TRUE(header.patient_id.empty());
    ASSERT_TRUE(header.study_instance_uid.empty());
}

// Test 2: Setting and getting values
TEST(DicomHeaderInfo_SetAndGet) {
    dcm_header_info header;

    header.patient_name = "Test Patient";
    header.patient_id = "12345";
    header.institution_name = "Test Hospital";

    ASSERT_EQ(header.patient_name, std::string("Test Patient"));
    ASSERT_EQ(header.patient_id, std::string("12345"));
    ASSERT_EQ(header.institution_name, std::string("Test Hospital"));
}

// Test 3: Copy construction
TEST(DicomHeaderInfo_CopyConstruction) {
    dcm_header_info header1;
    header1.patient_name = "John Doe";
    header1.patient_id = "P001";

    dcm_header_info header2 = header1;

    ASSERT_EQ(header2.patient_name, std::string("John Doe"));
    ASSERT_EQ(header2.patient_id, std::string("P001"));
}

// Test 4: Validate required fields helper
TEST(DicomHeaderInfo_ValidateRequiredFields) {
    dcm_header_info header;

    // Empty header should have missing required fields
    ASSERT_FALSE(validate_required_fields(header));

    // Fill minimum required fields
    header.patient_name = "Test";
    header.patient_id = "001";
    header.study_instance_uid = "1.2.3";
    header.series_instance_uid = "1.2.4";
    header.frame_of_reference_uid = "1.2.5";

    ASSERT_TRUE(validate_required_fields(header));
}

// Test 5: Create header from dicom_t
TEST(DicomHeaderInfo_CreateFromDicomT) {
    // This tests the helper function that will create dcm_header_info
    // from the dicom_t structure in mqi_tps_env
    // For now, just test the structure exists
    dcm_header_info header;
    header.dose_type = "PHYSICAL";
    ASSERT_EQ(header.dose_type, std::string("PHYSICAL"));
}

int main() {
    return mqi_test::TestRunner::instance().run_all();
}
