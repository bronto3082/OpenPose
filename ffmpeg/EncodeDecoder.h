#pragma once
#include "ffmpegEncoder.h"
#include "ffmpegDecoder.h"

class EncodeDecoder {
public:
	EncodeDecoder(std::string filePath, std::string tempPath, std::string outputPath) 
		: FILE_NAME(filePath), OUTPUT_FILE_PREFIX(tempPath + "\\image%d.bmp"), OUT_FILE_NAME(outputPath) {}

	bool Decode();

	bool Encode();
private:
	std::string FILE_NAME;
	std::string OUTPUT_FILE_PREFIX;
	std::string OUT_FILE_NAME;
};