#include "Image.h"
#include <opencv2/highgui/highgui.hpp>
#pragma warning(disable : 4996)

#define STB_IMAGE_IMPLEMENTATION
#include "stb-image/stb_image.h"

#define TINYEXR_IMPLEMENTATION
#include "tinyexr/tinyexr.h"

Image::Image(const std::string& imageName)
{
	readImage(imageName);
}

Image::Image(int width_, int height_, const std::vector<Vec3f>& pixelColors_)
	: width(width_), height(height_)
{
	pixelColors = pixelColors_;
}

Vec3f Image::getPixelColor(int i, int j)
{
	int rgbIdx = j * width + i;
	return pixelColors[rgbIdx];
}

void Image::writeImage(const std::string& imageName, const Tonemap& tonemap) const
{
	int pos = imageName.find_last_of(".");
	std::string extension = imageName.substr(pos + 1);
	std::string onlyImageName = imageName.substr(0, pos);

	if (extension == "exr")
	{
		// Write HDR image in EXR format without tonemapping.
		writeEXR(imageName);		
	}

	// Write LDR image in PNG format. Check for tonemapping.
	writePNG(onlyImageName + ".png", tonemap);
}

void Image::readImage(const std::string& imageName)
{
	int pos = imageName.find_last_of(".");
	std::string extension = imageName.substr(pos + 1);

	if (extension == "exr")
	{
		readEXR(imageName);
	}
	else
	{
		// PNG and JPEG
		readPNGorJPEG(imageName);		
	}
}

void Image::readEXR(const std::string& imageName)
{
	float *imageData;
	const char* err = NULL;

	int ret = LoadEXR(&imageData, &width, &height, imageName.c_str(), &err);
	if (ret != TINYEXR_SUCCESS)
	{
		if (err)
		{
			fprintf(stderr, "ERR : %s\n", err);
			FreeEXRErrorMessage(err); // release memory of error message.
		}
	}

	pixelColors = std::vector<Vec3f>(width * height, Vec3f());
	for (int i = 0; i < width * height; i++)
	{
		float r = imageData[4 * i];
		float g = imageData[4 * i + 1];
		float b = imageData[4 * i + 2];
		pixelColors[i] = Vec3f(r, g, b);
	}

	free(imageData); // release memory of image data
}

void Image::readPNGorJPEG(const std::string& imageName)
{
	int n;	// Number of channels
	unsigned char* imageData = stbi_load(imageName.c_str(), &width, &height, &n, 3);

	pixelColors = std::vector<Vec3f>(width * height, Vec3f());
	for (int i = 0; i < width * height; i++)
	{
		// Force n to be 3.
		float r = imageData[3 * i];
		float g = imageData[3 * i + 1];
		float b = imageData[3 * i + 2];
		pixelColors[i] = Vec3f(r, g, b);
	}

	free(imageData);
}

void Image::writePNG(const std::string& imageName, const Tonemap& tonemap) const
{
	unsigned char* imageData = applyTonemapping(tonemap);

	lodepng::encode(imageName.c_str(), imageData, width, height);
}

int Image::clampPixel(float val) const
{
	int clampedVal;
	if (val > 255.0f)
	{
		clampedVal = 255;
	}
	else
	{
		clampedVal = (int)val;
	}

	return clampedVal;
}

void Image::writeEXR(const std::string& imageName) const
{
	cv::Mat image = cv::Mat(height, width, CV_32FC3);

	int idx = 0;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			const Vec3f pixel = pixelColors[j * width + i];
			image.at<cv::Vec3f>(j, i)[0] = pixel.z;
			image.at<cv::Vec3f>(j, i)[1] = pixel.y;
			image.at<cv::Vec3f>(j, i)[2] = pixel.x;
		}
	}

	cv::imwrite(imageName, image);

	//EXRHeader header;
	//InitEXRHeader(&header);

	//EXRImage image;
	//InitEXRImage(&image);

	//image.num_channels = 3;

	//std::vector<float> images[3];
	//images[0].resize(width * height);
	//images[1].resize(width * height);
	//images[2].resize(width * height);

	//// Split RGBRGBRGB... into R, G and B layer
	//for (int i = 0; i < width * height; i++) {
	//	Vec3f pixelColor = pixelColors[i];
	//	images[0][i] = pixelColor.x; // R
	//	images[1][i] = pixelColor.y; // G
	//	images[2][i] = pixelColor.z; // B
	//}

	//float* image_ptr[3];
	//image_ptr[0] = &(images[2].at(0)); // B
	//image_ptr[1] = &(images[1].at(0)); // G
	//image_ptr[2] = &(images[0].at(0)); // R

	//image.images = (unsigned char**)image_ptr;
	//image.width = width;
	//image.height = height;

	//header.num_channels = 3;
	//header.channels = (EXRChannelInfo *)malloc(sizeof(EXRChannelInfo) * header.num_channels);
	//// Must be (A)BGR order, since most of EXR viewers expect this channel order.
	//strncpy(header.channels[0].name, "B", 255); header.channels[0].name[strlen("B")] = '\0';
	//strncpy(header.channels[1].name, "G", 255); header.channels[1].name[strlen("G")] = '\0';
	//strncpy(header.channels[2].name, "R", 255); header.channels[2].name[strlen("R")] = '\0';

	//header.pixel_types = (int *)malloc(sizeof(int) * header.num_channels);
	//header.requested_pixel_types = (int *)malloc(sizeof(int) * header.num_channels);
	//for (int i = 0; i < header.num_channels; i++)
	//{
	//	header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
	//	header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_HALF; // pixel type of output image to be stored in .EXR
	//}

	//const char* err = NULL;
	//int ret = SaveEXRImageToFile(&image, &header, imageName.c_str(), &err);
	//if (ret != TINYEXR_SUCCESS)
	//{
	//	fprintf(stderr, "Save EXR err: %s\n", err);
	//	FreeEXRErrorMessage(err); // free's buffer for an error message
	//}

	//free(header.channels);
	//free(header.pixel_types);
	//free(header.requested_pixel_types);
}

