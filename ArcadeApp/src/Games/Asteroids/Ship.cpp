//
//  Ship.cpp
//  PacMan
//
//  Created by Serge Lansiquot on 2018-03-31.
//  Copyright (c) 2018 Serge Lansiquot. All rights reserved.
//

#include "Ship.h"
#include "Utils.h"
#include "AARectangle.h"
#include "Screen.h"
#include "Line2D.h"
#include <algorithm>
#include "SpriteSheet.h"
#include "AsteroidsConstants.h"
#include "Easings.h"
#include "Missile.h"
#include "ShipLaser.h"
#include "App.h"

namespace
{
    const float ANGLE_PER_SECOND = PI;
    const float SHIP_SPEED = 80;
    const float MISSILE_SPEED = 150;
    const float LASER_SPEED = 800;
    const float GROW_SCALE = 1.5f;
    const uint32_t GROW_TRANSITION_TIME = 1000;
    const uint32_t STAY_BIG_SIZE_TIME = 7000 - GROW_TRANSITION_TIME*2; // in milliseconds
    const uint32_t LASER_USAGE_TIME = 10000; //in milliseconds
}

Ship::Ship():mShipYawMovement(SHIP_HEADING_NONE), mYaw(0), mSpeed(0), mAmmoIdGen(0), mHit(false), mDead(false), mCurrentAmmoType(ShipAmmo::Type::LASER), mGrowState(GrowState::NONE), mScaleTimer(0), mScale(1.0f), mUseLaserTimer(0)
{
    
}

void Ship::Init(const SpriteSheet& spriteSheet, const Vec2D& initialPos)
{
    mSprite.Init(App::Singleton().GetBasePath() + "assets/AsteroidsAnimations.txt", spriteSheet);
    
    const auto& sprite = mSprite.GetSpriteSheet()->GetSprite("space_ship");

    mBoundingCircle = Circle(initialPos, roundf(sprite.width/2.0f)-4);
}

void Ship::Update(unsigned int dt)
{
    if (mDead)
        return;

    if (mHit)
    {
        mSprite.Update(dt);
        
        if (mSprite.IsFinishedPlayingAnimation())
        {
            mHit = false;
            mDead = true;
        }
        
        return;
    }
    
    if (mShipYawMovement == SHIP_HEADING_TURN_LEFT)
    {
        mYaw -= MillisecondsToSeconds(dt) * ANGLE_PER_SECOND;
        
        if (mYaw < 0)
        {
            mYaw += TWO_PI;
        }
    }
    else if(mShipYawMovement == SHIP_HEADING_TURN_RIGHT) {
        mYaw += MillisecondsToSeconds(dt) * ANGLE_PER_SECOND;
        
        if (mYaw > TWO_PI)
        {
            mYaw -= TWO_PI;
        }
    }

    if (mUseLaserTimer > 0)
    {
        mUseLaserTimer -= dt;

        if (mUseLaserTimer <= 0)
        {
            mUseLaserTimer = 0;
        }
    }

    if (ALWAYS_USE_LASER)
    {
        mCurrentAmmoType = ShipAmmo::Type::LASER;
    }
    else
    {
        mCurrentAmmoType = ShipAmmo::Type::MISSILE;
    }

    if (mUseLaserTimer > 0)
    {
        mCurrentAmmoType = ShipAmmo::Type::LASER;
    }

    mScale = 1.0f;

    if (ALWAYS_BIG)
    {
        mGrowState = GrowState::STAY;
    }

    if (mGrowState == GrowState::GROW)
    {
        mScaleTimer += dt;

        if (mScaleTimer >= GROW_TRANSITION_TIME)
        {
            mScaleTimer = GROW_TRANSITION_TIME;
            mGrowState = GrowState::STAY;
        }

        float t = static_cast<float>(mScaleTimer) / static_cast<float>(GROW_TRANSITION_TIME);

        t = ease::EaseInBounce(t);

        mScale = Lerpf(1.0, GROW_SCALE, t);

        if (mGrowState == GrowState::STAY)
        {
            mScaleTimer = STAY_BIG_SIZE_TIME;
        }
    }
    else if (mGrowState == GrowState::STAY)
    {
        mScaleTimer -= dt;
        mScale = GROW_SCALE;

        if (mScaleTimer <= 0)
        {
            mGrowState = GrowState::REGRESS;
            mScaleTimer = 0;
        }
    }
    else if(mGrowState == GrowState::REGRESS)
    {
        mScaleTimer += dt;

		if (mScaleTimer >= GROW_TRANSITION_TIME)
		{
			mScaleTimer = GROW_TRANSITION_TIME;
			mGrowState = GrowState::NONE;
		}

        float t = static_cast<float>(mScaleTimer) / static_cast<float>(GROW_TRANSITION_TIME);

		t = ease::EaseOutBounce(t);

		mScale = Lerpf(GROW_SCALE, 1.0, t);

		if (mGrowState == GrowState::NONE)
		{
			mScaleTimer = 0;
		}
    }
    
    
    Vec2D heading = Vec2D(0, -1);
    heading = heading.RotationResult(mYaw);
    heading.Normalize();
    
    mBoundingCircle.MoveBy(mSpeed * heading * MillisecondsToSeconds(dt));
    
    if (mSpeed > 0)
    {
        mSprite.Update(dt);
    }
    
    for (auto& ammo : mAmmo)
    {
        ammo->Update(dt);
    }
}

