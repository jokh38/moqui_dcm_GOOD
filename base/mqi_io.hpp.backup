#ifndef MQI_IO_HPP
#define MQI_IO_HPP

#include <algorithm>
#include <complex>
#include <cstdint>
#include <iomanip>   // std::setprecision
#include <iostream>
#include <numeric>   //accumulate
#include <sstream>   // std::ostringstream
#include <string>    // std::to_string
#include <valarray>
#include <vector>    // std::vector
#include <zlib.h>

#include <sys/mman.h>   //for io

// GDCM headers for DICOM RT Dose support
#include "gdcmAttribute.h"
#include "gdcmDataElement.h"
#include "gdcmFile.h"
#include "gdcmImage.h"
#include "gdcmImageWriter.h"
#include "gdcmMediaStorage.h"
#include "gdcmPhotometricInterpretation.h"
#include "gdcmPixelFormat.h"
#include "gdcmTag.h"
#include "gdcmTransferSyntax.h"
#include "gdcmUIDGenerator.h"
#include "gdcmWriter.h"

#include <moqui/base/mqi_common.hpp>
#include <moqui/base/mqi_hash_table.hpp>
#include <moqui/base/mqi_roi.hpp>
#include <moqui/base/mqi_sparse_io.hpp>
#include <moqui/base/mqi_scorer.hpp>

namespace mqi
{
namespace io
{
/// Structure to hold DICOM header information from RT Plan
struct dcm_header_info {
    std::string patient_name;
    std::string patient_id;
    std::string patient_birth_date;
    std::string patient_sex;
    std::string study_instance_uid;
    std::string series_instance_uid;
    std::string frame_of_reference_uid;
    std::string series_date;
    std::string content_date;
    std::string series_time;
    std::string content_time;
    std::string institution_name;
    std::string referring_physician;
    std::string series_description;
    std::string dose_type;
    std::string tissue_heterogeneity_correction;
    std::string referenced_rt_plan_sop_instance_uid;
};

/// Save dose data as DICOM RT Dose file (from scorer directly with full header info)
template<typename R>
void
save_to_dcm(const mqi::scorer<R>*        src,
            const mqi::node_t<R>*        geometry_node,
            const dcm_header_info*       header_info,
            const R                      scale,
            const std::string&           filepath,
            const std::string&           filename,
            const uint32_t               length,
            const mqi::vec3<mqi::ijk_t>& dim,
            const bool                   is_2cm_mode = false);
///<  save scorer data to a file in binary format
///<  scr: scorer pointer
///<  scale: data will be multiplied by
///<  dir  : directory path. file name will be dir + scr->name + ".bin"
///<  reshape: roi is used in scorer, original size will be defined.
template<typename R>
void
save_to_bin(const mqi::scorer<R>* src,
            const R               scale,
            const std::string&    filepath,
            const std::string&    filename);

template<typename R>
void
save_to_bin(const R*           src,
            const R            scale,
            const std::string& filepath,
            const std::string& filename,
            const uint32_t     length);

template<typename R>
void
save_to_npz(const mqi::scorer<R>* src,
            const R               scale,
            const std::string&    filepath,
            const std::string&    filename,
            mqi::vec3<mqi::ijk_t> dim,
            uint32_t              num_spots);

template<typename R>
void
save_to_npz2(const mqi::scorer<R>* src,
             const R               scale,
             const std::string&    filepath,
             const std::string&    filename,
             mqi::vec3<mqi::ijk_t> dim,
             uint32_t              num_spots);

template<typename R>
void
save_to_npz(const mqi::scorer<R>* src,
            const R               scale,
            const std::string&    filepath,
            const std::string&    filename,
            mqi::vec3<mqi::ijk_t> dim,
            uint32_t              num_spots,
            R*                    time_scale,
            R                     threshold);

template<typename R>
void
save_to_bin(const mqi::key_value* src,
            const R               scale,
            uint32_t              max_capacity,
            const std::string&    filepath,
            const std::string&    filename);

template<typename R>
void
save_to_mhd(const mqi::node_t<R>* children,
            const double*         src,
            const R               scale,
            const std::string&    filepath,
            const std::string&    filename,
            const uint32_t        length);

template<typename R>
void
save_to_mha(const mqi::node_t<R>* children,
            const double*         src,
            const R               scale,
            const std::string&    filepath,
            const std::string&    filename,
            const uint32_t        length);
}   // namespace io
}   // namespace mqi

///< Function to write key values into file
///< src: array and this array is copied
///<
template<typename R>
void
mqi::io::save_to_bin(const mqi::scorer<R>* src,
                     const R               scale,
                     const std::string&    filepath,
                     const std::string&    filename) {
    /// create a copy using valarray and apply scale

    unsigned int            nnz = 0;
    std::vector<mqi::key_t> key1;
    std::vector<mqi::key_t> key2;
    std::vector<double>     value;
    key1.clear();
    key2.clear();
    value.clear();
    for (int ind = 0; ind < src->max_capacity_; ind++) {
        if (src->data_[ind].key1 != mqi::empty_pair && src->data_[ind].key2 != mqi::empty_pair &&
            src->data_[ind].value > 0) {
            key1.push_back(src->data_[ind].key1);
            key2.push_back(src->data_[ind].key2);
            value.push_back(src->data_[ind].value * scale);
        }
    }

    printf("length %lu %lu %lu\n", key1.size(), key2.size(), value.size());

    /// open out stream
    std::ofstream fid_key1(filepath + "/" + filename + "_key1.raw",
                           std::ios::out | std::ios::binary);
    if (!fid_key1)
        std::cout << "Cannot write :" << filepath + "/" + filename + "_key1.raw" << std::endl;

    /// write to a file
    fid_key1.write(reinterpret_cast<const char*>(&key1.data()[0]),
                   key1.size() * sizeof(mqi::key_t));
    fid_key1.close();

    std::ofstream fid_key2(filepath + "/" + filename + "_key2.raw",
                           std::ios::out | std::ios::binary);
    if (!fid_key2)
        std::cout << "Cannot write :" << filepath + "/" + filename + "_key2.raw" << std::endl;

    /// write to a file
    fid_key2.write(reinterpret_cast<const char*>(&key2.data()[0]),
                   key2.size() * sizeof(mqi::key_t));
    fid_key2.close();

    std::ofstream fid_bin(filepath + "/" + filename + "_value.raw",
                          std::ios::out | std::ios::binary);
    if (!fid_bin)
        std::cout << "Cannot write :" << filepath + "/" + filename + "_value.raw" << std::endl;

    /// write to a file
    fid_bin.write(reinterpret_cast<const char*>(&value.data()[0]), value.size() * sizeof(double));
    fid_bin.close();
}

