#include "DicomSeriesLoader.h"
#include <gdcmReader.h>
#include <gdcmFile.h>
#include <gdcmDataSet.h>
#include <gdcmAttribute.h>
#include <gdcmImageReader.h>
#include <gdcmImage.h>
#include <gdcmPixelFormat.h>
#include <gdcmPhotometricInterpretation.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>

std::string DicomSeriesLoader::s_lastError;

std::string DicomSeriesLoader::getLastError()
{
    return s_lastError;
}

Volume3D DicomSeriesLoader::loadFromDirectory(const std::string& directory, const std::string& seriesUID)
{
    s_lastError.clear();
    
    try {
        // For now, create a simple implementation that doesn't require directory scanning
        // This would normally be handled by DicomSeriesManager
        s_lastError = "loadFromDirectory requires DicomSeriesManager for directory scanning";
        return Volume3D{}; // Return invalid volume
    }
    catch (const std::exception& e) {
        s_lastError = std::string("Exception in loadFromDirectory: ") + e.what();
        return Volume3D{};
    }
}

Volume3D DicomSeriesLoader::loadFromSeriesInfo(const SeriesInfo& seriesInfo)
{
    s_lastError.clear();
    
    if (!seriesInfo.isValid()) {
        s_lastError = "Invalid series information provided";
        return Volume3D{};
    }
    
    if (seriesInfo.filePaths.empty()) {
        s_lastError = "No files in series";
        return Volume3D{};
    }
    
    try {
        std::vector<SliceInfo> slices;
        slices.reserve(seriesInfo.filePaths.size());
        
        // Extract information from each DICOM file
        for (const auto& filePath : seriesInfo.filePaths) {
            SliceInfo slice;
            if (extractSliceInfo(filePath, slice)) {
                slices.push_back(slice);
            } else {
                std::cout << "Warning: Failed to extract info from " << filePath << std::endl;
            }
        }
        
        if (slices.empty()) {
            s_lastError = "No valid DICOM slices found";
            return Volume3D{};
        }
        
        // Validate slice consistency
        if (!validateSliceConsistency(slices)) {
            s_lastError = "Slice consistency validation failed";
            return Volume3D{};
        }
        
        // Sort slices
        if (!sortSlices(slices)) {
            s_lastError = "Failed to sort slices";
            return Volume3D{};
        }
        
        // Create volume
        Volume3D volume(slices[0].columns, slices[0].rows, static_cast<int>(slices.size()));
        
        // Set spacing
        volume.spacing[0] = slices[0].pixelSpacing[1]; // Column spacing (X)
        volume.spacing[1] = slices[0].pixelSpacing[0]; // Row spacing (Y)  
        volume.spacing[2] = calculateSliceSpacing(slices); // Slice spacing (Z)
        
        // Set origin from first slice position (after sorting)
        volume.origin[0] = slices[0].imagePosition[0];
        volume.origin[1] = slices[0].imagePosition[1];
        volume.origin[2] = slices[0].imagePosition[2];
        
        // Set direction vectors from image orientation
        const double* iop = slices[0].imageOrientation;
        
        // Row direction (first 3 components of IOP) - maps to X direction
        volume.rowDir[0] = iop[0];
        volume.rowDir[1] = iop[1]; 
        volume.rowDir[2] = iop[2];
        normalizeVector(volume.rowDir);
        
        // Column direction (last 3 components of IOP) - maps to Y direction
        volume.colDir[0] = iop[3];
        volume.colDir[1] = iop[4];
        volume.colDir[2] = iop[5];
        normalizeVector(volume.colDir);
        
        // Slice direction (cross product) - maps to Z direction
        computeSliceDirection(iop, volume.sliceDir);
        normalizeVector(volume.sliceDir);
        
        // Set metadata
        volume.modality = seriesInfo.modality;
        volume.patientID = seriesInfo.patientID;
        volume.studyUID = seriesInfo.studyUID;
        volume.seriesUID = seriesInfo.seriesUID;
        volume.studyDate = seriesInfo.studyDate;
        volume.seriesDescription = seriesInfo.seriesDescription;
        
        // Load pixel data for each slice
        volume.vmin = std::numeric_limits<float>::max();
        volume.vmax = std::numeric_limits<float>::lowest();
        
        for (size_t i = 0; i < slices.size(); ++i) {
            std::vector<float> slicePixels;
            if (loadPixelData(slices[i], slicePixels)) {
                // Copy slice data into volume buffer
                size_t sliceSize = static_cast<size_t>(volume.width) * volume.height;
                size_t volumeOffset = i * sliceSize;
                
                if (volumeOffset + slicePixels.size() <= volume.voxels.size()) {
                    std::copy(slicePixels.begin(), slicePixels.end(), 
                             volume.voxels.begin() + volumeOffset);
                    
                    // Update min/max
                    for (float value : slicePixels) {
                        volume.vmin = std::min(volume.vmin, value);
                        volume.vmax = std::max(volume.vmax, value);
                    }
                }
            } else {
                s_lastError = "Failed to load pixel data for slice " + std::to_string(i);
                return Volume3D{};
            }
        }
        
        // Store rescale parameters from first slice
        if (slices[0].hasRescale) {
            volume.rescaleIntercept = slices[0].rescaleIntercept;
            volume.rescaleSlope = slices[0].rescaleSlope;
            volume.hasRescaleParams = true;
        }
        
        // Verify orthonormal basis
        if (!volume.isOrthonormal()) {
            std::cout << "Warning: Direction vectors do not form orthonormal basis" << std::endl;
        }
        
        std::cout << "Volume loaded successfully:" << std::endl;
        std::cout << "  Series: " << volume.seriesUID << std::endl;
        std::cout << "  Modality: " << volume.modality << std::endl;
        std::cout << "  Dimensions: " << volume.width << "x" << volume.height << "x" << volume.depth << std::endl;
        std::cout << "  Spacing: " << volume.spacing[0] << ", " << volume.spacing[1] << ", " << volume.spacing[2] << " mm" << std::endl;
        std::cout << "  Origin: " << volume.origin[0] << ", " << volume.origin[1] << ", " << volume.origin[2] << " mm" << std::endl;
        std::cout << "  Value range: " << volume.vmin << " to " << volume.vmax << std::endl;
        
        return volume;
    }
    catch (const std::exception& e) {
        s_lastError = std::string("Exception in loadFromSeriesInfo: ") + e.what();
        return Volume3D{};
    }
}

