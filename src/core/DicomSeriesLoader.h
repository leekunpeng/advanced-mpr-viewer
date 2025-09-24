#pragma once

#include "Volume3D.h"
#include <string>
#include <vector>

/**
 * @brief GDCM-based DICOM series loader
 * 
 * This class loads a single DICOM series into a normalized 3D volume with correct 
 * LPS geometry. It handles slice sorting, spacing calculation, and pixel data 
 * conversion to float32 with rescaling applied.
 */
class DicomSeriesLoader
{
public:
    /**
     * @brief Structure to hold basic series information
     */
    struct SeriesInfo
    {
        std::string seriesUID;
        std::string modality;
        std::string seriesDescription;
        std::string patientID;
        std::string studyUID;
        std::string studyDate;
        int numSlices{0};
        double pixelSpacing[2]{1.0, 1.0};  // row, column spacing
        double sliceThickness{1.0};
        int imageRows{0};
        int imageCols{0};
        std::vector<std::string> filePaths; // All files belonging to this series
        
        bool isValid() const {
            return !seriesUID.empty() && numSlices > 0 && imageRows > 0 && imageCols > 0;
        }
    };
    
    /**
     * @brief Load DICOM series from directory
     * @param directory Path to directory containing DICOM files
     * @param seriesUID Specific series to load (empty = auto-select first series)
     * @return Volume3D with loaded data, or invalid volume on error
     */
    static Volume3D loadFromDirectory(const std::string& directory, const std::string& seriesUID = "");
    
    /**
     * @brief Load DICOM series from SeriesInfo
     * @param seriesInfo Series information with file paths
     * @return Volume3D with loaded data, or invalid volume on error
     */
    static Volume3D loadFromSeriesInfo(const SeriesInfo& seriesInfo);
    
    /**
     * @brief Get last error message
     */
    static std::string getLastError();

private:
    /**
     * @brief Structure to hold slice-specific information for sorting
     */
    struct SliceInfo
    {
        std::string filePath;
        double imagePosition[3]{0.0, 0.0, 0.0};      // IPP - Image Position Patient
        double imageOrientation[6];                    // IOP - Image Orientation Patient  
        double sliceLocation{0.0};                     // Slice Location (if available)
        double projectedPosition{0.0};                 // Position projected onto slice normal
        int instanceNumber{0};                         // Instance Number
        int sliceIndex{-1};                           // Final sorted index
        
        // Pixel data info
        int rows{0};
        int columns{0};
        int bitsAllocated{0};
        int bitsStored{0};
        int pixelRepresentation{0};  // 0=unsigned, 1=signed
        double rescaleIntercept{0.0};
        double rescaleSlope{1.0};
        bool hasRescale{false};
        double pixelSpacing[2]{1.0, 1.0}; // row, column
    };
    
    /**
     * @brief Extract slice information from DICOM file
     * @param filePath Path to DICOM file
     * @param slice Output slice information
     * @return true on success
     */
    static bool extractSliceInfo(const std::string& filePath, SliceInfo& slice);
    
    /**
     * @brief Sort slices along slice normal direction
     * @param slices Vector of slice information to sort
     * @return true on success
     */
    static bool sortSlices(std::vector<SliceInfo>& slices);
    
    /**
     * @brief Calculate slice spacing from sorted slice positions
     * @param slices Sorted slice information
     * @return Calculated spacing in mm, or fallback value
     */
    static double calculateSliceSpacing(const std::vector<SliceInfo>& slices);
    
    /**
     * @brief Load pixel data from DICOM file
     * @param slice Slice information
     * @param pixelData Output buffer for pixel values (float32)
     * @return true on success
     */
    static bool loadPixelData(const SliceInfo& slice, std::vector<float>& pixelData);
    
    /**
     * @brief Validate slice consistency (same dimensions, orientation, etc.)
     * @param slices Vector of slices to validate
     * @return true if all slices are consistent
     */
    static bool validateSliceConsistency(const std::vector<SliceInfo>& slices);
    
    /**
     * @brief Compute slice direction from image orientation
     * @param imageOrientation IOP array [row_x, row_y, row_z, col_x, col_y, col_z]
     * @param sliceDirection Output slice direction (cross product of row and col)
     */
    static void computeSliceDirection(const double imageOrientation[6], double sliceDirection[3]);
    
    /**
     * @brief Normalize direction vector to unit length
     * @param direction Direction vector to normalize (in-place)
     */
    static void normalizeVector(double direction[3]);
    
    /**
     * @brief Calculate dot product of two vectors
     */
    static double dotProduct(const double a[3], const double b[3]);
    
    /**
     * @brief Copy 3D vector
     */
    static void copyVector(const double src[3], double dst[3]);
    
    static std::string s_lastError;
};