#include "EncodeDecoder.h"

int main() {
	std::string input = "C:\\Users\\USER\\Desktop\\video.avi";
	std::string temppath = "C:\\Users\\USER\\Desktop\\testOutput";
	std::string output = "C:\\Users\\USER\\Desktop\\output.avi";
	EncodeDecoder end(input, temppath, output);

}