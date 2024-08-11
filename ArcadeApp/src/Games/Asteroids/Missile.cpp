//
//  Missile.cpp
//  PacMan
//
//  Created by Serge Lansiquot on 2018-04-01.
//  Copyright (c) 2018 Serge Lansiquot. All rights reserved.
//

#include "Missile.h"
#include "AARectangle.h"
#include "Utils.h"
#include "Screen.h"
#include "Line2D.h"
#include <climits>
#include "SpriteSheet.h"
#include "AsteroidsConstants.h"
#include "App.h"

Missile::Missile(const SpriteSheet& spriteSheet, const Vec2D& initialPos, const Vec2D& vel, float yaw, float scale)
    :mVelocity(vel)
    ,mYaw(yaw)
    ,mScale(scale)
{
	mSprite.Init(App::Singleton().GetBasePath() + "assets/AsteroidsAnimations.txt", spriteSheet);
	mSprite.SetAnimation("missile", true);

	mBoundingCircle = Circle(initialPos, mSprite.GetBoundingBox().GetWidth() / 2 - 1);
	mBoundingCircle.MoveTo(initialPos);
}

void Missile::Update(uint32_t dt)
{
    mSprite.Update(dt);
    mBoundingCircle.MoveBy(mVelocity * MillisecondsToSeconds(dt));
}

void Missile::Draw(Screen& theScreen)
{
	const auto& sprite = mSprite.GetSpriteSheet()->GetSprite("missile_1");

	mSprite.SetAngle(mYaw);
	mSprite.SetPosition(mBoundingCircle.GetCenterPoint() - Vec2D(static_cast<float>(sprite.width) / 2.0f, static_cast<float>(sprite.height) / 2.0f) * mScale);
	mSprite.SetScale(mScale);
	mSprite.Draw(theScreen, BILINEAR_FILTERING);

   //     DrawDebug(theScreen);
    
}

ShipAmmo::Type Missile::GetType() const
{
    return ShipAmmo::Type::MISSILE;
}

bool Missile::IsOutOfBounds(int leftSide, int top, int screenWidth, int screenHeight) const
{
    Vec2D position = mBoundingCircle.GetCenterPoint();
    float width = GetWidth();

    return position.GetX() > screenWidth + width
        || position.GetX() < leftSide - width
        || position.GetY() < top - width
		|| position.GetY() > screenHeight + width;
}

bool Missile::Intersects(const Circle& circle, std::vector<Vec2D>& intersections) const
{
    bool intersects = mBoundingCircle.Intersects(circle);

    if (intersects)
    {
        intersections.push_back(mBoundingCircle.GetCenterPoint());
    }

    return intersects;
}

const Vec2D& Missile::GetVelocity() const
{
    return mVelocity;
}

float Missile::Scale() const
{
    return mScale;
}

bool Missile::DisappearsOnHit() const
{
    return true;
}

void Missile::DrawDebug(Screen& theScreen)
{
	theScreen.Draw(mBoundingCircle, Color::Red());
	theScreen.Draw(Line2D(mBoundingCircle.GetCenterPoint(), mVelocity + mBoundingCircle.GetCenterPoint()), Color::Red());
}

