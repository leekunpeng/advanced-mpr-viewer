#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include <limits>

/**
 * @brief Volume3D represents a 3D scalar volume with correct LPS geometry
 * 
 * This class holds DICOM image data as a normalized 3D volume with proper
 * spatial information for MPR rendering. All geometry is in LPS coordinate system:
 * - L: Left-Right (X axis, positive = Left)  
 * - P: Posterior-Anterior (Y axis, positive = Posterior)
 * - S: Superior-Inferior (Z axis, positive = Superior)
 */
struct Volume3D 
{
    // Volume dimensions
    int width{0};   // X dimension (columns)
    int height{0};  // Y dimension (rows) 
    int depth{0};   // Z dimension (slices)
    
    // Voxel spacing in mm (LPS coordinate system)
    double spacing[3]{1.0, 1.0, 1.0};  // dx, dy, dz
    
    // Volume origin in LPS coordinates (mm)
    // This is the position of the first voxel [0,0,0]
    double origin[3]{0.0, 0.0, 0.0};   // x, y, z in LPS
    
    // Direction vectors (unit vectors in LPS coordinate system)
    // These define the orientation of the volume axes relative to patient coordinate system
    double rowDir[3]{1.0, 0.0, 0.0};      // X direction (column/width direction)
    double colDir[3]{0.0, 1.0, 0.0};      // Y direction (row/height direction)  
    double sliceDir[3]{0.0, 0.0, 1.0};    // Z direction (slice/depth direction)
    
    // Volume buffer (float32 normalized values)
    // Size = width * height * depth
    // Storage order: [z][y][x] - slice-major ordering
    std::vector<float> voxels;
    
    // Value range information
    float vmin{0.0f};  // Minimum value in volume
    float vmax{0.0f};  // Maximum value in volume
    
    // DICOM metadata
    std::string modality;       // CT, MR, PT, etc.
    std::string patientID;
    std::string studyUID;
    std::string seriesUID;
    std::string studyDate;
    std::string seriesDescription;
    
    // Rescale parameters (for reference - already applied to voxel data)
    double rescaleIntercept{0.0};
    double rescaleSlope{1.0};
    bool hasRescaleParams{false};
    
    /**
     * @brief Default constructor creates empty volume
     */
    Volume3D() = default;
    
    /**
     * @brief Constructor with dimensions
     */
    Volume3D(int w, int h, int d) 
        : width(w), height(h), depth(d)
    {
        voxels.resize(static_cast<size_t>(width) * height * depth, 0.0f);
    }
    
    /**
     * @brief Get total number of voxels
     */
    size_t getTotalVoxels() const 
    {
        return static_cast<size_t>(width) * height * depth;
    }
    
    /**
     * @brief Check if volume is valid (has data)
     */
    bool isValid() const 
    {
        return width > 0 && height > 0 && depth > 0 && !voxels.empty();
    }
    
    /**
     * @brief Get voxel value at position (x, y, z)
     * @param x Column index [0, width-1]
     * @param y Row index [0, height-1] 
     * @param z Slice index [0, depth-1]
     * @return Voxel value or 0.0f if indices out of bounds
     */
    float getVoxel(int x, int y, int z) const 
    {
        if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth) {
            return 0.0f;
        }
        size_t index = static_cast<size_t>(z) * width * height + 
                       static_cast<size_t>(y) * width + 
                       static_cast<size_t>(x);
        return voxels[index];
    }
    
    /**
     * @brief Set voxel value at position (x, y, z)
     */
    void setVoxel(int x, int y, int z, float value) 
    {
        if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth) {
            return;
        }
        size_t index = static_cast<size_t>(z) * width * height + 
                       static_cast<size_t>(y) * width + 
                       static_cast<size_t>(x);
        voxels[index] = value;
    }
    
    /**
     * @brief Convert world coordinates (LPS) to voxel coordinates
     * @param worldX X coordinate in LPS space (mm)
     * @param worldY Y coordinate in LPS space (mm)
     * @param worldZ Z coordinate in LPS space (mm)
     * @param voxelX Output voxel X coordinate 
     * @param voxelY Output voxel Y coordinate
     * @param voxelZ Output voxel Z coordinate
     */
    void worldToVoxel(double worldX, double worldY, double worldZ,
                      double& voxelX, double& voxelY, double& voxelZ) const
    {
        // Translate to volume origin
        double tx = worldX - origin[0];
        double ty = worldY - origin[1]; 
        double tz = worldZ - origin[2];
        
        // Project onto volume axes and scale by spacing
        voxelX = (tx * rowDir[0] + ty * rowDir[1] + tz * rowDir[2]) / spacing[0];
        voxelY = (tx * colDir[0] + ty * colDir[1] + tz * colDir[2]) / spacing[1];
        voxelZ = (tx * sliceDir[0] + ty * sliceDir[1] + tz * sliceDir[2]) / spacing[2];
    }
    
    /**
     * @brief Convert voxel coordinates to world coordinates (LPS)
     */
    void voxelToWorld(double voxelX, double voxelY, double voxelZ,
                      double& worldX, double& worldY, double& worldZ) const
    {
        // Scale by spacing and transform by direction vectors
        double dx = voxelX * spacing[0];
        double dy = voxelY * spacing[1];
        double dz = voxelZ * spacing[2];
        
        worldX = origin[0] + dx * rowDir[0] + dy * colDir[0] + dz * sliceDir[0];
        worldY = origin[1] + dx * rowDir[1] + dy * colDir[1] + dz * sliceDir[1];
        worldZ = origin[2] + dx * rowDir[2] + dy * colDir[2] + dz * sliceDir[2];
    }
    
    /**
     * @brief Check if direction vectors form a right-handed orthonormal basis
     * @param epsilon Tolerance for orthogonality check (default 1e-6)
     */
    bool isOrthonormal(double epsilon = 1e-6) const
    {
        // Check if vectors are unit length
        auto length = [](const double v[3]) {
            return std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
        };
        
        if (std::abs(length(rowDir) - 1.0) > epsilon ||
            std::abs(length(colDir) - 1.0) > epsilon ||
            std::abs(length(sliceDir) - 1.0) > epsilon) {
            return false;
        }
        
        // Check orthogonality
        auto dot = [](const double a[3], const double b[3]) {
            return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
        };
        
        if (std::abs(dot(rowDir, colDir)) > epsilon ||
            std::abs(dot(rowDir, sliceDir)) > epsilon ||
            std::abs(dot(colDir, sliceDir)) > epsilon) {
            return false;
        }
        
        // Check right-handedness: sliceDir ≈ rowDir × colDir
        double cross[3] = {
            rowDir[1] * colDir[2] - rowDir[2] * colDir[1],
            rowDir[2] * colDir[0] - rowDir[0] * colDir[2], 
            rowDir[0] * colDir[1] - rowDir[1] * colDir[0]
        };
        
        if (std::abs(cross[0] - sliceDir[0]) > epsilon ||
            std::abs(cross[1] - sliceDir[1]) > epsilon ||
            std::abs(cross[2] - sliceDir[2]) > epsilon) {
            return false;
        }
        
        return true;
    }
};