void Ship::Draw(Screen& theScreen)
{
    if (mDead)
        return;

    if (!mHit)
    {
        for (auto& ammo: mAmmo)
        {
            ammo->Draw(theScreen);
        }
        
        const auto& sprite = mSprite.GetSpriteSheet()->GetSprite("space_ship");

        DrawTransform transform;
        transform.pos = mBoundingCircle.GetCenterPoint() - Vec2D(round(static_cast<float>(sprite.width) / 2.0f), round(static_cast<float>(sprite.height) / 2.0f)) * mScale;
        transform.scale = mScale;
        transform.rotationAngle = mYaw;

        ColorParams colorParams;
        colorParams.alpha = 1.0f;
        colorParams.bilinearFiltering = BILINEAR_FILTERING;
        colorParams.gradient.xParam = GradientXParam::NO_X_GRADIENT;
        colorParams.gradient.yParam = GradientYParam::NO_Y_GRADIENT;
        colorParams.overlay = Color::White();

        UVParams uvParams;

        theScreen.Draw(
            *mSprite.GetSpriteSheet(),
            "space_ship",
            transform,
            colorParams,
            uvParams);
        
        if (mSpeed > 0)
        {
            
            Vec2D heading = Vec2D(0, -1);
            heading = heading.RotationResult(mYaw);
            heading.Normalize();
           
            const auto& thrustersSprite = mSprite.GetSpriteSheet()->GetSprite("thrusters_1");
            
            mSprite.SetScale(mScale);
            mSprite.SetAngle(mYaw);
            mSprite.SetPosition((GetPosition() - (heading * ((static_cast<float>(sprite.height)*mScale) / 2.0f + (static_cast<float>(thrustersSprite.height)*mScale) / 2.0f + (BILINEAR_FILTERING ? 0 : -2 *mScale)))  - Vec2D((static_cast<float>(thrustersSprite.width)*mScale) / 2.0f, (static_cast<float>(thrustersSprite.height)* mScale) / 2.0f) ));
            mSprite.Draw(theScreen, BILINEAR_FILTERING);
        }
    }
    else
    {
        mSprite.SetPosition(mBoundingCircle.GetCenterPoint() - Vec2D(mSprite.GetBoundingBox().GetWidth()/2, mSprite.GetBoundingBox().GetHeight()/2));
        
        mSprite.Draw(theScreen, BILINEAR_FILTERING);
    }
    
  //  DrawDebug(theScreen);
}

