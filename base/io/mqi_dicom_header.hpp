#ifndef MQI_DICOM_HEADER_HPP
#define MQI_DICOM_HEADER_HPP

#include <string>

namespace mqi
{
namespace io
{

/// Structure to hold DICOM header information from RT Plan
struct dcm_header_info {
    // Patient Information
    std::string patient_name;
    std::string patient_id;
    std::string patient_birth_date;
    std::string patient_sex;

    // Study/Series Information
    std::string study_instance_uid;
    std::string series_instance_uid;
    std::string frame_of_reference_uid;
    std::string series_date;
    std::string content_date;
    std::string series_time;
    std::string content_time;

    // Institution Information
    std::string institution_name;
    std::string referring_physician;
    std::string series_description;

    // Dose-specific Information
    std::string dose_type;
    std::string tissue_heterogeneity_correction;
    std::string referenced_rt_plan_sop_instance_uid;

    // Default constructor
    dcm_header_info() : dose_type("PHYSICAL") {}
};

/// Validate that required DICOM fields are present
inline bool validate_required_fields(const dcm_header_info& header) {
    return !header.patient_name.empty() &&
           !header.patient_id.empty() &&
           !header.study_instance_uid.empty() &&
           !header.series_instance_uid.empty() &&
           !header.frame_of_reference_uid.empty();
}

}   // namespace io
}   // namespace mqi

#endif // MQI_DICOM_HEADER_HPP
