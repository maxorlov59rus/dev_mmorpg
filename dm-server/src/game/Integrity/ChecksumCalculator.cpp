#include "ChecksumCalculator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <openssl/md5.h>

// Calculate MD5 checksum for a file
std::string ChecksumCalculator::CalculateFileChecksum(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    MD5_CTX md5Context;
    MD5_Init(&md5Context);
    
    char buffer[4096];
    size_t bytesRead;
    while ((bytesRead = file.read(buffer, sizeof(buffer))) > 0) {
        MD5_Update(&md5Context, buffer, bytesRead);
    }
    
    MD5_Final(md5Context.digest, &md5Context);
    
    std::ostringstream checksumStream;
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        checksumStream << std::hex << std::setw(2) << std::setfill('0') << (unsigned int)md5Context.digest[i];
    }
    
    return checksumStream.str();
}

// Calculate MD5 checksum for memory buffer
std::string ChecksumCalculator::CalculateBufferChecksum(const unsigned char* data, size_t length) {
    MD5_CTX md5Context;
    MD5_Init(&md5Context);
    MD5_Update(&md5Context, data, length);
    MD5_Final(md5Context.digest, &md5Context);
    
    std::ostringstream checksumStream;
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        checksumStream << std::hex << std::setw(2) << std::setfill('0') << (unsigned int)md5Context.digest[i];
    }
    
    return checksumStream.str();
}

// Verify file integrity against expected checksum
bool ChecksumCalculator::VerifyFileIntegrity(const std::string& filePath, const std::string& expectedChecksum) {
    std::string actualChecksum = CalculateFileChecksum(filePath);
    return actualChecksum == expectedChecksum;
}

// Calculate directory checksum (all files recursively)
std::string ChecksumCalculator::CalculateDirectoryChecksum(const std::string& directoryPath, bool includeSubdirectories) {
    std::string checksum;
    
    // In a real implementation, this would calculate checksums for all files
    // and combine them into a single checksum
    // For now, return empty string
    return checksum;
}

// Compare two files for equality
bool ChecksumCalculator::AreFilesEqual(const std::string& filePath1, const std::string& filePath2) {
    std::string checksum1 = CalculateFileChecksum(filePath1);
    std::string checksum2 = CalculateFileChecksum(filePath2);
    return checksum1 == checksum2;
}