///< Function to write array into file
///< src: array and this array is copied
///<
template<typename R>
void
mqi::io::save_to_bin(const R*           src,
                     const R            scale,
                     const std::string& filepath,
                     const std::string& filename,
                     const uint32_t     length) {
    /// create a copy using valarray and apply scale
    std::valarray<R> dest(src, length);
    munmap(&dest, length * sizeof(R));
    dest *= scale;
    /// open out stream
    std::ofstream fid_bin(filepath + "/" + filename + ".raw", std::ios::out | std::ios::binary);
    if (!fid_bin) std::cout << "Cannot write :" << filepath + "/" + filename + ".raw" << std::endl;

    /// write to a file
    fid_bin.write(reinterpret_cast<const char*>(&dest[0]), length * sizeof(R));
    fid_bin.close();
}

///< Function to write key values into file
///< src: array and this array is copied
///<
template<typename R>
void
mqi::io::save_to_bin(const mqi::key_value* src,
                     const R               scale,
                     uint32_t              max_capacity,
                     const std::string&    filepath,
                     const std::string&    filename) {
    /// create a copy using valarray and apply scale

    unsigned int            nnz = 0;
    std::vector<mqi::key_t> key1;
    std::vector<mqi::key_t> key2;
    std::vector<R>          value;
    key1.clear();
    key2.clear();
    value.clear();
    for (int ind = 0; ind < max_capacity; ind++) {
        if (src[ind].key1 != mqi::empty_pair && src[ind].key2 != mqi::empty_pair &&
            src[ind].value > 0) {
            key1.push_back(src[ind].key1);
            key2.push_back(src[ind].key2);
            value.push_back(src[ind].value * scale);
        }
    }

    printf("length %lu %lu %lu\n", key1.size(), key2.size(), value.size());
    /// open out stream
    std::ofstream fid_key1(filepath + "/" + filename + "_key1.raw",
                           std::ios::out | std::ios::binary);
    if (!fid_key1)
        std::cout << "Cannot write :" << filepath + "/" + filename + "_key1.raw" << std::endl;

    /// write to a file
    fid_key1.write(reinterpret_cast<const char*>(&key1.data()[0]),
                   key1.size() * sizeof(mqi::key_t));
    fid_key1.close();

    std::ofstream fid_key2(filepath + "/" + filename + "_key2.raw",
                           std::ios::out | std::ios::binary);
    if (!fid_key2)
        std::cout << "Cannot write :" << filepath + "/" + filename + "_key2.raw" << std::endl;

    /// write to a file
    fid_key2.write(reinterpret_cast<const char*>(&key2.data()[0]),
                   key2.size() * sizeof(mqi::key_t));
    fid_key2.close();

    std::ofstream fid_bin(filepath + "/" + filename + "_value.raw",
                          std::ios::out | std::ios::binary);
    if (!fid_bin)
        std::cout << "Cannot write :" << filepath + "/" + filename + "_value.raw" << std::endl;

    /// write to a file
    fid_bin.write(reinterpret_cast<const char*>(&value.data()[0]), value.size() * sizeof(R));
    fid_bin.close();
}

///< Function to write key values into file
///< src: array and this array is copied
///<

template<typename R>
void
mqi::io::save_to_npz(const mqi::scorer<R>* src,
                     const R               scale,
                     const std::string&    filepath,
                     const std::string&    filename,
                     mqi::vec3<mqi::ijk_t> dim,
                     uint32_t              num_spots) {
    uint32_t vol_size;
    vol_size = dim.x * dim.y * dim.z;

    /// create a copy using valarray and apply scale
    const std::string name_a = "indices.npy", name_b = "indptr.npy", name_c = "shape.npy",
                      name_d = "data.npy", name_e = "format.npy";
    std::vector<double>* value_vec = new std::vector<double>[num_spots];
    std::vector<mqi::key_t>*          vox_vec = new std::vector<mqi::key_t>[num_spots];
    std::vector<double>               data_vec;
    std::vector<uint32_t>             indices_vec;
    std::vector<uint32_t>             indptr_vec;
    mqi::key_t                        vox_ind, spot_ind;
    double                            value;
    int                               spot_start = 0, spot_end = 0;
    int                               vox_in_spot[num_spots];
    std::vector<double>::iterator     it_data;
    std::vector<uint32_t>::iterator   it_ind;
    std::vector<mqi::key_t>::iterator it_spot;
    int                               vox_count;
    printf("save_to_npz\n");

    printf("scan start %d\n", src->max_capacity_);
    for (int ind = 0; ind < src->max_capacity_; ind++) {
        if (src->data_[ind].key1 != mqi::empty_pair && src->data_[ind].key2 != mqi::empty_pair) {
            vox_count = 0;
            vox_ind   = src->data_[ind].key1;
            spot_ind  = src->data_[ind].key2;
            assert(vox_ind >= 0 && vox_ind < vol_size);
            value = src->data_[ind].value;
            value_vec[spot_ind].push_back(value * scale);
            vox_vec[spot_ind].push_back(vox_ind);
        }
    }

    vox_count = 0;
    indptr_vec.push_back(vox_count);
    for (int ii = 0; ii < num_spots; ii++) {
        data_vec.insert(data_vec.end(), value_vec[ii].begin(), value_vec[ii].end());
        indices_vec.insert(indices_vec.end(), vox_vec[ii].begin(), vox_vec[ii].end());
        vox_count += vox_vec[ii].size();
        indptr_vec.push_back(vox_count);
    }

    printf("scan done %lu %lu %lu\n", data_vec.size(), indices_vec.size(), indptr_vec.size());
    printf("%d %d\n", vol_size, num_spots);

    uint32_t    shape[2] = { num_spots, vol_size };
    std::string format   = "csr";
    size_t      size_a = indices_vec.size(), size_b = indptr_vec.size(), size_c = 2,
           size_d = data_vec.size(), size_e = 3;

    uint32_t* indices = new uint32_t[indices_vec.size()];
    uint32_t* indptr  = new uint32_t[indptr_vec.size()];
    double*   data    = new double[data_vec.size()];
    std::copy(indices_vec.begin(), indices_vec.end(), indices);
    std::copy(indptr_vec.begin(), indptr_vec.end(), indptr);
    std::copy(data_vec.begin(), data_vec.end(), data);
    printf("%lu\n", size_b);
    mqi::io::save_npz(filepath + "/" + filename + ".npz", name_a, indices, size_a, "w");
    mqi::io::save_npz(filepath + "/" + filename + ".npz", name_b, indptr, size_b, "a");
    mqi::io::save_npz(filepath + "/" + filename + ".npz", name_c, shape, size_c, "a");
    mqi::io::save_npz(filepath + "/" + filename + ".npz", name_d, data, size_d, "a");
    mqi::io::save_npz(filepath + "/" + filename + ".npz", name_e, format, size_e, "a");
}

