/*
 * BitmapFont.cpp
 *
 *  Created on: Jan. 17, 2019
 *      Author: serge
 */

#include "BitmapFont.h"
#include "AARectangle.h"
#include "Vec2D.h"
#include <iostream>

bool BitmapFont::Load(const std::string& name)
{
	return mFontSheet.Load(name);
}

Size BitmapFont::GetSizeOf(const std::string & str) const
{
	Size textSize;

	size_t length = str.length();

	int i = 0;

	for(char c : str)
	{
		if(c == ' ')
		{
			textSize.width += GetFontSpacingBetweenWords();

			continue;
		}

		Sprite sprite = mFontSheet.GetSprite(std::string("") + c);
		textSize.height = textSize.height < sprite.height ? sprite.height : textSize.height;
		textSize.width += sprite.width;

		if(i + 1 < length)
		{
			textSize.width += GetFontSpacingBetweenLetters();
		}
	}

	return textSize;
}

Vec2D BitmapFont::GetDrawPosition(const std::string& str, const AARectangle& box, BitmapFontXAlignment xAlign, BitmapFontYAlignment yAlign) const
{
	Size textSize = GetSizeOf(str);

	float textSizeWidth = static_cast<float>(textSize.width);
	float textSizeHeight = static_cast<float>(textSize.height);

	float x = 0.0f;
	float y = 0.0f;

	if(xAlign == BFXA_CENTER)
	{
		x = box.GetWidth() / 2.0f - textSizeWidth / 2.0f;
	}
	else if(xAlign == BFXA_RIGHT)
	{
		x = box.GetWidth() - textSizeWidth;
	}

	x += box.GetTopLeftPoint().GetX();

	if(yAlign == BFYA_CENTER)
	{
		y = (box.GetHeight()/2.0f) - textSizeHeight/2.0f;
	}
	else if(yAlign == BFYA_BOTTOM)
	{
		y = box.GetHeight() - textSizeHeight;
	}

	y += box.GetTopLeftPoint().GetY();

	return Vec2D(x, y);

}

