#pragma once

#include "DicomSeriesLoader.h"
#include <string>
#include <vector>

/**
 * @brief DICOM series manager for directory scanning and series selection
 * 
 * This class scans directories for DICOM files, groups them by series,
 * and provides functionality to select and load specific series.
 */
class DicomSeriesManager
{
public:
    /**
     * @brief Scan directory for DICOM series
     * @param directory Path to directory containing DICOM files
     * @return Vector of series information found in directory
     */
    static std::vector<DicomSeriesLoader::SeriesInfo> scanDirectory(const std::string& directory);
    
    /**
     * @brief Load a specific series
     * @param seriesInfo Series information from scanDirectory
     * @return Volume3D with loaded data, or invalid volume on error
     */
    static Volume3D loadSeries(const DicomSeriesLoader::SeriesInfo& seriesInfo);
    
    /**
     * @brief Get last error message
     */
    static std::string getLastError();
    
private:
    /**
     * @brief Check if file is a DICOM file
     * @param filePath Path to file to check
     * @return true if file appears to be DICOM
     */
    static bool isDicomFile(const std::string& filePath);
    
    /**
     * @brief Extract basic series information from DICOM file
     * @param filePath Path to DICOM file
     * @param seriesUID Output series instance UID
     * @param modality Output modality
     * @param seriesDescription Output series description
     * @param patientID Output patient ID
     * @param studyUID Output study instance UID
     * @param studyDate Output study date
     * @param pixelSpacing Output pixel spacing [row, column]
     * @param sliceThickness Output slice thickness
     * @param rows Output number of rows
     * @param columns Output number of columns
     * @return true on success
     */
    static bool extractSeriesInfo(const std::string& filePath,
                                 std::string& seriesUID,
                                 std::string& modality,
                                 std::string& seriesDescription,
                                 std::string& patientID,
                                 std::string& studyUID,
                                 std::string& studyDate,
                                 double pixelSpacing[2],
                                 double& sliceThickness,
                                 int& rows,
                                 int& columns);
    
    static std::string s_lastError;
};