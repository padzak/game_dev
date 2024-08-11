/*
 * Button.cpp
 *
 *  Created on: Jan. 17, 2019
 *      Author: serge
 */

#include "Button.h"
#include "Screen.h"
#include <iostream>

Button::Button(const BitmapFont& bitmapFont, const Color& textColor, const Color& highlightColor)
	: mBitmapFont(bitmapFont)
	, mTitle("")
	, mTextColor(textColor)
	, mHighlightColor(highlightColor)
	, mAction(nullptr)
	, mHighlighted(false)
	, mTextScale(1.0f)
{

}

void Button::Init(const std::string& text, Vec2D topLeft, unsigned int width, unsigned height)
{
	mBBox = AARectangle(topLeft, width, height);

	mDrawPosition = mBitmapFont.GetDrawPosition(mTitle, mBBox, BFXA_CENTER, BFYA_CENTER);
}

void Button::Draw(Screen& theScreen)
{
	DrawTransform transform;
	transform.pos = mDrawPosition;

	ColorParams colorParams;
	colorParams.overlay = mTextColor;

	UVParams uvParams;
	
	theScreen.Draw(mBitmapFont, mTitle, transform, colorParams, uvParams);

	DrawHighlight(theScreen);
}

void Button::DrawHighlight(Screen& theScreen)
{
	if (mHighlighted)
	{
		theScreen.Draw(mBBox, mHighlightColor);
	}
}

void Button::ExecuteAction()
{
	mAction();
}


