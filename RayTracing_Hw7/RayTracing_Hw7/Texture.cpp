#include "Texture.h"

Texture::Texture(const std::string& decalMode_)
{
	decalMode = parseDecalMode(decalMode_);

	// Check if texture will be used as a normal map.
	// Replace normal option is only for image textures.
	isNormalMap = false;
	if (decalMode == DECALMODE_REPLACENORMAL)
	{
		isNormalMap = true;
	}

	// Check if texture will be used as a bump map.
	isBumpMap = false;
	if (decalMode == DECALMODE_BUMPNORMAL)
	{
		isBumpMap = true;
	}
}

DecalMode Texture::getDecalMode() const
{
	return decalMode;
}

DecalMode Texture::parseDecalMode(const std::string& str)
{
	DecalMode decalMode;
	if (str == "replace_kd")
	{
		decalMode = DECALMODE_REPLACEKD;
	}
	else if (str == "blend_kd")
	{
		decalMode = DECALMODE_BLENDKD;
	}
	else if (str == "replace_normal")
	{
		decalMode = DECALMODE_REPLACENORMAL;
	}
	else if (str == "replace_background")
	{
		decalMode = DECALMODE_REPLACEBACKGROUND;
	}
	else if (str == "replace_all")
	{
		decalMode = DECALMODE_REPLACEALL;
	}
	else if (str == "bump_normal")
	{
		decalMode = DECALMODE_BUMPNORMAL;
	}

	return decalMode;
}

