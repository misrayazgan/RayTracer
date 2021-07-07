#ifndef IMAGE_H_
#define IMAGE_H_

#include "Vec3f.h"
#include "Tonemap.h"
#include "lodepng\lodepng.h"

class Image
{
public:
	int width;
	int height;

	Image() {}
	Image(const std::string& imageName);
	Image(int width_, int height_, const std::vector<Vec3f>& pixelColors_);
	Vec3f getPixelColor(int i, int j);
	void writeImage(const std::string& imageName, const Tonemap& tonemap) const;
	void readImage(const std::string& imageName);
	
private:
	std::vector<Vec3f> pixelColors;

	int clampPixel(float val) const;
	void writePNG(const std::string& imageName, const Tonemap& tonemap) const;
	void writeEXR(const std::string& imageName) const;
	void readEXR(const std::string& imageName);
	void readPNGorJPEG(const std::string& imageName);
	unsigned char* applyTonemapping(const Tonemap& tonemap) const;
};



#endif
