#ifndef MQI_IO_HPP
#define MQI_IO_HPP

// New modular structure
#include <moqui/base/io/mqi_io_common.hpp>
#include <moqui/base/io/mqi_dicom_header.hpp>
#include <moqui/base/io/mqi_io_writers.hpp>

namespace mqi
{
namespace io
{

// ============================================================================
// Backward-compatible wrapper functions
// ============================================================================

/// Save scorer data to binary files (backward compatible)
template<typename R>
void save_to_bin(const mqi::scorer<R>* src,
                const R               scale,
                const std::string&    filepath,
                const std::string&    filename) {
    BinaryWriter<R>::save_scorer(src, scale, filepath, filename);
}

/// Save array to binary file (backward compatible)
template<typename R>
void save_to_bin(const R*           src,
                const R            scale,
                const std::string& filepath,
                const std::string& filename,
                const uint32_t     length) {
    BinaryWriter<R>::save_array(src, scale, filepath, filename, length);
}

/// Save key-value pairs to binary (backward compatible)
template<typename R>
void save_to_bin(const mqi::key_value* src,
                const R               scale,
                uint32_t              max_capacity,
                const std::string&    filepath,
                const std::string&    filename) {
    // Extract data
    std::vector<mqi::key_t> key1, key2;
    std::vector<R> value;

    for (uint32_t ind = 0; ind < max_capacity; ind++) {
        if (src[ind].key1 != mqi::empty_pair &&
            src[ind].key2 != mqi::empty_pair &&
            src[ind].value > 0) {
            key1.push_back(src[ind].key1);
            key2.push_back(src[ind].key2);
            value.push_back(src[ind].value * scale);
        }
    }

    // Write files
    write_binary_file(build_file_path(filepath, filename + "_key1", "raw"),
                     key1.data(), key1.size());
    write_binary_file(build_file_path(filepath, filename + "_key2", "raw"),
                     key2.data(), key2.size());
    write_binary_file(build_file_path(filepath, filename + "_value", "raw"),
                     value.data(), value.size());
}

/// Save scorer to NPZ format (backward compatible)
template<typename R>
void save_to_npz(const mqi::scorer<R>* src,
                const R               scale,
                const std::string&    filepath,
                const std::string&    filename,
                mqi::vec3<mqi::ijk_t> dim,
                uint32_t              num_spots) {
    NpzWriter<R>::save_scorer(src, scale, filepath, filename, dim, num_spots);
}

/// Save scorer to NPZ format with threshold (backward compatible)
template<typename R>
void save_to_npz(const mqi::scorer<R>* src,
                const R               scale,
                const std::string&    filepath,
                const std::string&    filename,
                mqi::vec3<mqi::ijk_t> dim,
                uint32_t              num_spots,
                R*                    time_scale,
                R                     threshold) {
    // This version with time_scale and threshold needs custom implementation
    // For now, delegate to the simpler version
    // TODO: Implement threshold version in NpzWriter
    NpzWriter<R>::save_scorer(src, scale, filepath, filename, dim, num_spots);
}

/// Save to MHD format (backward compatible)
template<typename R>
void save_to_mhd(const mqi::node_t<R>* geometry,
                const double*         data,
                const R               scale,
                const std::string&    filepath,
                const std::string&    filename,
                const uint32_t        length) {
    MetaImageWriter<R>::save_mhd(geometry, data, scale, filepath, filename, length);
}

/// Save to MHA format (backward compatible)
template<typename R>
void save_to_mha(const mqi::node_t<R>* geometry,
                const double*         data,
                const R               scale,
                const std::string&    filepath,
                const std::string&    filename,
                const uint32_t        length) {
    MetaImageWriter<R>::save_mha(geometry, data, scale, filepath, filename, length);
}

/// Save to DICOM RT Dose format (backward compatible)
template<typename R>
void save_to_dcm(const mqi::scorer<R>*        src,
                const mqi::node_t<R>*        geometry_node,
                const dcm_header_info*       header_info,
                const R                      scale,
                const std::string&           filepath,
                const std::string&           filename,
                const uint32_t               length,
                const mqi::vec3<mqi::ijk_t>& dim,
                const bool                   is_2cm_mode = false) {
    DicomWriter<R>::save_from_scorer(src, geometry_node, header_info, scale,
                                     filepath, filename, dim, is_2cm_mode);
}

}   // namespace io
}   // namespace mqi

#endif // MQI_IO_HPP