template<typename R>
void
mqi::io::save_to_npz2(const mqi::scorer<R>* src,
                      const R               scale,
                      const std::string&    filepath,
                      const std::string&    filename,
                      mqi::vec3<mqi::ijk_t> dim,
                      uint32_t              num_spots) {
    uint32_t vol_size;
    vol_size = src->roi_->get_mask_size();
    /// create a copy using valarray and apply scale
    const std::string name_a = "indices.npy", name_b = "indptr.npy", name_c = "shape.npy",
                      name_d = "data.npy", name_e = "format.npy";

    std::vector<double>*              value_vec = new std::vector<double>[vol_size];
    std::vector<mqi::key_t>*          spot_vec  = new std::vector<mqi::key_t>[vol_size];
    std::vector<double>               data_vec;
    std::vector<uint32_t>             indices_vec;
    std::vector<uint32_t>             indptr_vec;
    mqi::key_t                        vox_ind, spot_ind;
    double                            value;
    int                               spot_start = 0, spot_end = 0;
    std::vector<double>::iterator     it_data;
    std::vector<uint32_t>::iterator   it_ind;
    std::vector<mqi::key_t>::iterator it_spot;
    int                               spot_count;
    printf("save_to_npz\n");

    printf("scan start %d\n", src->max_capacity_);
    for (int ind = 0; ind < src->max_capacity_; ind++) {
        if (src->data_[ind].key1 != mqi::empty_pair && src->data_[ind].key2 != mqi::empty_pair) {
            vox_ind = src->data_[ind].key1;
            vox_ind = src->roi_->get_mask_idx(vox_ind);
            if (vox_ind < 0) {
                printf("is this right?\n");
                continue;
            }
            spot_ind = src->data_[ind].key2;
            assert(vox_ind >= 0 && vox_ind < vol_size);
            value = src->data_[ind].value;
            assert(value > 0);
            value_vec[vox_ind].push_back(value * scale);
            spot_vec[vox_ind].push_back(spot_ind);
        }
    }
    printf("Sorting start\n");
    for (int ind = 0; ind < vol_size; ind++) {
        if (spot_vec[ind].size() > 1) {
            std::vector<int> sort_ind(spot_vec[ind].size());
            std::iota(sort_ind.begin(), sort_ind.end(), 0);
            sort(sort_ind.begin(), sort_ind.end(), [&](int i, int j) {
                return spot_vec[ind][i] < spot_vec[ind][j];
            });
            std::vector<double>     sorted_value(spot_vec[ind].size());
            std::vector<mqi::key_t> sorted_spot(spot_vec[ind].size());
            for (int sorted_ind = 0; sorted_ind < spot_vec[ind].size(); sorted_ind++) {
                sorted_value[sorted_ind] = value_vec[ind][sort_ind[sorted_ind]];
                sorted_spot[sorted_ind]  = spot_vec[ind][sort_ind[sorted_ind]];
            }
            spot_vec[ind]  = sorted_spot;
            value_vec[ind] = sorted_value;
        }
    }

    spot_count = 0;
    indptr_vec.push_back(spot_count);
    for (int ii = 0; ii < vol_size; ii++) {
        data_vec.insert(data_vec.end(), value_vec[ii].begin(), value_vec[ii].end());
        indices_vec.insert(indices_vec.end(), spot_vec[ii].begin(), spot_vec[ii].end());
        spot_count += spot_vec[ii].size();
        indptr_vec.push_back(spot_count);
    }

    uint32_t    shape[2] = { vol_size, num_spots };
    std::string format   = "csr";
    size_t      size_a = indices_vec.size(), size_b = indptr_vec.size(), size_c = 2,
           size_d = data_vec.size(), size_e = 3;

    uint32_t* indices = new uint32_t[indices_vec.size()];
    uint32_t* indptr  = new uint32_t[indptr_vec.size()];
    double*   data    = new double[data_vec.size()];
    std::copy(indices_vec.begin(), indices_vec.end(), indices);
    std::copy(indptr_vec.begin(), indptr_vec.end(), indptr);
    std::copy(data_vec.begin(), data_vec.end(), data);
    printf("%lu\n", size_b);
    mqi::io::save_npz(filepath + "/" + filename + ".npz", name_a, indices, size_a, "w");
    mqi::io::save_npz(filepath + "/" + filename + ".npz", name_b, indptr, size_b, "a");
    mqi::io::save_npz(filepath + "/" + filename + ".npz", name_c, shape, size_c, "a");
    mqi::io::save_npz(filepath + "/" + filename + ".npz", name_d, data, size_d, "a");
    mqi::io::save_npz(filepath + "/" + filename + ".npz", name_e, format, size_e, "a");
}

template<typename R>
void
mqi::io::save_to_npz(const mqi::scorer<R>* src,
                     const R               scale,
                     const std::string&    filepath,
                     const std::string&    filename,
                     mqi::vec3<mqi::ijk_t> dim,
                     uint32_t              num_spots,
                     R*                    time_scale,
                     R                     threshold) {
    uint32_t vol_size;
    vol_size = dim.x * dim.y * dim.z;
    /// create a copy using valarray and apply scale
    const std::string name_a = "indices.npy", name_b = "indptr.npy", name_c = "shape.npy",
                      name_d = "data.npy", name_e = "format.npy";
    std::vector<double>               value_vec[num_spots];
    std::vector<mqi::key_t>           vox_vec[num_spots];
    std::vector<double>               data_vec;
    std::vector<uint32_t>             indices_vec;
    std::vector<uint32_t>             indptr_vec;
    mqi::key_t                        vox_ind, spot_ind;
    double                            value;
    int                               spot_start = 0, spot_end = 0;
    int                               vox_in_spot[num_spots];
    std::vector<double>::iterator     it_data;
    std::vector<uint32_t>::iterator   it_ind;
    std::vector<mqi::key_t>::iterator it_spot;
    int                               vox_count;
    printf("save_to_npz\n");
    for (int ind = 0; ind < num_spots; ind++) {
        vox_in_spot[ind] = 0;
    }
    printf("scan start %d\n", src->max_capacity_);
    for (int ind = 0; ind < src->max_capacity_; ind++) {
        if (src->data_[ind].key1 != mqi::empty_pair && src->data_[ind].key2 != mqi::empty_pair) {
            vox_count = 0;
            vox_ind   = src->data_[ind].key1;
            spot_ind  = src->data_[ind].key2;
            assert(vox_ind >= 0 && vox_ind < vol_size);
            value = src->data_[ind].value;
            value *= scale;
            value -= 2 * threshold;
            if (value < 0) value = 0;
            value /= time_scale[spot_ind];
            value_vec[spot_ind].push_back(value);
            vox_vec[spot_ind].push_back(vox_ind);
        }
    }

    vox_count = 0;
    indptr_vec.push_back(vox_count);
    for (int ii = 0; ii < num_spots; ii++) {
        data_vec.insert(data_vec.end(), value_vec[ii].begin(), value_vec[ii].end());
        indices_vec.insert(indices_vec.end(), vox_vec[ii].begin(), vox_vec[ii].end());
        vox_count += vox_vec[ii].size();
        indptr_vec.push_back(vox_count);
    }
    printf("scan done %lu %lu %lu\n", data_vec.size(), indices_vec.size(), indptr_vec.size());
    printf("%d %d\n", vol_size, num_spots);

    uint32_t    shape[2] = { num_spots, vol_size };
    std::string format   = "csr";
    size_t      size_a = indices_vec.size(), size_b = indptr_vec.size(), size_c = 2,
           size_d = data_vec.size(), size_e = 3;

    uint32_t* indices = new uint32_t[indices_vec.size()];
    uint32_t* indptr  = new uint32_t[indptr_vec.size()];
    double*   data    = new double[data_vec.size()];
    std::copy(indices_vec.begin(), indices_vec.end(), indices);
    std::copy(indptr_vec.begin(), indptr_vec.end(), indptr);
    std::copy(data_vec.begin(), data_vec.end(), data);
    printf("%lu\n", size_b);
    mqi::io::save_npz(filepath + "/" + filename + ".npz", name_a, indices, size_a, "w");
    mqi::io::save_npz(filepath + "/" + filename + ".npz", name_b, indptr, size_b, "a");
    mqi::io::save_npz(filepath + "/" + filename + ".npz", name_c, shape, size_c, "a");
    mqi::io::save_npz(filepath + "/" + filename + ".npz", name_d, data, size_d, "a");
    mqi::io::save_npz(filepath + "/" + filename + ".npz", name_e, format, size_e, "a");
}

