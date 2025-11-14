#ifndef MQI_TPS_TYPES_HPP
#define MQI_TPS_TYPES_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <moqui/base/mqi_common.hpp>
#include <moqui/base/mqi_vec.hpp>
#include <moqui/base/mqi_ct.hpp>

// GDCM for DICOM directory types
#include "gdcmDirectory.h"

namespace mqi
{

/// Log file data for a single spot
struct logfile_t {
    std::vector<float> posX;
    std::vector<float> posY;
    std::vector<int> muCount;
};

/// Log file data for all beams
struct logfiles_t {
    std::vector<std::vector<float>> beamEnergyInfo;
    std::vector<std::vector<logfile_t>> beamInfo;
};

/// DICOM data structure containing CT, plan, and structure information
struct dicom_t {
    // Voxel dimensions
    mqi::vec3<ijk_t> dim_;       // Number of voxels
    mqi::vec3<ijk_t> org_dim_;   // Original number of voxels

    // Voxel spacing
    float dx = -1;
    float dy = -1;
    float* org_dz = nullptr;
    float* dz = nullptr;

    // File counts
    uint16_t num_vol = 0;
    uint16_t nfiles = 0;
    uint16_t n_plan = 0;
    uint16_t n_dose = 0;
    uint16_t n_struct = 0;

    // Grid edges
    float* xe = nullptr;
    float* ye = nullptr;
    float* ze = nullptr;
    float* org_xe = nullptr;
    float* org_ye = nullptr;
    float* org_ze = nullptr;

    // DICOM file lists
    gdcm::Directory::FilenamesType plan_list;
    gdcm::Directory::FilenamesType dose_list;
    gdcm::Directory::FilenamesType struct_list;
    gdcm::Directory::FilenamesType ct_list;

    // File names
    std::string plan_name = "";
    std::string struct_name = "";
    std::string dose_name = "";

    // CT data
    mqi::ct<phsp_t>* ct = nullptr;

    // Geometry information
    mqi::vec3<float> image_center;
    mqi::vec3<size_t> dose_dim;
    mqi::vec3<float> dose_pos0;
    float dose_dx = -1;
    float dose_dy = -1;
    float* dose_dz = nullptr;

    // Clipping and contour
    mqi::vec3<uint16_t> clip_shift_;
    uint8_t* body_contour = nullptr;

    // RT Plan header information for DICOM export
    std::string sop_class_uid = "";
    std::string sop_instance_uid = "";
    std::string series_date = "";
    std::string content_date = "";
    std::string series_time = "";
    std::string content_time = "";
    std::string institution_name = "";
    std::string referring_physician = "";
    std::string series_description = "";
    std::string patient_name = "";
    std::string patient_id = "";
    std::string patient_birth_date = "";
    std::string patient_sex = "";
    std::string study_instance_uid = "";
    std::string series_instance_uid = "";
    std::string frame_of_reference_uid = "";
    std::string dose_type = "PHYSICAL";
    std::string tissue_heterogeneity_correction = "";
    std::string referenced_rt_plan_sop_instance_uid = "";

    // Constructor
    dicom_t() = default;

    // Destructor
    ~dicom_t() {
        if (org_dz) delete[] org_dz;
        if (dz && dz != org_dz) delete[] dz;
        if (org_xe) delete[] org_xe;
        if (org_ye) delete[] org_ye;
        if (org_ze) delete[] org_ze;
        if (xe && xe != org_xe) delete[] xe;
        if (ye && ye != org_ye) delete[] ye;
        if (ze && ze != org_ze) delete[] ze;
        if (dose_dz) delete[] dose_dz;
        if (body_contour) delete[] body_contour;
    }

    // Prevent copying to avoid double-free
    dicom_t(const dicom_t&) = delete;
    dicom_t& operator=(const dicom_t&) = delete;
};

}   // namespace mqi

#endif // MQI_TPS_TYPES_HPP
