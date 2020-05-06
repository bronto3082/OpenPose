#include "EncodeDecoder.h"


int main(int argc, char* argv[]) {
	//std::string input = "C:\\Users\\USER\\Desktop\\video.avi";
	//std::string temppath = "C:\\Users\\USER\\Desktop\\testOutput";
	//std::string inputpath = "C:\\Users\\USER\\Desktop\\testOutput\\image%d.bmp";
	//std::string output = "C:\\Users\\USER\\Desktop\\output.avi";
	char* helo = argv[0];
	if (argc != 5) {
		printf("Usage: ./ffmpeg [input file path] [temp folder path] [modified image name] [output path]");
		exit(1);
	}
	std::string input = argv[1];
	std::string temppath = argv[2];
	std::string inputpath = temppath + "\\" + argv[3] + "%d.bmp";
	std::string output = argv[4];
	EncodeDecoder end(input, temppath, inputpath, output);
	end.Decode();
	end.Encode();
}