template<typename R>
void
mqi::io::save_to_mhd(const mqi::node_t<R>* children,
                     const double*         src,
                     const R               scale,
                     const std::string&    filepath,
                     const std::string&    filename,
                     const uint32_t        length) {
    ///< TODO: this works only for two depth world
    ///< TODO: dx, dy, and dz calculation works only for AABB
    float dx = children->geo[0].get_x_edges()[1];
    dx -= children->geo[0].get_x_edges()[0];
    float dy = children->geo[0].get_y_edges()[1];
    dy -= children->geo[0].get_y_edges()[0];
    float dz = children->geo[0].get_z_edges()[1];
    dz -= children->geo[0].get_z_edges()[0];
    float x0 = children->geo[0].get_x_edges()[0];
    x0 += children->geo[0].get_x_edges()[0];
    x0 /= 2.0;
    float y0 = children->geo[0].get_y_edges()[0];
    y0 += children->geo[0].get_y_edges()[0];
    y0 /= 2.0;
    float z0 = children->geo[0].get_z_edges()[0];
    z0 += children->geo[0].get_z_edges()[0];
    z0 /= 2.0;
    std::ofstream fid_header(filepath + "/" + filename + ".mhd", std::ios::out);
    if (!fid_header) { std::cout << "Cannot open file!" << std::endl; }
    fid_header << "ObjectType = Image\n";
    fid_header << "NDims = 3\n";
    fid_header << "BinaryData = True\n";
    fid_header
      << "BinaryDataByteOrderMSB = False\n";   // True for big endian, False for little endian
    fid_header << "CompressedData = False\n";
    fid_header << "TransformMatrix 1 0 0 0 1 0 0 0 1\n";
    fid_header << "Offset " << x0 << " " << y0 << " " << z0 << std::endl;
    fid_header << "CenterOfRotation 0 0 0\n";
    fid_header << "AnatomicOrientation = RAI\n";
    fid_header << "DimSize = " << children->geo[0].get_nxyz().x << " "
               << children->geo[0].get_nxyz().y << " " << children->geo[0].get_nxyz().z << "\n";
    ///< TODO: if R is double, MET_FLOAT should be MET_DOUBLE
    fid_header << "ElementType = MET_DOUBLE\n";

    fid_header << "ElementSpacing = " << dx << " " << dy << " " << dz << "\n";
    fid_header << "ElementDataFile = " << filename << ".raw"
               << "\n";
    fid_header.close();
    if (!fid_header.good()) { std::cout << "Error occurred at writing time!" << std::endl; }
    std::valarray<double> dest(src, length);
    munmap(&dest, length * sizeof(double));
    dest *= scale;
    std::ofstream fid_raw(filepath + "/" + filename + ".raw", std::ios::out | std::ios::binary);
    if (!fid_raw) { std::cout << "Cannot open file!" << std::endl; }
    fid_raw.write(reinterpret_cast<const char*>(&dest[0]), length * sizeof(double));

    fid_raw.close();
    if (!fid_raw.good()) { std::cout << "Error occurred at writing time!" << std::endl; }
}

template<typename R>
void
mqi::io::save_to_mha(const mqi::node_t<R>* children,
                     const double*         src,
                     const R               scale,
                     const std::string&    filepath,
                     const std::string&    filename,
                     const uint32_t        length) {
    ///< TODO: this works only for two depth world
    ///< TODO: dx, dy, and dz calculation works only for AABB
    float dx = children->geo[0].get_x_edges()[1];
    dx -= children->geo[0].get_x_edges()[0];
    float dy = children->geo[0].get_y_edges()[1];
    dy -= children->geo[0].get_y_edges()[0];
    float dz = children->geo[0].get_z_edges()[1];
    dz -= children->geo[0].get_z_edges()[0];
    float x0 = children->geo[0].get_x_edges()[0] + dx * 0.5;
    float y0 = children->geo[0].get_y_edges()[0] + dy * 0.5;
    float z0 = children->geo[0].get_z_edges()[0] + dz * 0.5;
    std::cout << "x0 " << std::setprecision(9) << x0 << " y0 " << y0 << " z0 " << z0 << std::endl;
    std::valarray<double> dest(src, length);
    munmap(&dest, length * sizeof(double));
    dest *= scale;
    std::ofstream fid_header(filepath + "/" + filename + ".mha", std::ios::out);
    if (!fid_header) { std::cout << "Cannot open file!" << std::endl; }
    fid_header << "ObjectType = Image\n";
    fid_header << "NDims = 3\n";
    fid_header << "BinaryData = True\n";
    fid_header
      << "BinaryDataByteOrderMSB = False\n";   // True for big endian, False for little endian
    fid_header << "CompressedData = False\n";
    fid_header << "TransformMatrix = 1 0 0 0 1 0 0 0 1\n";
    fid_header << "Origin = " << std::setprecision(9) << x0 << " " << y0 << " " << z0 << "\n";
    fid_header << "CenterOfRotation = 0 0 0\n";
    fid_header << "AnatomicOrientation = RAI\n";
    fid_header << "DimSize = " << children->geo[0].get_nxyz().x << " "
               << children->geo[0].get_nxyz().y << " " << children->geo[0].get_nxyz().z << "\n";
    ///< TODO: if R is double, MET_FLOAT should be MET_DOUBLE
    fid_header << "ElementType = MET_DOUBLE\n";
    fid_header << "HeaderSize = -1\n";
    fid_header << "ElementSpacing = " << std::setprecision(9) << dx << " " << dy << " " << dz
               << "\n";
    fid_header << "ElementDataFile = LOCAL\n";
    fid_header.write(reinterpret_cast<const char*>(&dest[0]), length * sizeof(double));
    fid_header.close();
    if (!fid_header.good()) { std::cout << "Error occurred at writing time!" << std::endl; }
}

