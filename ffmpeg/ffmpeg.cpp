extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

int main() {
	//Register all components of FFMPEG (Simply, initializing)
	avcodec_register_all();

	avformat_open_input(&pFormatCtx, )
}