bool DicomSeriesLoader::extractSliceInfo(const std::string& filePath, SliceInfo& slice)
{
    try {
        gdcm::ImageReader reader;
        reader.SetFileName(filePath.c_str());
        
        if (!reader.Read()) {
            std::cout << "Failed to read DICOM file: " << filePath << std::endl;
            return false;
        }
        
        const gdcm::File& file = reader.GetFile();
        const gdcm::DataSet& ds = file.GetDataSet();
        
        slice.filePath = filePath;
        
        // Extract Image Position Patient (0020,0032)
        gdcm::Attribute<0x0020, 0x0032> ipp;
        ipp.SetFromDataSet(ds);
        if (ds.FindDataElement(ipp.GetTag())) {
            const double* values = ipp.GetValues();
            if (ipp.GetNumberOfValues() >= 3) {
                slice.imagePosition[0] = values[0];
                slice.imagePosition[1] = values[1];
                slice.imagePosition[2] = values[2];
            }
        }
        
        // Extract Image Orientation Patient (0020,0037) 
        gdcm::Attribute<0x0020, 0x0037> iop;
        iop.SetFromDataSet(ds);
        if (ds.FindDataElement(iop.GetTag())) {
            const double* values = iop.GetValues();
            if (iop.GetNumberOfValues() >= 6) {
                for (int i = 0; i < 6; ++i) {
                    slice.imageOrientation[i] = values[i];
                }
            }
        }
        
        // Extract Slice Location (0020,1041) - optional
        gdcm::Attribute<0x0020, 0x1041> sl;
        sl.SetFromDataSet(ds);
        if (ds.FindDataElement(sl.GetTag())) {
            slice.sliceLocation = sl.GetValue();
        }
        
        // Extract Instance Number (0020,0013)
        gdcm::Attribute<0x0020, 0x0013> instanceNum;
        instanceNum.SetFromDataSet(ds);
        if (ds.FindDataElement(instanceNum.GetTag())) {
            slice.instanceNumber = instanceNum.GetValue();
        }
        
        // Extract image dimensions
        const gdcm::Image& image = reader.GetImage();
        const unsigned int* dims = image.GetDimensions();
        slice.rows = dims[1];
        slice.columns = dims[0];
        
        // Extract pixel format information
        const gdcm::PixelFormat& pf = image.GetPixelFormat();
        slice.bitsAllocated = pf.GetBitsAllocated();
        slice.bitsStored = pf.GetBitsStored();
        slice.pixelRepresentation = pf.GetPixelRepresentation();
        
        // Extract rescale parameters (0028,1052) and (0028,1053)
        gdcm::Attribute<0x0028, 0x1052> rescaleIntercept;
        gdcm::Attribute<0x0028, 0x1053> rescaleSlope;
        
        rescaleIntercept.SetFromDataSet(ds);
        rescaleSlope.SetFromDataSet(ds);
        
        if (ds.FindDataElement(rescaleIntercept.GetTag()) && ds.FindDataElement(rescaleSlope.GetTag())) {
            slice.rescaleIntercept = rescaleIntercept.GetValue();
            slice.rescaleSlope = rescaleSlope.GetValue();
            slice.hasRescale = true;
        }
        
        // Extract Pixel Spacing (0028,0030)
        gdcm::Attribute<0x0028, 0x0030> pixelSpacing;
        pixelSpacing.SetFromDataSet(ds);
        if (ds.FindDataElement(pixelSpacing.GetTag())) {
            const double* values = pixelSpacing.GetValues();
            if (pixelSpacing.GetNumberOfValues() >= 2) {
                slice.pixelSpacing[0] = values[0]; // Row spacing
                slice.pixelSpacing[1] = values[1]; // Column spacing  
            }
        }
        
        return true;
    }
    catch (const std::exception& e) {
        std::cout << "Exception extracting slice info from " << filePath << ": " << e.what() << std::endl;
        return false;
    }
}

