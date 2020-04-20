#pragma once
#include "ffmpegEncoder.h"
#include "ffmpegDecoder.h"

class EncodeDecoder {
public:
	EncodeDecoder(std::string filePath, std::string tempPath, std::string inputPath, std::string outputPath) 
		: FILE_NAME(filePath), OUTPUT_FILE_PREFIX(tempPath + "\\image%d.bmp"), INPUT_FILE_PATH(inputPath), OUT_FILE_NAME(outputPath) {}

	void CreateFrame(char* buffer, int w, int h, int bytespan);

	bool Decode();

	bool Encode();
private:
	std::string FILE_NAME;
	std::string OUTPUT_FILE_PREFIX;
	std::string INPUT_FILE_PATH;
	std::string OUT_FILE_NAME;
	int FRAME_COUNT;
	int W_VIDEO;
	int H_VIDEO;
};