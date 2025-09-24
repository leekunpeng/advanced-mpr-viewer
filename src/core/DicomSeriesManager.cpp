#include "DicomSeriesManager.h"
#include <gdcmReader.h>
#include <gdcmFile.h>
#include <gdcmDataSet.h>
#include <gdcmAttribute.h>
#include <iostream>
#include <filesystem>
#include <map>
#include <algorithm>

std::string DicomSeriesManager::s_lastError;

std::string DicomSeriesManager::getLastError()
{
    return s_lastError;
}

std::vector<DicomSeriesLoader::SeriesInfo> DicomSeriesManager::scanDirectory(const std::string& directory)
{
    s_lastError.clear();
    std::vector<DicomSeriesLoader::SeriesInfo> seriesList;
    
    try {
        if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
            s_lastError = "Directory does not exist or is not a directory: " + directory;
            return seriesList;
        }
        
        // Map to group files by series UID
        std::map<std::string, DicomSeriesLoader::SeriesInfo> seriesMap;
        
        // Iterate through all files in directory
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filePath = entry.path().string();
                
                if (isDicomFile(filePath)) {
                    std::string seriesUID, modality, seriesDescription, patientID, studyUID, studyDate;
                    double pixelSpacing[2];
                    double sliceThickness;
                    int rows, columns;
                    
                    if (extractSeriesInfo(filePath, seriesUID, modality, seriesDescription,
                                        patientID, studyUID, studyDate, pixelSpacing, 
                                        sliceThickness, rows, columns)) {
                        
                        // Add or update series info
                        auto& seriesInfo = seriesMap[seriesUID];
                        
                        if (seriesInfo.seriesUID.empty()) {
                            // First file in this series
                            seriesInfo.seriesUID = seriesUID;
                            seriesInfo.modality = modality;
                            seriesInfo.seriesDescription = seriesDescription;
                            seriesInfo.patientID = patientID;
                            seriesInfo.studyUID = studyUID;
                            seriesInfo.studyDate = studyDate;
                            seriesInfo.pixelSpacing[0] = pixelSpacing[0];
                            seriesInfo.pixelSpacing[1] = pixelSpacing[1];
                            seriesInfo.sliceThickness = sliceThickness;
                            seriesInfo.imageRows = rows;
                            seriesInfo.imageCols = columns;
                            seriesInfo.numSlices = 0;
                        }
                        
                        // Add file to series
                        seriesInfo.filePaths.push_back(filePath);
                        seriesInfo.numSlices++;
                    } else {
                        std::cout << "Warning: Could not extract series info from " << filePath << std::endl;
                    }
                } else {
                    // Skip non-DICOM files silently
                }
            }
        }
        
        // Convert map to vector
        seriesList.reserve(seriesMap.size());
        for (const auto& pair : seriesMap) {
            if (pair.second.isValid()) {
                seriesList.push_back(pair.second);
            }
        }
        
        // Sort series by series description and then by series UID
        std::sort(seriesList.begin(), seriesList.end(),
                  [](const DicomSeriesLoader::SeriesInfo& a, const DicomSeriesLoader::SeriesInfo& b) {
                      if (a.seriesDescription != b.seriesDescription) {
                          return a.seriesDescription < b.seriesDescription;
                      }
                      return a.seriesUID < b.seriesUID;
                  });
        
        std::cout << "Found " << seriesList.size() << " DICOM series in " << directory << std::endl;
        for (const auto& series : seriesList) {
            std::cout << "  Series: " << series.seriesDescription << " (" << series.modality 
                      << ") - " << series.numSlices << " slices, " 
                      << series.imageCols << "x" << series.imageRows << std::endl;
        }
        
    }
    catch (const std::exception& e) {
        s_lastError = std::string("Exception in scanDirectory: ") + e.what();
        seriesList.clear();
    }
    
    return seriesList;
}

Volume3D DicomSeriesManager::loadSeries(const DicomSeriesLoader::SeriesInfo& seriesInfo)
{
    s_lastError.clear();
    return DicomSeriesLoader::loadFromSeriesInfo(seriesInfo);
}

bool DicomSeriesManager::isDicomFile(const std::string& filePath)
{
    try {
        // Simple check - try to read as DICOM file
        gdcm::Reader reader;
        reader.SetFileName(filePath.c_str());
        
        // Just check if the file can be opened as DICOM, don't do full read
        return reader.CanRead();
    }
    catch (...) {
        return false;
    }
}