bool DicomSeriesLoader::validateSliceConsistency(const std::vector<SliceInfo>& slices)
{
    if (slices.empty()) {
        return false;
    }
    
    const SliceInfo& first = slices[0];
    
    for (size_t i = 1; i < slices.size(); ++i) {
        const SliceInfo& slice = slices[i];
        
        // Check dimensions
        if (slice.rows != first.rows || slice.columns != first.columns) {
            std::cout << "Slice dimension mismatch at slice " << i << std::endl;
            return false;
        }
        
        // Check pixel format
        if (slice.bitsAllocated != first.bitsAllocated ||
            slice.bitsStored != first.bitsStored ||
            slice.pixelRepresentation != first.pixelRepresentation) {
            std::cout << "Pixel format mismatch at slice " << i << std::endl;
            return false;
        }
        
        // Check orientation (should be very similar)
        for (int j = 0; j < 6; ++j) {
            if (std::abs(slice.imageOrientation[j] - first.imageOrientation[j]) > 1e-6) {
                std::cout << "Image orientation mismatch at slice " << i << std::endl;
                return false;
            }
        }
        
        // Check pixel spacing consistency
        if (std::abs(slice.pixelSpacing[0] - first.pixelSpacing[0]) > 1e-6 ||
            std::abs(slice.pixelSpacing[1] - first.pixelSpacing[1]) > 1e-6) {
            std::cout << "Pixel spacing mismatch at slice " << i << std::endl;
            return false;
        }
    }
    
    return true;
}

bool DicomSeriesLoader::sortSlices(std::vector<SliceInfo>& slices)
{
    if (slices.empty()) {
        return false;
    }
    
    // Compute slice direction from first slice orientation
    double sliceDirection[3];
    computeSliceDirection(slices[0].imageOrientation, sliceDirection);
    normalizeVector(sliceDirection);
    
    // Project each slice position onto the slice normal direction
    for (auto& slice : slices) {
        slice.projectedPosition = dotProduct(slice.imagePosition, sliceDirection);
    }
    
    // Sort by projected position (ascending)
    std::sort(slices.begin(), slices.end(), 
              [](const SliceInfo& a, const SliceInfo& b) {
                  return a.projectedPosition < b.projectedPosition;
              });
    
    // Assign final slice indices
    for (size_t i = 0; i < slices.size(); ++i) {
        slices[i].sliceIndex = static_cast<int>(i);
    }
    
    return true;
}

double DicomSeriesLoader::calculateSliceSpacing(const std::vector<SliceInfo>& slices)
{
    if (slices.size() < 2) {
        return 1.0; // Fallback spacing
    }
    
    // Calculate spacing between consecutive slices
    std::vector<double> spacings;
    spacings.reserve(slices.size() - 1);
    
    for (size_t i = 1; i < slices.size(); ++i) {
        double spacing = slices[i].projectedPosition - slices[i-1].projectedPosition;
        if (spacing > 1e-6) { // Avoid zero or negative spacings
            spacings.push_back(spacing);
        }
    }
    
    if (spacings.empty()) {
        return 1.0; // Fallback
    }
    
    // Use median spacing for robustness
    std::sort(spacings.begin(), spacings.end());
    size_t mid = spacings.size() / 2;
    
    double medianSpacing;
    if (spacings.size() % 2 == 0) {
        medianSpacing = (spacings[mid-1] + spacings[mid]) / 2.0;
    } else {
        medianSpacing = spacings[mid];
    }
    
    return medianSpacing > 1e-6 ? medianSpacing : 1.0;
}