void Ship::SetPosition(const Vec2D& pos)
{
    mBoundingCircle.MoveTo(pos);
}

void Ship::Respawn(const Vec2D& position)
{
    mDead = false;
    mAmmo.clear();
    mBoundingCircle.MoveTo(position);
    mShipYawMovement = SHIP_HEADING_NONE;
    mYaw = 0;
    mSpeed = 0;
    mGrowState = GrowState::NONE;
	mScale = 1.0f;
	mScaleTimer = 0;
	mUseLaserTimer = 0;
}    

size_t Ship::AmmoLeft() const
{
    return AMMO_CAPACITY - mAmmo.size();
}

void Ship::SwitchToLaser()
{
    mUseLaserTimer = LASER_USAGE_TIME;
}

void Ship::Grow()
{
    if (mGrowState != GrowState::STAY)
    {
		mGrowState = GrowState::GROW;
		mScaleTimer = 0;
    }
    else
    {
        ExtendGrow();
    }
}

bool Ship::IsBig()
{
    return mGrowState != GrowState::NONE;
}

void Ship::ExtendGrow()
{
    if (mGrowState == GrowState::STAY)
    {
        mScaleTimer = STAY_BIG_SIZE_TIME;
    }
}

void Ship::ResetGrowth()
{
    mGrowState = GrowState::NONE;
    mScaleTimer = 0;
    mScale = 1.0f;
}

void Ship::ResetAmmoType()
{
    mUseLaserTimer = 0;
}

void Ship::HitByAsteroid()
{
    if (!mHit && !mDead)
    {
		mHit = true;
		mSprite.SetAnimation("explosion", false);

        ResetGrowth();
        ResetAmmoType();
        mAmmo.clear();
    }
}

void Ship::UpdateYaw(AsteroidsShipYawMovement movement)
{
    mShipYawMovement = movement;
}

void Ship::EngageThrusters()
{
    if (!mHit && !ThrustersEngaged())
    {
        mSpeed = SHIP_SPEED;
        mSprite.SetAnimation("thrusters", true);
    }
}

void Ship::DisengageThrusters()
{
    mSpeed = 0;
}

void Ship::Fire()
{
    if (mAmmo.size() < AMMO_CAPACITY)
    {
        Vec2D heading = Vec2D(0, -1);
        heading = heading.RotationResult(mYaw);
        heading.Normalize();
        
        std::unique_ptr<ShipAmmo> newAmmo;

        if (mCurrentAmmoType == ShipAmmo::Type::MISSILE)
        {
            newAmmo = std::make_unique<Missile>(*mSprite.GetSpriteSheet(), mBoundingCircle.GetCenterPoint(), heading * MISSILE_SPEED, mYaw, mScale);   
        }
        else if (mCurrentAmmoType == ShipAmmo::Type::LASER)
        {
            newAmmo = std::make_unique<ShipLaser>(mBoundingCircle.GetCenterPoint(), heading * LASER_SPEED, Color::Yellow(), Color::Red(), IsBig());
        }

        newAmmo->SetID(mAmmoIdGen++);
        mAmmo.push_back(std::move(newAmmo));
    }
}

void Ship::RegainAmmo(size_t ammoID)
{
	auto iter = std::remove_if(mAmmo.begin(), mAmmo.end(), [&](const auto& m) {
		return ammoID == m->GetID();
		});

	mAmmo.erase(iter);
}

void Ship::DrawDebug(Screen& theScreen)
{
    Vec2D heading = Vec2D(0, -1);
    heading = heading.RotationResult(mYaw);
    heading.Normalize();
    theScreen.Draw(GetBoundingCircle(), Color::White());
    theScreen.Draw(Line2D(GetPosition(), 12*heading + GetPosition()), Color::White());
}


Circle Ship::GetBoundingCircle() const
{
    return Circle(mBoundingCircle.GetCenterPoint(), mBoundingCircle.GetRadius() * mScale);
}
