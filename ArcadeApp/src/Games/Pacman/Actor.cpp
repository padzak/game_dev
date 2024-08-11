/*
 * Actor.cpp
 *
 *  Created on: Jan. 18, 2019
 *      Author: serge
 */


#include "Actor.h"
#include "Utils.h"
#include "Screen.h"
#include <cmath>


void Actor::Init(const SpriteSheet& spriteSheet, const std::string& animationsPath, const Vec2D& initialPos, uint32_t movementSpeed, bool updateSpriteOnMovement, const Color&spriteColor)
{
	mMovementDirection = PACMAN_MOVEMENT_NONE;
	mSprite.Init(animationsPath, spriteSheet, spriteColor);
	mUpdateSpriteOnUpdate = updateSpriteOnMovement;
	mMovementSpeed = movementSpeed;
	mDelta = Vec2D::Zero;
	mSprite.SetPosition(initialPos);
}

void Actor::Update(uint32_t dt)
{
	if(mMovementDirection != PACMAN_MOVEMENT_NONE)
	{
		Vec2D delta = Vec2D::Zero;

		delta = GetMovementVector(mMovementDirection) * static_cast<float>(mMovementSpeed);

		mDelta += delta * MillisecondsToSeconds(dt);

		if(fabsf(mDelta.GetX()) >= 1)
		{
			float dx = static_cast<float>(int(fabsf(mDelta.GetX())));

			if(mDelta.GetX() < 0)
			{
				mSprite.MoveBy(Vec2D(-dx, 0.0f));
				mDelta.SetX(mDelta.GetX() + dx);
			}
			else
			{
				mSprite.MoveBy(Vec2D(dx, 0.0f));
				mDelta.SetX(mDelta.GetX() - dx);
			}
		}
		else if(fabsf(mDelta.GetY()) >= 1)
		{
			float dy = static_cast<float>(int(fabsf(mDelta.GetY())));

			if(mDelta.GetY() < 0)
			{
				mSprite.MoveBy(Vec2D(0.f, -dy));
				mDelta.SetY(mDelta.GetY() + dy);
			}
			else
			{
				mSprite.MoveBy(Vec2D(0.f, dy));
				mDelta.SetY(mDelta.GetY() - dy);
			}
		}

		mSprite.Update(dt);
	}

	if(mUpdateSpriteOnUpdate && mMovementDirection == PACMAN_MOVEMENT_NONE)
	{
		mSprite.Update(dt);
	}
}

void Actor::Draw(Screen& screen, bool debug)
{
	mSprite.Draw(screen, debug);
	//screen.Draw(mSprite.GetBoundingBox(), Color::Red());
//	screen.Draw(mEatingBoundingBox, Color::Orange());
}

void Actor::Stop()
{
	SetMovementDirection(PACMAN_MOVEMENT_NONE);
	mSprite.Stop();
}

const AARectangle& Actor::GetEatingBoundingBox() const
{
	Vec2D inset = Vec2D(3, 3);
	Vec2D size = Vec2D(GetBoundingBox().GetWidth() - 2 * inset.GetX() - 1, GetBoundingBox().GetHeight() - 2 * inset.GetY() -1);
	mEatingBoundingBox.SetTopLeftPoint(GetBoundingBox().GetTopLeftPoint() + inset);
	mEatingBoundingBox.SetBottomRightPoint(mEatingBoundingBox.GetTopLeftPoint() + size);

	return mEatingBoundingBox;
}

void Actor::SetAnimation(const std::string& animationName, bool looped)
{
	mSprite.SetAnimation(animationName, looped);
}