bool DicomSeriesLoader::loadPixelData(const SliceInfo& slice, std::vector<float>& pixelData)
{
    try {
        gdcm::ImageReader reader;
        reader.SetFileName(slice.filePath.c_str());
        
        if (!reader.Read()) {
            return false;
        }
        
        const gdcm::Image& image = reader.GetImage();
        
        // Get raw pixel data
        std::vector<char> buffer(image.GetBufferLength());
        if (!image.GetBuffer(buffer.data())) {
            return false;
        }
        
        // Prepare output
        size_t numPixels = static_cast<size_t>(slice.rows) * slice.columns;
        pixelData.resize(numPixels);
        
        // Convert based on pixel format
        const gdcm::PixelFormat& pf = image.GetPixelFormat();
        
        if (pf.GetBitsAllocated() == 16) {
            if (pf.GetPixelRepresentation() == 0) {
                // Unsigned 16-bit
                const uint16_t* pixels = reinterpret_cast<const uint16_t*>(buffer.data());
                for (size_t i = 0; i < numPixels; ++i) {
                    float value = static_cast<float>(pixels[i]);
                    if (slice.hasRescale) {
                        value = static_cast<float>(slice.rescaleIntercept + slice.rescaleSlope * value);
                    }
                    pixelData[i] = value;
                }
            } else {
                // Signed 16-bit  
                const int16_t* pixels = reinterpret_cast<const int16_t*>(buffer.data());
                for (size_t i = 0; i < numPixels; ++i) {
                    float value = static_cast<float>(pixels[i]);
                    if (slice.hasRescale) {
                        value = static_cast<float>(slice.rescaleIntercept + slice.rescaleSlope * value);
                    }
                    pixelData[i] = value;
                }
            }
        } else if (pf.GetBitsAllocated() == 8) {
            if (pf.GetPixelRepresentation() == 0) {
                // Unsigned 8-bit
                const uint8_t* pixels = reinterpret_cast<const uint8_t*>(buffer.data());
                for (size_t i = 0; i < numPixels; ++i) {
                    float value = static_cast<float>(pixels[i]);
                    if (slice.hasRescale) {
                        value = static_cast<float>(slice.rescaleIntercept + slice.rescaleSlope * value);
                    }
                    pixelData[i] = value;
                }
            } else {
                // Signed 8-bit
                const int8_t* pixels = reinterpret_cast<const int8_t*>(buffer.data());
                for (size_t i = 0; i < numPixels; ++i) {
                    float value = static_cast<float>(pixels[i]);
                    if (slice.hasRescale) {
                        value = static_cast<float>(slice.rescaleIntercept + slice.rescaleSlope * value);
                    }
                    pixelData[i] = value;
                }
            }
        } else {
            std::cout << "Unsupported pixel format: " << pf.GetBitsAllocated() << " bits" << std::endl;
            return false;
        }
        
        return true;
    }
    catch (const std::exception& e) {
        std::cout << "Exception loading pixel data: " << e.what() << std::endl;
        return false;
    }
}

void DicomSeriesLoader::computeSliceDirection(const double imageOrientation[6], double sliceDirection[3])
{
    // Cross product of row and column directions
    // Row direction: imageOrientation[0:2]
    // Column direction: imageOrientation[3:5]
    
    sliceDirection[0] = imageOrientation[1] * imageOrientation[5] - imageOrientation[2] * imageOrientation[4];
    sliceDirection[1] = imageOrientation[2] * imageOrientation[3] - imageOrientation[0] * imageOrientation[5];
    sliceDirection[2] = imageOrientation[0] * imageOrientation[4] - imageOrientation[1] * imageOrientation[3];
}

void DicomSeriesLoader::normalizeVector(double direction[3])
{
    double length = std::sqrt(direction[0]*direction[0] + direction[1]*direction[1] + direction[2]*direction[2]);
    if (length > 1e-12) {
        direction[0] /= length;
        direction[1] /= length;
        direction[2] /= length;
    }
}

double DicomSeriesLoader::dotProduct(const double a[3], const double b[3])
{
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

void DicomSeriesLoader::copyVector(const double src[3], double dst[3])
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}