#include "Tonemap.h"

Tonemap::Tonemap(const std::string& tmo_, float keyValue_, float burnPercentage_, float saturation_, float gamma_)
	: keyValue(keyValue_), burnPercentage(burnPercentage_), saturation(saturation_), gamma(gamma_)
{
	if (tmo_ == "Photographic")
	{
		tonemapOperator = TMO_PHOTOGRAPHIC;
	}
	else
	{
		tonemapOperator = TMO_NONE;
	}
}