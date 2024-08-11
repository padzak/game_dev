/*
 * AnimatedSprite.h
 *
 *  Created on: Jan. 18, 2019
 *      Author: serge
 */

#ifndef GRAPHICS_ANIMATEDSPRITE_H_
#define GRAPHICS_ANIMATEDSPRITE_H_

#include "AnimationPlayer.h"
#include "AARectangle.h"
#include "Vec2D.h"
#include "Color.h"
#include <stdint.h>
#include <string>

class Screen;
class SpriteSheet;

class AnimatedSprite
{
public:
	AnimatedSprite();
	void Init(const std::string& animationsPath, const SpriteSheet& spriteSheet, const Color& color = Color::White());
	void Update(uint32_t dt);
	void Draw(Screen& theScreen, bool bilinearFilter = false, bool debug = false);

	void SetAnimation(const std::string& animationName, bool looped);
	void Stop();
	const AARectangle& GetBoundingBox() const;

	inline Vec2D Position() const {return mPosition;}
	inline void SetPosition(const Vec2D& position) {mPosition = position;}
	inline void MoveBy(const Vec2D& delta) {mPosition += delta;}
	inline bool IsFinishedPlayingAnimation() const {return mAnimationPlayer.IsFinishedPlaying();}
	inline const Color& GetColor() const {return mColor;}
	const SpriteSheet* GetSpriteSheet() const {return mnoptrSpriteSheet;}
	void PrintFrame() { mAnimationPlayer.PrintFrame(); }
	inline Vec2D Size() const { return mSize; }
	inline float GetAngle() const { return mAngle; }
	inline void SetAngle(float angle) { mAngle = angle; }
	inline Vec2D GetCenterPos() const { return mBoundingBox.GetCenterPoint(); }

	inline void SetScale(float scale) { mScale = scale; }
	inline float GetScale() const { return mScale; }
	inline void SetAlpha(float alpha) { mAlpha = alpha; }
	inline float GetAlpha() const { return mAlpha; }

	uint32_t GetTotalAnimationTimeInMS() const;

private:
	const SpriteSheet * mnoptrSpriteSheet;
	AnimationPlayer mAnimationPlayer;
	Vec2D mPosition;
	Vec2D mSize;
	Color mColor;
	mutable AARectangle mBoundingBox;
	float mAngle;
	float mScale;
	float mAlpha;
};

#endif /* GRAPHICS_ANIMATEDSPRITE_H_ */
