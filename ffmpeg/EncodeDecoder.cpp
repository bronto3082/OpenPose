#pragma warning(disable:4996)
#include "EncodeDecoder.h"
#include <windows.h>

bool BMPSave(const char* pFileName, AVFrame* frame, int w, int h)
{
	bool bResult = false;

	if (frame)
	{
		FILE* file = fopen(pFileName, "wb");

		if (file)
		{
			// RGB image
			int imageSizeInBytes = 3 * w * h;
			int headersSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
			int fileSize = headersSize + imageSizeInBytes;

			uint8_t* pData = new uint8_t[headersSize];

			if (pData != NULL)
			{
				BITMAPFILEHEADER& bfHeader = *((BITMAPFILEHEADER*)(pData));

				bfHeader.bfType = 'MB';
				bfHeader.bfSize = fileSize;
				bfHeader.bfOffBits = headersSize;
				bfHeader.bfReserved1 = bfHeader.bfReserved2 = 0;

				BITMAPINFOHEADER& bmiHeader = *((BITMAPINFOHEADER*)(pData + headersSize - sizeof(BITMAPINFOHEADER)));

				bmiHeader.biBitCount = 3 * 8;
				bmiHeader.biWidth = w;
				bmiHeader.biHeight = h;
				bmiHeader.biPlanes = 1;
				bmiHeader.biSize = sizeof(bmiHeader);
				bmiHeader.biCompression = BI_RGB;
				bmiHeader.biClrImportant = bmiHeader.biClrUsed =
					bmiHeader.biSizeImage = bmiHeader.biXPelsPerMeter =
					bmiHeader.biYPelsPerMeter = 0;

				fwrite(pData, headersSize, 1, file);

				uint8_t* pBits = frame->data[0] + frame->linesize[0] * h - frame->linesize[0];
				int nSpan = -frame->linesize[0];

				int numberOfBytesToWrite = 3 * w;

				for (size_t i = 0; i < h; ++i, pBits += nSpan)
				{
					fwrite(pBits, numberOfBytesToWrite, 1, file);
				}

				bResult = true;
				delete[] pData;
			}

			fclose(file);
		}
	}

	return bResult;
}

static int inputCount = 0;

unsigned char* readBMP(const char* filename)
{
	int i;
	FILE* f = fopen(filename, "rb");
	unsigned char info[54];

	// read the 54-byte header
	fread(info, sizeof(unsigned char), 54, f);

	// extract image height and width from header
	int width = *(int*)&info[18];
	int height = *(int*)&info[22];

	// allocate 3 bytes per pixel
	int size = 3 * width * height;
	unsigned char* data = new unsigned char[size];

	// read the rest of the data at once
	fread(data, sizeof(unsigned char), size, f);
	fclose(f);

	return data;
}

void EncodeDecoder::CreateFrame(char* buffer, int w, int h, int bytespan)
{
	char filename[MAX_PATH];
	sprintf(filename, INPUT_FILE_PATH.c_str(), inputCount++);
	unsigned char* readResult = readBMP(filename);
	int wxh = w * h;
	static float seed = 1.0;
	int temp = 0;
	
	for (int i = h - 1; i >= 0; i--)
	{
		char* line = buffer + (h - 1 - i) * bytespan;
		for (int j = 0; j < w; j++)
		{
			// RGB
			line[0] = readResult[3 * (i * w + j) + 2];
			line[1] = readResult[3 * (i * w + j) + 1];
			line[2] = readResult[3 * (i * w + j) ];
			line += 3;
		}
	}
	
	seed = seed + 2.2;
}

// Create sample
void CreateSample(short* buffer, int sampleCount)
{
	static float shift = 0.0;
	static float seconds = 0.0;

	const float  minNu = 3.14 / (44100.0f) * 700.0f;
	const float  maxNu = 3.14 / (44100.0f) * 1500.0f;
	const float  speedNu = 3.14 / (44100.0f) * 10.0f;

	static float varNu = minNu;

	if (varNu > maxNu)
	{
		varNu = minNu;
	}

	varNu += speedNu;

	for (int i = 0; i < sampleCount; i++)
	{
		seconds += 1.0f / 44100.0f;

		buffer[i] = sin(i * varNu + shift) * 0x4FFF;
	}
	shift = shift + varNu * sampleCount;
}

bool EncodeDecoder::Decode()
{
	ffmpegDecoder decoder;

	if (decoder.OpenFile(FILE_NAME))
	{
		int w = decoder.GetWidth();
		int h = decoder.GetHeight();
		W_VIDEO = w;
		H_VIDEO = h;
		int count = 0;
		bool stop = false;
		while (!stop)
		{
			AVFrame* frame = decoder.GetNextFrame();
			if (frame)
			{
				char filename[MAX_PATH];
				sprintf(filename, OUTPUT_FILE_PREFIX.c_str(), count++);
				if (!BMPSave(filename, frame, frame->width, frame->height))
				{
					printf("Cannot save file %s\n", filename);
				}
				av_free(frame->data[0]);
				av_free(frame);
			}
			else {
				stop = true;
			}
		}
		FRAME_COUNT = count;
		decoder.CloseFile();
	}
	else
	{
		printf("Cannot open file %s \n", FILE_NAME.c_str());
		return false;
	}
	FPS = decoder.GetFPS();
	return true;
}

bool EncodeDecoder::Encode() {
	ffmpegEncoder encoder(W_VIDEO, H_VIDEO, FRAME_COUNT, FPS);
	std::string container = "auto";
	if (encoder.InitFile(OUT_FILE_NAME, container))
	{
		int w = W_VIDEO;
		int h = H_VIDEO;
		AVFrame* frame = av_frame_alloc();
		int nSampleSize = 2 * 44100.0f / 25.0f; // 1 / 25 sec * FORMAT SIZE(S16)
		char* sample = new char[nSampleSize];
		// Create frame
		int bufferImgSize = av_image_get_buffer_size(AV_PIX_FMT_BGR24, w, h, 1);
		uint8_t* buffer = (uint8_t*)av_mallocz(bufferImgSize);
		av_image_fill_arrays(((AVPicture*)frame)->data, ((AVPicture*)frame)->linesize, buffer, AV_PIX_FMT_BGR24, w, h, 1);
		for (int i = 0; i < FRAME_COUNT; i++)
		{
			CreateFrame((char*)frame->data[0], w,h, frame->linesize[0]);
			CreateSample((short*)sample, nSampleSize / 2);
			if (!encoder.AddFrame(frame, sample, nSampleSize))
			{
				printf("Cannot write frame\n");
			}
			printf("%d", i);
		}

		encoder.Finish();
		av_free(frame->data[0]);
		av_free(frame);
		delete[] sample;
		sample = NULL;
	}
	else
	{
		printf("Cannot open file %s \n", OUT_FILE_NAME.c_str());
		return false;
	}

	return true;
}