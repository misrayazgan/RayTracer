#ifndef TONEMAP_H_
#define TONEMAP_H_

#include <string>

enum TMO
{
	TMO_NONE = 0,
	TMO_PHOTOGRAPHIC
};

class Tonemap
{
public:
	TMO tonemapOperator;
	float keyValue;
	float burnPercentage;
	float saturation;
	float gamma;

	Tonemap() : tonemapOperator(TMO_NONE) {}
	Tonemap(const std::string& tmo_, float keyValue_, float burnPercentage_, float saturation_, float gamma_);
};


#endif
