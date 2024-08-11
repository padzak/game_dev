//
//  Asteroid.cpp
//  PacMan
//
//  Created by Serge Lansiquot on 2018-03-31.
//  Copyright (c) 2018 Serge Lansiquot. All rights reserved.
//

#include "Asteroid.h"
#include "SpriteSheet.h"
#include "Vec2D.h"
#include "Screen.h"
#include "Utils.h"
#include "Line2D.h"
#include "App.h"
#include "AsteroidsConstants.h"
#include <cassert>


namespace
{
    const float SPLIT_SPEED = 50; //px/sec
}

Asteroid::Asteroid() 
: mSpriteName ("")
, mBoundingCircle{}
, mVelocity{}
, mSpawnPosition{}
, mSize(Asteroid::NONE)
, mRotationRate(0)
, mRotation(0)
, mHit(false)
, mDestroyed(false)
, mSplit(false)
, mSplitPoint1(Vec2D::Zero)
, mSplitPoint2(Vec2D::Zero)
, mSplitTime(0)
{

}

void Asteroid::Init(const SpriteSheet& spriteSheet, const std::string& spriteName, Size size, const Vec2D& initialPos, const Vec2D& velocity, float rotationRate)
{
    
    mSpriteName = spriteName;
    mSprite.Init(App::Singleton().GetBasePath() + "assets/AsteroidsAnimations.txt", spriteSheet);

    mVelocity = velocity;
    const auto & sprite = spriteSheet.GetSprite(spriteName);
    mRotationRate = rotationRate;
    mBoundingCircle = Circle(initialPos, static_cast<float>(sprite.width) / 2.0f);
    mSpawnPosition = initialPos;
    mSize = size;

}

void Asteroid::Update(unsigned int dt)
{
    if (mDestroyed)
        return;

    if (mHit)
    {
        mSprite.Update(dt);

		if (mSplit)
		{
			mSplitTime += dt;
		}

        if (mSprite.IsFinishedPlayingAnimation())
        {
            mDestroyed = true;
            return;
        }
    }

    mBoundingCircle.MoveBy(mVelocity * MillisecondsToSeconds(dt));
    
    mRotation += mRotationRate * MillisecondsToSeconds(dt);
    
    if (mRotation >= TWO_PI)
    {
       mRotation -= TWO_PI;
    }
}

void Asteroid::Draw(Screen& theScreen)
{
    if (mDestroyed)
        return;

    if (!mHit)
    {
        const auto& sprite = mSprite.GetSpriteSheet()->GetSprite(mSpriteName);

        DrawTransform transform;
        transform.pos = mBoundingCircle.GetCenterPoint() - Vec2D(static_cast<float>(sprite.width) / 2.0f, static_cast<float>(sprite.height) / 2.0f);
        transform.rotationAngle = mRotation;
        transform.scale = 1.0f;

        ColorParams colorParams;
        colorParams.alpha = 1.0f;
        colorParams.gradient.xParam = GradientXParam::NO_X_GRADIENT;
        colorParams.gradient.yParam = GradientYParam::NO_Y_GRADIENT;
        colorParams.bilinearFiltering = BILINEAR_FILTERING;
        colorParams.overlay = Color::White();

        UVParams uvParams;

		theScreen.Draw(
            *mSprite.GetSpriteSheet(),
			mSpriteName,
			transform,
            colorParams,
            uvParams);
    }
    else
    {
        mSprite.Draw(theScreen, BILINEAR_FILTERING);
        
        if (mSplit)
        {
            float alpha = static_cast<float>(mSprite.GetTotalAnimationTimeInMS() - mSplitTime) / static_cast<float>(mSprite.GetTotalAnimationTimeInMS());

            DrawAsteroidSplit(theScreen, alpha);
        }
    }
}

bool Asteroid::IsOutOfBounds(const Vec2D& largestSprite) const
{
    if (GetPosition().GetX() > App::Singleton().Width() + largestSprite.GetX()
        || GetPosition().GetX() < -largestSprite.GetX()
        || GetPosition().GetY() < -largestSprite.GetY()
        || GetPosition().GetY() > App::Singleton().Height() + largestSprite.GetY())
    {
        return !IsEqual(GetPosition().GetX(), mSpawnPosition.GetX()) && !IsEqual(GetPosition().GetY(), mSpawnPosition.GetY());
    }
    return false;
}

void Asteroid::HitByAmmo(const std::vector<Vec2D>& intersections)
{
    assert(intersections.size() > 0 && "Don't call this if we haven't been hit by anything!");

    mHit = true;
    mSprite.SetAnimation("explosion", false);
    const AARectangle& bbox = mSprite.GetBoundingBox();
    mSprite.SetPosition(intersections[0] - Vec2D(bbox.GetWidth() / 2, bbox.GetHeight() / 2));

    if (intersections.size() > 1)
    {
		mSplit = true;
		mSplitPoint1 = intersections[0]; //in worldspace
		mSplitPoint2 = intersections[1]; // in worldspace
        mSplitTime = 0;

        const auto& sprite = mSprite.GetSpriteSheet()->GetSprite(mSpriteName);
        mPosAtSplitTime = mBoundingCircle.GetCenterPoint() - Vec2D(static_cast<float>(sprite.width) / 2.0f, static_cast<float>(sprite.height) / 2.0f);
        mRotationAtSplit = mRotation;
    }
}

void Asteroid::DrawAsteroidSplit(Screen& theScreen, float alpha)
{
	//Section 4 - Exercise 1
    //TODO: implement

    /*
        figure out how to get the mSplitPoint1 and mSplitPoint2 of the uvParams

        Helpful functions:
        ConvertWorldSpaceToUVSpace
        GetCounterClockwisePerpendicularUnitVector
    
    */

    /*
    You can draw like this:

    DrawTransform transform;
    transform.pos = //? - one side of the asteroid
    transform.rotationAngle = mRotationAtSplit;
    transform.scale = 1.0f;

	ColorParams colorParams;
	colorParams.bilinearFiltering = BILINEAR_FILTERING;
	colorParams.overlay = Color::White();
	colorParams.alpha = alpha;

    UVParams uvParams;
    uvParams.mSplitPoint1 = //?
    uvParams.mSplitPoint2 = //?
    uvParams.mOrientation = INSIDE

	theScreen.Draw(
		*mSprite.GetSpriteSheet(),
		mSpriteName,
		transform,
		colorParams,
		uvParams);

    transform.pos = //? - other side of the asteroid
    uvParams.mOrientation = OUTSIDE;

	theScreen.Draw(
		*mSprite.GetSpriteSheet(),
		mSpriteName,
		transform,
		colorParams,
		uvParams);
    
    */
}

void Asteroid::DrawDebug(Screen& theScreen)
{
	theScreen.Draw(mBoundingCircle, Color::White());
	theScreen.Draw(Line2D(GetPosition(), GetPosition() + mVelocity), Color::White());
}