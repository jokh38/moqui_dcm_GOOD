#ifndef MQI_IO_COMMON_HPP
#define MQI_IO_COMMON_HPP

#include <algorithm>
#include <ctime>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace mqi
{
namespace io
{

// Forward declarations
template<typename R> class scorer;

/// Simple 3D vector for IO operations (to avoid dependencies)
template<typename T>
struct vec3 {
    T x, y, z;
    vec3() : x(0), y(0), z(0) {}
    vec3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {}
};

/// Metadata structure for file output
struct FileMetadata {
    vec3<int> dimensions;
    vec3<float> spacing;
    vec3<float> origin;
};

/// Abstract base class for file writers (Strategy pattern)
class IFileWriter {
public:
    virtual ~IFileWriter() = default;
    virtual void write(const std::string& filepath, const std::string& filename) = 0;
};

// ============================================================================
// Utility Functions
// ============================================================================

/// Generate current date in DICOM format (YYYYMMDD)
inline std::string get_current_date() {
    time_t now = time(0);
    struct tm tstruct = *localtime(&now);
    char buf[9];
    strftime(buf, sizeof(buf), "%Y%m%d", &tstruct);
    return std::string(buf);
}

/// Generate current time in DICOM format (HHMMSS)
inline std::string get_current_time() {
    time_t now = time(0);
    struct tm tstruct = *localtime(&now);
    char buf[7];
    strftime(buf, sizeof(buf), "%H%M%S", &tstruct);
    return std::string(buf);
}

// Note: UID generation is handled by GDCM library (gdcm::UIDGenerator)
// for DICOM files to ensure compliance with DICOM standards.
// No custom UID generation is needed in this common utilities file.

/// Build file path from directory, filename, and extension
inline std::string build_file_path(const std::string& dir,
                                    const std::string& name,
                                    const std::string& ext) {
    std::string path = dir;
    if (!path.empty() && path.back() != '/') {
        path += '/';
    }
    path += name + "." + ext;
    return path;
}

// ============================================================================
// Data Extraction and Transformation
// ============================================================================

/// Extract non-zero indices from sparse data
inline std::vector<std::pair<uint32_t, double>>
extract_nonzero_indices(const std::vector<std::pair<uint32_t, double>>& data) {
    std::vector<std::pair<uint32_t, double>> result;
    result.reserve(data.size());

    for (const auto& pair : data) {
        if (pair.second != 0.0) {
            result.push_back(pair);
        }
    }

    return result;
}

/// Apply scaling to data vector
inline void apply_scaling(std::vector<double>& data, double scale) {
    for (auto& val : data) {
        val *= scale;
    }
}

/// Apply scaling to data vector (float version)
inline void apply_scaling(std::vector<float>& data, float scale) {
    for (auto& val : data) {
        val *= scale;
    }
}

// ============================================================================
// File I/O Helpers
// ============================================================================

/// Write binary data to file
template<typename T>
inline bool write_binary_file(const std::string& filepath,
                               const T* data,
                               size_t count) {
    std::ofstream file(filepath, std::ios::binary | std::ios::out);
    if (!file) {
        return false;
    }

    file.write(reinterpret_cast<const char*>(data), count * sizeof(T));
    file.close();

    return file.good();
}

/// Read binary data from file
template<typename T>
inline bool read_binary_file(const std::string& filepath,
                              T* data,
                              size_t count) {
    std::ifstream file(filepath, std::ios::binary | std::ios::in);
    if (!file) {
        return false;
    }

    file.read(reinterpret_cast<char*>(data), count * sizeof(T));
    file.close();

    return file.good();
}

/// Check if file exists
inline bool file_exists(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

/// Get file size in bytes
inline size_t get_file_size(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file) {
        return 0;
    }
    return static_cast<size_t>(file.tellg());
}

}   // namespace io
}   // namespace mqi

#endif // MQI_IO_COMMON_HPP