bool DicomSeriesManager::extractSeriesInfo(const std::string& filePath,
                                          std::string& seriesUID,
                                          std::string& modality,
                                          std::string& seriesDescription,
                                          std::string& patientID,
                                          std::string& studyUID,
                                          std::string& studyDate,
                                          double pixelSpacing[2],
                                          double& sliceThickness,
                                          int& rows,
                                          int& columns)
{
    try {
        gdcm::Reader reader;
        reader.SetFileName(filePath.c_str());
        
        if (!reader.Read()) {
            return false;
        }
        
        const gdcm::File& file = reader.GetFile();
        const gdcm::DataSet& ds = file.GetDataSet();
        
        // Series Instance UID (0020,000E)
        gdcm::Attribute<0x0020, 0x000E> seriesInstanceUID;
        seriesInstanceUID.SetFromDataSet(ds);
        if (ds.FindDataElement(seriesInstanceUID.GetTag())) {
            seriesUID = seriesInstanceUID.GetValue();
        }
        
        // Modality (0008,0060)
        gdcm::Attribute<0x0008, 0x0060> modalityAttr;
        modalityAttr.SetFromDataSet(ds);
        if (ds.FindDataElement(modalityAttr.GetTag())) {
            modality = modalityAttr.GetValue();
        }
        
        // Series Description (0008,103E)
        gdcm::Attribute<0x0008, 0x103E> seriesDescAttr;
        seriesDescAttr.SetFromDataSet(ds);
        if (ds.FindDataElement(seriesDescAttr.GetTag())) {
            seriesDescription = seriesDescAttr.GetValue();
        }
        
        // Patient ID (0010,0020)
        gdcm::Attribute<0x0010, 0x0020> patientIDAttr;
        patientIDAttr.SetFromDataSet(ds);
        if (ds.FindDataElement(patientIDAttr.GetTag())) {
            patientID = patientIDAttr.GetValue();
        }
        
        // Study Instance UID (0020,000D)
        gdcm::Attribute<0x0020, 0x000D> studyInstanceUID;
        studyInstanceUID.SetFromDataSet(ds);
        if (ds.FindDataElement(studyInstanceUID.GetTag())) {
            studyUID = studyInstanceUID.GetValue();
        }
        
        // Study Date (0008,0020)
        gdcm::Attribute<0x0008, 0x0020> studyDateAttr;
        studyDateAttr.SetFromDataSet(ds);
        if (ds.FindDataElement(studyDateAttr.GetTag())) {
            studyDate = studyDateAttr.GetValue();
        }
        
        // Pixel Spacing (0028,0030)
        gdcm::Attribute<0x0028, 0x0030> pixelSpacingAttr;
        pixelSpacingAttr.SetFromDataSet(ds);
        if (ds.FindDataElement(pixelSpacingAttr.GetTag())) {
            const double* values = pixelSpacingAttr.GetValues();
            if (pixelSpacingAttr.GetNumberOfValues() >= 2) {
                pixelSpacing[0] = values[0]; // Row spacing
                pixelSpacing[1] = values[1]; // Column spacing
            } else {
                pixelSpacing[0] = pixelSpacing[1] = 1.0;
            }
        } else {
            pixelSpacing[0] = pixelSpacing[1] = 1.0;
        }
        
        // Slice Thickness (0018,0050)
        gdcm::Attribute<0x0018, 0x0050> sliceThicknessAttr;
        sliceThicknessAttr.SetFromDataSet(ds);
        if (ds.FindDataElement(sliceThicknessAttr.GetTag())) {
            sliceThickness = sliceThicknessAttr.GetValue();
        } else {
            sliceThickness = 1.0;
        }
        
        // Rows (0028,0010)
        gdcm::Attribute<0x0028, 0x0010> rowsAttr;
        rowsAttr.SetFromDataSet(ds);
        if (ds.FindDataElement(rowsAttr.GetTag())) {
            rows = rowsAttr.GetValue();
        }
        
        // Columns (0028,0011)
        gdcm::Attribute<0x0028, 0x0011> columnsAttr;
        columnsAttr.SetFromDataSet(ds);
        if (ds.FindDataElement(columnsAttr.GetTag())) {
            columns = columnsAttr.GetValue();
        }
        
        return !seriesUID.empty() && rows > 0 && columns > 0;
    }
    catch (const std::exception& e) {
        std::cout << "Exception extracting series info from " << filePath << ": " << e.what() << std::endl;
        return false;
    }
}