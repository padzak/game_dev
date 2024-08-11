/*
 * AnimationPlayer.cpp
 *
 *  Created on: Jan. 18, 2019
 *      Author: serge
 */


#include "AnimationPlayer.h"
#include <cmath>
#include <cassert>

AnimationPlayer::AnimationPlayer():mTime(0), mCurrentAnimation(0), mFrame(0), mLooped(false), mIsPlaying(false), mIsFinishedPlaying(false), mHasAnimation(false)
{

}

bool AnimationPlayer::Init(const std::string& animationsPath)
{
	mAnimations = Animation::LoadAnimations(animationsPath);
	return mAnimations.size() > 0;
}

bool AnimationPlayer::Play(const std::string& animationName, bool looped)
{
	bool found = false;

	for(size_t i = 0; i < mAnimations.size(); ++i)
	{
		if(mAnimations[i].GetName() == animationName)
		{
			mCurrentAnimation = i;
			found = true;
			break;
		}
	}

	if(found)
	{
		mIsPlaying = true;
		mFrame = 0;
		mTime = 0;
		mLooped = looped;
		mIsFinishedPlaying = false;

		mHasAnimation = true;
	}

	return found;
}

void AnimationPlayer::Pause()
{
	if(mIsFinishedPlaying)
	{
		mIsPlaying = !mIsPlaying;
	}
}

void AnimationPlayer::Stop()
{
	mIsPlaying = false;
	mTime = 0;
	mFrame = 0;
	mIsFinishedPlaying = true;
}

void AnimationPlayer::Update(uint32_t dt)
{
	if(mIsPlaying)
	{
		
		if (mCurrentAnimation < mAnimations.size())
		{
			mTime += dt;

			int fps = mAnimations[mCurrentAnimation].FPS();

			float millisecondsPerFrame = static_cast<float>(1000) / static_cast<float>(fps);

			auto numFrames = mAnimations[mCurrentAnimation].NumFrames();
			uint32_t totalAnimationTime = static_cast<uint32_t>( millisecondsPerFrame * numFrames );

			auto result = mTime % totalAnimationTime;

			mFrame = static_cast<uint32_t>(float(result) / millisecondsPerFrame);
			//printf("ms per frame: %f, mTime: %u\n", millisecondsPerFrame, mTime);

			if (mTime >= totalAnimationTime && !mLooped)
			{
				mIsFinishedPlaying = true;
			}
		}
		
	}
}

AnimationFrame AnimationPlayer::GetCurrentAnimationFrame() const
{
	assert(mHasAnimation && "We need an animation for this to work!");

	if(mAnimations.empty())
	{
		return AnimationFrame();
	}

	return mAnimations[mCurrentAnimation].GetAnimationFrame(GetCurrentFrameNumber());
}



const Animation& AnimationPlayer::GetCurrentAnimation() const
{
	assert(mHasAnimation && "We need an animation for this to work!");
	return mAnimations[mCurrentAnimation];
}

void AnimationPlayer::PrintFrame()
{
	printf("Frame: %u\n", mFrame);
}
