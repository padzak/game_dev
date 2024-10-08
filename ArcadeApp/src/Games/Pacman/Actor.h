/*
 * Actor.h
 *
 *  Created on: Jan. 18, 2019
 *      Author: serge
 */

#ifndef GAMES_PACMAN_ACTOR_H_
#define GAMES_PACMAN_ACTOR_H_

#include "AnimatedSprite.h"
#include "Vec2D.h"
#include "PacmanGameUtils.h"
#include "AARectangle.h"
#include "Color.h"
#include <string>
#include <stdint.h>

class Screen;
class SpriteSheet;

class Actor
{
public:

	virtual ~Actor() {}
	virtual void Init(const SpriteSheet& spriteSheet, const std::string& animationsPath, const Vec2D& initialPos, uint32_t movementSpeed, bool updateSpriteOnMovement, const Color&spriteColor = Color::White());
	virtual void Update(uint32_t dt);
	virtual void Draw(Screen& screen, bool debug = false);

	virtual void Stop();
	const AARectangle& GetEatingBoundingBox() const;

	inline bool IsFinishedAnimation() const {return mSprite.IsFinishedPlayingAnimation();}
	inline const AARectangle& GetBoundingBox() const {return mSprite.GetBoundingBox();}
	inline void MoveBy(const Vec2D& delta) {mSprite.MoveBy(delta);}
	inline void MoveTo(const Vec2D& position) {mSprite.SetPosition(position);}
	inline Vec2D Position() const {return mSprite.Position();}
	inline PacmanMovement GetMovementDirection() const {return mMovementDirection;}
	virtual inline void SetMovementDirection(PacmanMovement direction) {mMovementDirection = direction;}
	inline const Color& GetSpriteColor() const {return mSprite.GetColor();}

protected:

	void SetAnimation(const std::string& animationName, bool looped);

	inline void ResetDelta() {mDelta = Vec2D::Zero;}
	inline void SetMovementSpeed(uint32_t movementSpeed) {mMovementSpeed = movementSpeed;}

	AnimatedSprite mSprite;
private:
	Vec2D mDelta;
	PacmanMovement mMovementDirection;
	uint32_t mMovementSpeed;
	bool mUpdateSpriteOnUpdate;
	mutable AARectangle mEatingBoundingBox;
};



#endif /* GAMES_PACMAN_ACTOR_H_ */
