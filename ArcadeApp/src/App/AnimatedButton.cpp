#include "AnimatedButton.h"
#include "Utils.h"
#include "Screen.h"

namespace
{
	const int TOTAL_ANIMATION_TIME = 1000;
    const bool BOUNCE_FULL_TEXT = true; //Section 3 - Exercise 3 - set to false
}


AnimatedButton::AnimatedButton(const BitmapFont& bitmapFont, const Color& textColor, const Color& highlightColor /*= Color::White()*/)
	:Button(bitmapFont, textColor, highlightColor)
	, mOriginalColor(textColor)
	, mTotalAnimationTime(TOTAL_ANIMATION_TIME)
	, mAnimationTime(0)
	, mIsAnimating(false)
	, mExecuteTriggered(false)
{
}

void AnimatedButton::Init(const std::string& text, Vec2D topLeft, unsigned int width, unsigned height)
{
	Button::Init(text, topLeft, width, height);

    if(!BOUNCE_FULL_TEXT)
    {
        SetupLetterAnimationData(text);
    }
}

void AnimatedButton::Update(uint32_t dt)
{
	if (IsHighlighted())
	{
		mHighlightColor.SetAlpha( static_cast<uint8_t>(round(255.0 * Pulse(0.5))));
	}

	mDrawPosition = mBitmapFont.GetDrawPosition(mTitle, mBBox, BFXA_CENTER, BFYA_CENTER);
	mTextColor = mOriginalColor;

	if (mIsAnimating && IsHighlighted() && !mExecuteTriggered)
	{
        float t = UpdateAnimationTime(dt);
        
        if(BOUNCE_FULL_TEXT)
        {
            const Vec2D endPosition = mBitmapFont.GetDrawPosition(mTitle, mBBox, BFXA_CENTER, BFYA_CENTER);
            
            mDrawPosition = Vec2D::Lerp(
				Vec2D(mBBox.GetTopLeftPoint().GetX(), endPosition.GetY()),
				endPosition,
				t,
				ease::EaseOutBounce);
        }
        else
        {
            UpdateLetterAnimationData();
        }
	}

	if (mExecuteTriggered)
	{
		float t = UpdateAnimationTime(dt);

		if (IsGreaterThanOrEqual(t, 1.0f))
		{
			Button::ExecuteAction();
			mExecuteTriggered = false;
			return;
		}

		auto HighPulse = [](float) -> float
		{
			return Pulse(10);
		};

		mTextColor = Color::Lerp(mOriginalColor, Color::Orange(), t, HighPulse);
	}
}

void AnimatedButton::Draw(Screen& theScreen)
{
	if (!IsHighlighted() || mExecuteTriggered)
	{
		Button::Draw(theScreen);
	}
	else
	{
		DrawButtonAnimation(theScreen);
	}
}

void AnimatedButton::ExecuteAction()
{
	mExecuteTriggered = true;
	mIsAnimating = true;
	mAnimationTime = 0;
}

void AnimatedButton::SetHighlighted(bool highlighted)
{
	Button::SetHighlighted(highlighted);
	mIsAnimating = true;
	mAnimationTime = 0;
}


float AnimatedButton::UpdateAnimationTime(uint32_t dt, float totalAnimTimeMod)
{
	mAnimationTime += static_cast<int>(dt);

	int totalAnimTime = static_cast<int>(mTotalAnimationTime * totalAnimTimeMod);

	if (mAnimationTime >= totalAnimTime)
	{
		mAnimationTime = totalAnimTime;
		mIsAnimating = false;
	}

	float elapsedSeconds = MillisecondsToSeconds(mAnimationTime);
	float totalSeconds = MillisecondsToSeconds(totalAnimTime);

	return elapsedSeconds / totalSeconds;
}

void AnimatedButton::SetupLetterAnimationData(const std::string& text)
{
	//Section 3 - Exercise 3 
	//TODO: implement
	/*
	* HINTS:
		std::string letter(1, text[i]); to make a 1 character string
		mBitmapFont.GetFontSpacingBetweenLetters() which is the amount of pixels between each letter in our font
		and mBitmapFont.GetSizeOf(letter) which gives how tall and wide each letter is in our font.
	*/
}

void AnimatedButton::UpdateLetterAnimationData()
{
	//Section 3 - Exercise 3 
	//TODO: implement
}

void AnimatedButton::DrawButtonAnimation(Screen& theScreen)
{
	DrawTransform transform;
	transform.pos = mDrawPosition;

	ColorParams colorParams;
	colorParams.overlay = mTextColor;


	//Section 3 - Exercise 2 - Change the xParam and yParam of the gradient to each different combination
	colorParams.gradient.xParam = LEFT_TO_RIGHT;
	colorParams.gradient.yParam = BOTTOM_TO_TOP;
	colorParams.gradient.color1 = Color::Green();
	colorParams.gradient.color2 = Color::Red();
	

	UVParams uvParams;

	DrawHighlight(theScreen);
    
    if(BOUNCE_FULL_TEXT)
    {
        theScreen.Draw(mBitmapFont, mTitle, transform, colorParams, uvParams);
    }
    else
    {
		//Section 3 - Exercise 3 
		//TODO: implement
    }
}