unsigned char* Image::applyTonemapping(const Tonemap& tonemap) const
{
	unsigned char* imageData = new unsigned char[width * height * 4];
	int rgbIdx = 0;

	if (tonemap.tonemapOperator == TMO_NONE)
	{
		// No tonemapping exists, just clamp the pixels.
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Vec3f pixelColor = pixelColors[i * width + j];

				imageData[rgbIdx++] = clampPixel(pixelColor.x);
				imageData[rgbIdx++] = clampPixel(pixelColor.y);
				imageData[rgbIdx++] = clampPixel(pixelColor.z);
				imageData[rgbIdx++] = 255;
			}
		}
	}
	else if (tonemap.tonemapOperator == TMO_PHOTOGRAPHIC)
	{
		// Apply photographic tonemapping.
		int n = width * height;
		std::vector<float> worldLuminances(n, 0.0f);
		std::vector<float> luminances(n, 0.0f);
		const float epsilon = 0.00001f;

		float sumLogLw = 0.0f;
		for (int i = 0; i < n; i++)
		{
			Vec3f rgb = pixelColors[i];
			worldLuminances[i] = 0.2126f * rgb.x + 0.7152f * rgb.y + 0.0722 * rgb.z;
			float logValue = log(epsilon + worldLuminances[i]);
			if (!isnan(logValue))
			{
				sumLogLw += logValue;
			}
		}
		
		float LwHat = exp((1.0f / n) * sumLogLw);

		for (int i = 0; i < n; i++)
		{
			luminances[i] = (tonemap.keyValue * worldLuminances[i]) / LwHat;
		}

		// Burnout
		float burnoutRatio = (100.0f - tonemap.burnPercentage) / 100.0f;
		int idxLwhite = (int)(n * burnoutRatio) - 1;
		if (idxLwhite < 0)
		{
			idxLwhite = 0;
		}

		// Sort pixels wrt luminance values and find Lwhite.
		std::vector<float> sortedLuminances(luminances);
		std::sort(sortedLuminances.begin(), sortedLuminances.end());
		float Lwhite = sortedLuminances[idxLwhite];

		std::vector<float> finalLuminances(n, 0.0f);
		for (int i = 0; i < n; i++)
		{
			float L = luminances[i];
			float Ld = (L * (1.0f + (L / (Lwhite * Lwhite)))) / (1.0f + L);
			finalLuminances[i] = Ld;
		}

		// Ld is in [0,1] range. Gamma-correct and scale to [0,255] range.
		for (int i = 0; i < n; i++)
		{
			Vec3f color = pixelColors[i];
			float r = finalLuminances[i] * pow((color.x / worldLuminances[i]), tonemap.saturation);
			float g = finalLuminances[i] * pow((color.y / worldLuminances[i]), tonemap.saturation);
			float b = finalLuminances[i] * pow((color.z / worldLuminances[i]), tonemap.saturation);

			imageData[rgbIdx++] = clampPixel(pow(r, 1.0f / tonemap.gamma) * 255.0f);
			imageData[rgbIdx++] = clampPixel(pow(g, 1.0f / tonemap.gamma) * 255.0f);
			imageData[rgbIdx++] = clampPixel(pow(b, 1.0f / tonemap.gamma) * 255.0f);
			imageData[rgbIdx++] = 255;
		}
	}

	return imageData;
}
