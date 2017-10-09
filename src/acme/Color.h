//
// Created by Dawid Drozd aka Gelldur on 14.05.17.
//
#pragma once

namespace Acme
{

namespace Color
{

/***
 * Color format is AARRGGBB in hex
 * eg. red = 0xffff0000
 * eg. green = 0xff00ff00
 * eg. blue = 0xff0000ff
 */
namespace ARGB
{

/**
 * @param color format is AARRGGBB
 * @return RR
 */
inline unsigned char getRed(const int colorARGB)
{
	return static_cast<unsigned char>((colorARGB & 0xFF0000) >> 16);
}

/**
 * @param color format is AARRGGBB
 * @return GG
 */
inline unsigned char getGreen(const int colorARGB)
{
	return static_cast<unsigned char>((colorARGB & 0x00FF00) >> 8);
}

/**
 * @param color format is AARRGGBB
 * @return BB
 */
inline unsigned char getBlue(const int colorARGB)
{
	return static_cast<unsigned char>(colorARGB & 0x0000FF);
}

/**
 * @param color format is AARRGGBB
 * @return AA
 */
inline unsigned char getAlpha(const int colorARGB)
{
	return static_cast<unsigned char>((colorARGB & 0xFF000000) >> 24);
}

inline int setAlpha(const int colorARGB, unsigned char alpha)
{
	return (colorARGB & 0x00FFFFFF) | (alpha << 24);
}

}

class convert
{
public:
	convert(const int color)
			: _color(color)
	{
	}

	template<typename T>
	operator T() const
	{
		return T{_color};
	}

	convert& opacity(const float value, int (* setAlphaFunction)(const int, unsigned char) = &ARGB::setAlpha)
	{
		_color = setAlphaFunction(_color, static_cast<unsigned char>(255 * value));
		return *this;
	}

private:
	int _color;
};

}
}
