/*
 * AnimatedSprite.cpp
 *
 *  Created on: Jan. 18, 2019
 *      Author: serge
 */

#include "AnimatedSprite.h"
#include "AARectangle.h"
#include "Screen.h"

AnimatedSprite::AnimatedSprite():mPosition(Vec2D::Zero), mnoptrSpriteSheet(nullptr), mAngle(0.0f), mScale(1.0f), mAlpha(1.0f)
{

}

void AnimatedSprite::Init(const std::string& animationsPath, const SpriteSheet& spriteSheet, const Color& color)
{
	mAnimationPlayer.Init(animationsPath);
	mnoptrSpriteSheet = &spriteSheet;
	mColor = color;
}

void AnimatedSprite::Update(uint32_t dt)
{
	mAnimationPlayer.Update(dt);
}

void AnimatedSprite::Draw(Screen& theScreen, bool bilinearFilter, bool debug)
{
	AnimationFrame frame = mAnimationPlayer.GetCurrentAnimationFrame();

	Color frameColor = frame.frameColor;

	if(!frame.frameColorSet)
	{
		frameColor = mColor;
	}

	if (debug)
	{
		printf("frame: %s, frame number: %u\n", frame.frame.c_str(), mAnimationPlayer.GetCurrentFrameNumber());
	}

	DrawTransform transform;
	transform.pos = mPosition + frame.offset;
	transform.scale = mScale;
	transform.rotationAngle = mAngle;

	ColorParams colorParams;
	colorParams.alpha = mAlpha;
	colorParams.bilinearFiltering = bilinearFilter;
	colorParams.overlay = frameColor;
	colorParams.gradient.xParam = GradientXParam::NO_X_GRADIENT;
	colorParams.gradient.yParam = GradientYParam::NO_Y_GRADIENT;

	UVParams uvParams;

	theScreen.Draw(*mnoptrSpriteSheet, frame.frame, transform, colorParams, uvParams);

	if(frame.overlay.size() > 0)
	{
		transform.pos = mPosition;
		colorParams.overlay = frame.overlayColor;
		theScreen.Draw(*mnoptrSpriteSheet, frame.overlay, transform, colorParams, uvParams);
	}
}

void AnimatedSprite::SetAnimation(const std::string& animationName, bool looped)
{
	if (mAnimationPlayer.Play(animationName, looped))
	{
		mSize = mAnimationPlayer.GetCurrentAnimation().Size();
	}
}

void AnimatedSprite::Stop()
{
	mAnimationPlayer.Stop();
}

const AARectangle& AnimatedSprite::GetBoundingBox() const
{
	mBoundingBox.SetTopLeftPoint(mPosition);
	mBoundingBox.SetBottomRightPoint(Vec2D(mPosition.GetX() + mSize.GetX() - 1, mPosition.GetY() + mSize.GetY() - 1));

	return mBoundingBox;
}

uint32_t AnimatedSprite::GetTotalAnimationTimeInMS() const
{
	if (mAnimationPlayer.HasAnimation())
	{
		float msPerFrame = 1000.f / static_cast<float>(mAnimationPlayer.GetCurrentAnimation().FPS());
		return static_cast<uint32_t>(msPerFrame) * static_cast<uint32_t>(mAnimationPlayer.GetCurrentAnimation().NumFrames());
	}

	return 0;
}