// Helper functions for DICOM RT Dose export
static std::string generate_uid() {
    gdcm::UIDGenerator uid_gen;
    return uid_gen.Generate();
}

static std::string get_current_date() {
    time_t now = time(0);
    struct tm tstruct = *localtime(&now);
    char buf[9];
    strftime(buf, sizeof(buf), "%Y%m%d", &tstruct);
    return buf;
}

static std::string get_current_time() {
    time_t now = time(0);
    struct tm tstruct = *localtime(&now);
    char buf[7];
    strftime(buf, sizeof(buf), "%H%M%S", &tstruct);
    return buf;
}

// Save dose data from scorer directly with full DICOM header support
template<typename R>
void
mqi::io::save_to_dcm(const mqi::scorer<R>*        src,
                      const mqi::node_t<R>*        geometry_node,
                      const dcm_header_info*       header_info,
                      const R                      scale,
                      const std::string&           filepath,
                      const std::string&           filename,
                      const uint32_t               /*length*/,
                      const mqi::vec3<mqi::ijk_t>& dim,
                      const bool                   is_2cm_mode)
{
    // Phase 1: Extract dose data from scorer
    const size_t actual_size = static_cast<size_t>(dim.x) * dim.y * dim.z;
    std::vector<double> dose_data(actual_size, 0.0);

    for (int ind = 0; ind < src->max_capacity_; ind++) {
        if (src->data_[ind].key1 != mqi::empty_pair &&
            src->data_[ind].key2 != mqi::empty_pair &&
            src->data_[ind].value > 0) {

            mqi::key_t key = src->data_[ind].key1;
            if (key < actual_size) {
                dose_data[key] += src->data_[ind].value * scale;
            }
        }
    }

    // Calculate max dose
    double max_dose = 0.0;
    for (const auto& dose : dose_data) {
        if (dose > max_dose) max_dose = dose;
    }

    // Phase 2: Convert to 16-bit pixel data
    double dose_grid_scaling = (max_dose > 0) ? (max_dose / 65535.0) : 1.0;
    std::vector<uint16_t> pixel_data(actual_size);

    for (size_t i = 0; i < actual_size; i++) {
        double scaled = dose_data[i] / dose_grid_scaling;
        pixel_data[i] = (scaled > 65535.0) ? 65535 : static_cast<uint16_t>(scaled);
    }

    // Phase 3: Extract geometry information from geometry_node
    double dx = geometry_node->geo->get_x_edges()[1] - geometry_node->geo->get_x_edges()[0];
    double dy = geometry_node->geo->get_y_edges()[1] - geometry_node->geo->get_y_edges()[0];
    double dz = geometry_node->geo->get_z_edges()[1] - geometry_node->geo->get_z_edges()[0];

    // Phase 4: Create DICOM file with GDCM
    try {
        gdcm::UIDGenerator uid_gen;
        std::string sop_uid = uid_gen.Generate();
        std::string study_uid = (header_info && !header_info->study_instance_uid.empty())
                                 ? header_info->study_instance_uid : uid_gen.Generate();
        std::string series_uid = uid_gen.Generate();  // Always generate new series UID for dose

        // Create Image object (GDCM manages ownership)
        gdcm::Image* image = new gdcm::Image();
        gdcm::PixelFormat pf(gdcm::PixelFormat::UINT16);
        pf.SetBitsAllocated(16);
        pf.SetBitsStored(16);
        pf.SetHighBit(15);
        pf.SetSamplesPerPixel(1);
        image->SetPixelFormat(pf);
        image->SetPhotometricInterpretation(gdcm::PhotometricInterpretation::MONOCHROME2);
        image->SetDimension(0, static_cast<unsigned int>(dim.x));
        image->SetDimension(1, static_cast<unsigned int>(dim.y));
        image->SetDimension(2, static_cast<unsigned int>(dim.z));

        // Create File object (GDCM manages ownership)
        gdcm::File* file = new gdcm::File();
        gdcm::DataSet& ds = file->GetDataSet();

        // ========== SOP Common Module ==========
        const char* rt_dose_class = "1.2.840.10008.5.1.4.1.1.481.2";
        gdcm::DataElement de_sop_class(gdcm::Tag(0x0008, 0x0016));
        de_sop_class.SetVR(gdcm::VR::UI);
        de_sop_class.SetByteValue(rt_dose_class, strlen(rt_dose_class));
        ds.Insert(de_sop_class);

        gdcm::DataElement de_sop_inst(gdcm::Tag(0x0008, 0x0018));
        de_sop_inst.SetVR(gdcm::VR::UI);
        de_sop_inst.SetByteValue(sop_uid.c_str(), sop_uid.length());
        ds.Insert(de_sop_inst);

        // Date/Time
        std::string creation_date = (header_info && !header_info->content_date.empty())
                                     ? header_info->content_date : get_current_date();
        gdcm::DataElement de_creation_date(gdcm::Tag(0x0008, 0x0012));
        de_creation_date.SetVR(gdcm::VR::DA);
        de_creation_date.SetByteValue(creation_date.c_str(), creation_date.length());
        ds.Insert(de_creation_date);

        std::string creation_time = (header_info && !header_info->content_time.empty())
                                     ? header_info->content_time : get_current_time();
        gdcm::DataElement de_creation_time(gdcm::Tag(0x0008, 0x0013));
        de_creation_time.SetVR(gdcm::VR::TM);
        de_creation_time.SetByteValue(creation_time.c_str(), creation_time.length());
        ds.Insert(de_creation_time);

        // ========== Patient Module ==========
        std::string patient_name = (header_info && !header_info->patient_name.empty())
                                    ? header_info->patient_name : "PHANTOM";
        gdcm::DataElement de_patient_name(gdcm::Tag(0x0010, 0x0010));
        de_patient_name.SetVR(gdcm::VR::PN);
        de_patient_name.SetByteValue(patient_name.c_str(), patient_name.length());
        ds.Insert(de_patient_name);

        std::string patient_id = (header_info && !header_info->patient_id.empty())
                                  ? header_info->patient_id : "QA_PHANTOM";
        gdcm::DataElement de_patient_id(gdcm::Tag(0x0010, 0x0020));
        de_patient_id.SetVR(gdcm::VR::LO);
        de_patient_id.SetByteValue(patient_id.c_str(), patient_id.length());
        ds.Insert(de_patient_id);

        if (header_info && !header_info->patient_birth_date.empty()) {
            gdcm::DataElement de_patient_birth_date(gdcm::Tag(0x0010, 0x0030));
            de_patient_birth_date.SetVR(gdcm::VR::DA);
            de_patient_birth_date.SetByteValue(header_info->patient_birth_date.c_str(),
                                                header_info->patient_birth_date.length());
            ds.Insert(de_patient_birth_date);
        }

        if (header_info && !header_info->patient_sex.empty()) {
            gdcm::DataElement de_patient_sex(gdcm::Tag(0x0010, 0x0040));
            de_patient_sex.SetVR(gdcm::VR::CS);
            de_patient_sex.SetByteValue(header_info->patient_sex.c_str(),
                                         header_info->patient_sex.length());
            ds.Insert(de_patient_sex);
        }

        // ========== General Study Module ==========
        gdcm::DataElement de_study_uid(gdcm::Tag(0x0020, 0x000D));
        de_study_uid.SetVR(gdcm::VR::UI);
        de_study_uid.SetByteValue(study_uid.c_str(), study_uid.length());
        ds.Insert(de_study_uid);

        std::string study_date = (header_info && !header_info->series_date.empty())
                                  ? header_info->series_date : creation_date;
        gdcm::DataElement de_study_date(gdcm::Tag(0x0008, 0x0020));
        de_study_date.SetVR(gdcm::VR::DA);
        de_study_date.SetByteValue(study_date.c_str(), study_date.length());
        ds.Insert(de_study_date);

        std::string study_time = (header_info && !header_info->series_time.empty())
                                  ? header_info->series_time : creation_time;
        gdcm::DataElement de_study_time(gdcm::Tag(0x0008, 0x0030));
        de_study_time.SetVR(gdcm::VR::TM);
        de_study_time.SetByteValue(study_time.c_str(), study_time.length());
        ds.Insert(de_study_time);

        gdcm::DataElement de_study_id(gdcm::Tag(0x0020, 0x0010));
        de_study_id.SetVR(gdcm::VR::SH);
        de_study_id.SetByteValue("1", 1);
        ds.Insert(de_study_id);

        if (header_info && !header_info->referring_physician.empty()) {
            gdcm::DataElement de_referring_physician(gdcm::Tag(0x0008, 0x0090));
            de_referring_physician.SetVR(gdcm::VR::PN);
            de_referring_physician.SetByteValue(header_info->referring_physician.c_str(),
                                                 header_info->referring_physician.length());
            ds.Insert(de_referring_physician);
        }

        // ========== RT Series Module ==========
        gdcm::DataElement de_modality(gdcm::Tag(0x0008, 0x0060));
        de_modality.SetVR(gdcm::VR::CS);
        de_modality.SetByteValue("RTDOSE", strlen("RTDOSE"));
        ds.Insert(de_modality);

        gdcm::DataElement de_series_uid(gdcm::Tag(0x0020, 0x000E));
        de_series_uid.SetVR(gdcm::VR::UI);
        de_series_uid.SetByteValue(series_uid.c_str(), series_uid.length());
        ds.Insert(de_series_uid);

        gdcm::DataElement de_series_num(gdcm::Tag(0x0020, 0x0011));
        de_series_num.SetVR(gdcm::VR::IS);
        de_series_num.SetByteValue("1", 1);
        ds.Insert(de_series_num);

        // Series Date/Time from RT Plan
        if (header_info && !header_info->series_date.empty()) {
            gdcm::DataElement de_series_date(gdcm::Tag(0x0008, 0x0021));
            de_series_date.SetVR(gdcm::VR::DA);
            de_series_date.SetByteValue(header_info->series_date.c_str(),
                                         header_info->series_date.length());
            ds.Insert(de_series_date);
        }

        if (header_info && !header_info->series_time.empty()) {
            gdcm::DataElement de_series_time(gdcm::Tag(0x0008, 0x0031));
            de_series_time.SetVR(gdcm::VR::TM);
            de_series_time.SetByteValue(header_info->series_time.c_str(),
                                         header_info->series_time.length());
            ds.Insert(de_series_time);
        }

        // Content Date/Time
        if (header_info && !header_info->content_date.empty()) {
            gdcm::DataElement de_content_date(gdcm::Tag(0x0008, 0x0023));
            de_content_date.SetVR(gdcm::VR::DA);
            de_content_date.SetByteValue(header_info->content_date.c_str(),
                                          header_info->content_date.length());
            ds.Insert(de_content_date);
        }

        if (header_info && !header_info->content_time.empty()) {
            gdcm::DataElement de_content_time(gdcm::Tag(0x0008, 0x0033));
            de_content_time.SetVR(gdcm::VR::TM);
            de_content_time.SetByteValue(header_info->content_time.c_str(),
                                          header_info->content_time.length());
            ds.Insert(de_content_time);
        }

        if (header_info && !header_info->series_description.empty()) {
            gdcm::DataElement de_series_desc(gdcm::Tag(0x0008, 0x103E));
            de_series_desc.SetVR(gdcm::VR::LO);
            de_series_desc.SetByteValue(header_info->series_description.c_str(),
                                         header_info->series_description.length());
            ds.Insert(de_series_desc);
        }

        // ========== Frame of Reference Module ==========
        std::string frame_uid = (header_info && !header_info->frame_of_reference_uid.empty())
                                 ? header_info->frame_of_reference_uid : uid_gen.Generate();
        gdcm::DataElement de_frame_uid(gdcm::Tag(0x0020, 0x0052));
        de_frame_uid.SetVR(gdcm::VR::UI);
        de_frame_uid.SetByteValue(frame_uid.c_str(), frame_uid.length());
        ds.Insert(de_frame_uid);

        // ========== General Equipment Module ==========
        std::string institution_name = (header_info && !header_info->institution_name.empty())
                                        ? header_info->institution_name : "MOQUI_SMC";
        gdcm::DataElement de_institution(gdcm::Tag(0x0008, 0x0080));
        de_institution.SetVR(gdcm::VR::LO);
        de_institution.SetByteValue(institution_name.c_str(), institution_name.length());
        ds.Insert(de_institution);

        const char* manufacturer = "MOQUI_SMC";
        gdcm::DataElement de_manufacturer(gdcm::Tag(0x0008, 0x0070));
        de_manufacturer.SetVR(gdcm::VR::LO);
        de_manufacturer.SetByteValue(manufacturer, strlen(manufacturer));
        ds.Insert(de_manufacturer);

        const char* model_name = "MOQUI_SMC";
        gdcm::DataElement de_model_name(gdcm::Tag(0x0008, 0x1090));
        de_model_name.SetVR(gdcm::VR::LO);
        de_model_name.SetByteValue(model_name, strlen(model_name));
        ds.Insert(de_model_name);

        gdcm::DataElement de_software_ver(gdcm::Tag(0x0018, 0x1020));
        de_software_ver.SetVR(gdcm::VR::LO);
        de_software_ver.SetByteValue("1.0", 3);
        ds.Insert(de_software_ver);

        // ========== Image Plane Module ==========
        uint16_t rows_val = static_cast<uint16_t>(dim.y);
        gdcm::DataElement de_rows(gdcm::Tag(0x0028, 0x0010));
        de_rows.SetVR(gdcm::VR::US);
        de_rows.SetByteValue(reinterpret_cast<const char*>(&rows_val), sizeof(uint16_t));
        ds.Insert(de_rows);

        uint16_t cols_val = static_cast<uint16_t>(dim.x);
        gdcm::DataElement de_cols(gdcm::Tag(0x0028, 0x0011));
        de_cols.SetVR(gdcm::VR::US);
        de_cols.SetByteValue(reinterpret_cast<const char*>(&cols_val), sizeof(uint16_t));
        ds.Insert(de_cols);

        std::string frames_str = std::to_string(dim.z);
        gdcm::DataElement de_frames(gdcm::Tag(0x0028, 0x0008));
        de_frames.SetVR(gdcm::VR::IS);
        de_frames.SetByteValue(frames_str.c_str(), frames_str.length());
        ds.Insert(de_frames);

        uint16_t samples_val = 1;
        gdcm::DataElement de_samples(gdcm::Tag(0x0028, 0x0002));
        de_samples.SetVR(gdcm::VR::US);
        de_samples.SetByteValue(reinterpret_cast<const char*>(&samples_val), sizeof(uint16_t));
        ds.Insert(de_samples);

        gdcm::DataElement de_pi(gdcm::Tag(0x0028, 0x0004));
        de_pi.SetVR(gdcm::VR::CS);
        de_pi.SetByteValue("MONOCHROME2", strlen("MONOCHROME2"));
        ds.Insert(de_pi);

        uint16_t bits_alloc = 16;
        gdcm::DataElement de_bits_alloc(gdcm::Tag(0x0028, 0x0100));
        de_bits_alloc.SetVR(gdcm::VR::US);
        de_bits_alloc.SetByteValue(reinterpret_cast<const char*>(&bits_alloc), sizeof(uint16_t));
        ds.Insert(de_bits_alloc);

        uint16_t bits_stored = 16;
        gdcm::DataElement de_bits_stored(gdcm::Tag(0x0028, 0x0101));
        de_bits_stored.SetVR(gdcm::VR::US);
        de_bits_stored.SetByteValue(reinterpret_cast<const char*>(&bits_stored), sizeof(uint16_t));
        ds.Insert(de_bits_stored);

        uint16_t high_bit = 15;
        gdcm::DataElement de_high_bit(gdcm::Tag(0x0028, 0x0102));
        de_high_bit.SetVR(gdcm::VR::US);
        de_high_bit.SetByteValue(reinterpret_cast<const char*>(&high_bit), sizeof(uint16_t));
        ds.Insert(de_high_bit);

        uint16_t pixel_rep = 0;
        gdcm::DataElement de_pixel_rep(gdcm::Tag(0x0028, 0x0103));
        de_pixel_rep.SetVR(gdcm::VR::US);
        de_pixel_rep.SetByteValue(reinterpret_cast<const char*>(&pixel_rep), sizeof(uint16_t));
        ds.Insert(de_pixel_rep);

        // ========== RT Dose Module ==========
        gdcm::DataElement de_dose_units(gdcm::Tag(0x3004, 0x0002));
        de_dose_units.SetVR(gdcm::VR::CS);
        de_dose_units.SetByteValue("GY", strlen("GY"));
        ds.Insert(de_dose_units);

        std::string dose_type = (header_info && !header_info->dose_type.empty())
                                 ? header_info->dose_type : "PHYSICAL";
        gdcm::DataElement de_dose_type(gdcm::Tag(0x3004, 0x0004));
        de_dose_type.SetVR(gdcm::VR::CS);
        de_dose_type.SetByteValue(dose_type.c_str(), dose_type.length());
        ds.Insert(de_dose_type);

        gdcm::DataElement de_dose_sum(gdcm::Tag(0x3004, 0x000a));
        de_dose_sum.SetVR(gdcm::VR::CS);
        de_dose_sum.SetByteValue("PLAN", strlen("PLAN"));
        ds.Insert(de_dose_sum);

        std::ostringstream dgs_ss;
        dgs_ss << std::scientific << std::setprecision(8) << dose_grid_scaling;
        std::string dgs_str = dgs_ss.str();
        gdcm::DataElement de_dose_grid(gdcm::Tag(0x3004, 0x000e));
        de_dose_grid.SetVR(gdcm::VR::DS);
        de_dose_grid.SetByteValue(dgs_str.c_str(), dgs_str.length());
        ds.Insert(de_dose_grid);

        if (header_info && !header_info->tissue_heterogeneity_correction.empty()) {
            gdcm::DataElement de_tissue_hetero(gdcm::Tag(0x3004, 0x0014));
            de_tissue_hetero.SetVR(gdcm::VR::CS);
            de_tissue_hetero.SetByteValue(header_info->tissue_heterogeneity_correction.c_str(),
                                           header_info->tissue_heterogeneity_correction.length());
            ds.Insert(de_tissue_hetero);
        }

        // ========== Spatial Attributes ==========
        // Pixel Spacing (Row spacing \ Column spacing)
        std::stringstream ps_ss;
        ps_ss << std::fixed << std::setprecision(6) << dy << "\\" << dx;
        std::string ps_str = ps_ss.str();
        gdcm::DataElement de_pixel_spacing(gdcm::Tag(0x0028, 0x0030));
        de_pixel_spacing.SetVR(gdcm::VR::DS);
        de_pixel_spacing.SetByteValue(ps_str.c_str(), ps_str.length());
        ds.Insert(de_pixel_spacing);

        // Grid Frame Offset Vector
        std::stringstream gf_ss;
        for (int i = 0; i < dim.z; i++) {
            if (i > 0) gf_ss << "\\";
            double z_pos = geometry_node->geo->get_z_edges()[i] - geometry_node->geo->get_z_edges()[0];
            gf_ss << std::fixed << std::setprecision(6) << z_pos;
        }
        std::string gf_str = gf_ss.str();
        gdcm::DataElement de_grid_offset(gdcm::Tag(0x3004, 0x000c));
        de_grid_offset.SetVR(gdcm::VR::DS);
        de_grid_offset.SetByteValue(gf_str.c_str(), gf_str.length());
        ds.Insert(de_grid_offset);

        // Image Position Patient
        std::stringstream ip_ss;
        if (is_2cm_mode) {
            // Special positioning for 2cm mode
            ip_ss << std::fixed << std::setprecision(1) << "-199.5" << "\\" << "20.5" << "\\" << "-200.5";
        } else {
            ip_ss << std::fixed << std::setprecision(6)
                  << (geometry_node->geo->get_x_edges()[0] - dx/2) << "\\"
                  << (geometry_node->geo->get_y_edges()[0] - dy/2) << "\\"
                  << (geometry_node->geo->get_z_edges()[0] - dz/2);
        }
        std::string ip_str = ip_ss.str();
        gdcm::DataElement de_img_pos(gdcm::Tag(0x0020, 0x0032));
        de_img_pos.SetVR(gdcm::VR::DS);
        de_img_pos.SetByteValue(ip_str.c_str(), ip_str.length());
        ds.Insert(de_img_pos);

        // Image Orientation Patient
        std::string orientation_str;
        if (is_2cm_mode) {
            // Special orientation for 2cm mode
            orientation_str = "1\\0\\0\\0\\0\\1";
        } else {
            orientation_str = "1\\0\\0\\0\\1\\0";
        }
        gdcm::DataElement de_img_orient(gdcm::Tag(0x0020, 0x0037));
        de_img_orient.SetVR(gdcm::VR::DS);
        de_img_orient.SetByteValue(orientation_str.c_str(), orientation_str.length());
        ds.Insert(de_img_orient);

        // ========== Referenced RT Plan Sequence ==========
        if (header_info && !header_info->referenced_rt_plan_sop_instance_uid.empty()) {
            // Create sequence item for Referenced RT Plan
            gdcm::SmartPointer<gdcm::SequenceOfItems> ref_rt_plan_seq = new gdcm::SequenceOfItems();
            ref_rt_plan_seq->SetLengthToUndefined();

            gdcm::Item item;
            item.SetVLToUndefined();
            gdcm::DataSet& item_ds = item.GetNestedDataSet();

            // Referenced SOP Class UID (RT Plan Storage)
            const char* rt_plan_class = "1.2.840.10008.5.1.4.1.1.481.5";
            gdcm::DataElement de_ref_sop_class(gdcm::Tag(0x0008, 0x1150));
            de_ref_sop_class.SetVR(gdcm::VR::UI);
            de_ref_sop_class.SetByteValue(rt_plan_class, strlen(rt_plan_class));
            item_ds.Insert(de_ref_sop_class);

            // Referenced SOP Instance UID
            gdcm::DataElement de_ref_sop_inst(gdcm::Tag(0x0008, 0x1155));
            de_ref_sop_inst.SetVR(gdcm::VR::UI);
            de_ref_sop_inst.SetByteValue(header_info->referenced_rt_plan_sop_instance_uid.c_str(),
                                          header_info->referenced_rt_plan_sop_instance_uid.length());
            item_ds.Insert(de_ref_sop_inst);

            ref_rt_plan_seq->AddItem(item);

            gdcm::DataElement de_ref_rt_plan_seq(gdcm::Tag(0x300C, 0x0002));
            de_ref_rt_plan_seq.SetVR(gdcm::VR::SQ);
            de_ref_rt_plan_seq.SetValue(*ref_rt_plan_seq);
            de_ref_rt_plan_seq.SetVLToUndefined();
            ds.Insert(de_ref_rt_plan_seq);
        }

        // ========== Pixel Data ==========
        size_t pd_size = pixel_data.size() * sizeof(uint16_t);
        gdcm::DataElement de_pixel_data(gdcm::Tag(0x7FE0, 0x0010));
        de_pixel_data.SetVR(gdcm::VR::OW);
        de_pixel_data.SetByteValue(reinterpret_cast<const char*>(pixel_data.data()),
                                    static_cast<unsigned int>(pd_size));
        ds.Insert(de_pixel_data);

        // ========== File Meta Information ==========
        gdcm::TransferSyntax ts = gdcm::TransferSyntax::ExplicitVRLittleEndian;
        file->GetHeader().SetDataSetTransferSyntax(ts);

        // File Meta Information Version
        uint8_t version[2] = {0x00, 0x01};
        gdcm::DataElement de_version(gdcm::Tag(0x0002, 0x0001));
        de_version.SetVR(gdcm::VR::OB);
        de_version.SetByteValue(reinterpret_cast<const char*>(version), 2);
        file->GetHeader().Insert(de_version);

        // Media Storage SOP Class UID
        gdcm::DataElement de_media_class(gdcm::Tag(0x0002, 0x0002));
        de_media_class.SetVR(gdcm::VR::UI);
        de_media_class.SetByteValue(rt_dose_class, strlen(rt_dose_class));
        file->GetHeader().Insert(de_media_class);

        // Media Storage SOP Instance UID
        gdcm::DataElement de_media_inst(gdcm::Tag(0x0002, 0x0003));
        de_media_inst.SetVR(gdcm::VR::UI);
        de_media_inst.SetByteValue(sop_uid.c_str(), sop_uid.length());
        file->GetHeader().Insert(de_media_inst);

        // Transfer Syntax UID
        const char* ts_uid = "1.2.840.10008.1.2.1";  // Explicit VR Little Endian
        gdcm::DataElement de_ts_uid(gdcm::Tag(0x0002, 0x0010));
        de_ts_uid.SetVR(gdcm::VR::UI);
        de_ts_uid.SetByteValue(ts_uid, strlen(ts_uid));
        file->GetHeader().Insert(de_ts_uid);

        // Implementation Class UID
        const char* impl_uid = "1.2.826.0.1.3680043.2.1143.1";  // GDCM Implementation UID
        gdcm::DataElement de_impl_uid(gdcm::Tag(0x0002, 0x0012));
        de_impl_uid.SetVR(gdcm::VR::UI);
        de_impl_uid.SetByteValue(impl_uid, strlen(impl_uid));
        file->GetHeader().Insert(de_impl_uid);

        // Implementation Version Name
        const char* impl_ver = "MOQUI_SMC_1.0";
        gdcm::DataElement de_impl_ver(gdcm::Tag(0x0002, 0x0013));
        de_impl_ver.SetVR(gdcm::VR::SH);
        de_impl_ver.SetByteValue(impl_ver, strlen(impl_ver));
        file->GetHeader().Insert(de_impl_ver);

        // Write file using ImageWriter
        std::string output_path = filepath + "/" + filename + ".dcm";
        gdcm::ImageWriter writer;
        writer.SetFileName(output_path.c_str());
        writer.SetImage(*image);  // GDCM takes ownership
        writer.SetFile(*file);    // GDCM takes ownership

        if (writer.Write()) {
            std::cout << "Successfully wrote DICOM RT Dose file: " << output_path << std::endl;
            std::cout << "  Dimensions: " << dim.x << "x" << dim.y << "x" << dim.z << std::endl;
            std::cout << "  Dose grid scaling: " << dose_grid_scaling << " Gy/pixel" << std::endl;
            std::cout << "  Max dose: " << max_dose << " Gy" << std::endl;
        } else {
            std::cerr << "Error: Failed to write DICOM file: " << output_path << std::endl;
        }

        // Note: image and file are NOT deleted - GDCM Writer manages memory automatically

    } catch (const std::exception& e) {
        std::cerr << "Exception in save_to_dcm: " << e.what() << std::endl;
    }
}

#endif
