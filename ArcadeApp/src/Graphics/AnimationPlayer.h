/*
 * AnimationPlayer.h
 *
 *  Created on: Jan. 18, 2019
 *      Author: serge
 */

#ifndef GRAPHICS_ANIMATIONPLAYER_H_
#define GRAPHICS_ANIMATIONPLAYER_H_


#include "Animation.h"
#include <stdint.h>
#include <vector>
#include <string>

class AnimationPlayer
{
public:

	AnimationPlayer();
	bool Init(const std::string& animationsPath);
	bool Play(const std::string& animationName, bool looped);
	void Pause();
	void Stop();
	void Update(uint32_t dt);
	AnimationFrame GetCurrentAnimationFrame() const;

	inline bool IsFinishedPlaying() const {return mIsFinishedPlaying;}
	inline uint32_t GetCurrentFrameNumber() const {return mFrame;}
	const Animation& GetCurrentAnimation() const;
	inline bool IsPlaying() const { return mIsPlaying; }
	void PrintFrame();

	bool HasAnimation() const { return mHasAnimation; }
private:
	uint32_t mTime;
	std::vector<Animation> mAnimations;
	size_t mCurrentAnimation;
	uint32_t mFrame;

	bool mLooped;
	bool mIsPlaying;
	bool mIsFinishedPlaying;

	bool mHasAnimation;
};




#endif /* GRAPHICS_ANIMATIONPLAYER_H_ */
