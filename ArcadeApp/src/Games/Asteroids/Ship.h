//
//  Ship.h
//  PacMan
//
//  Created by Serge Lansiquot on 2018-03-31.
//  Copyright (c) 2018 Serge Lansiquot. All rights reserved.
//

#ifndef __PacMan__Ship__
#define __PacMan__Ship__

#include "Vec2D.h"
#include "AnimatedSprite.h"
#include "Circle.h"
#include "ShipAmmo.h"
#include <memory>
#include <vector>

enum AsteroidsShipYawMovement
{
    SHIP_HEADING_NONE = 0,
    SHIP_HEADING_TURN_LEFT,
    SHIP_HEADING_TURN_RIGHT
};

class Screen;

class Ship
{
public:
    


    Ship();
    void Init(const SpriteSheet& spriteSheet, const Vec2D& initialPos);
    void Update(unsigned int dt);
    void Draw(Screen& theScreen);
    void UpdateYaw(AsteroidsShipYawMovement movement);
    void EngageThrusters();
    void DisengageThrusters();

    void Fire();
    void RegainAmmo(size_t ammoInstanceID);
    
    void HitByAsteroid();
    float GetWidth() const { return mBoundingCircle.GetRadius() * 2; }
    float GetHeight() const { return GetWidth(); }
    bool ThrustersEngaged() const {return mSpeed > 0;}

    void SetPosition(const Vec2D& pos);

    Vec2D GetPosition() const {return mBoundingCircle.GetCenterPoint();}
    
    Circle GetBoundingCircle()const;
    
    const std::vector<std::unique_ptr<ShipAmmo>>& GetAmmoInFlight()const {return mAmmo;}

    bool IsDestroyed() const { return mDead; }

    void Respawn(const Vec2D& position);

    size_t AmmoLeft() const;

    //Power up methods

    void SwitchToLaser();
    void Grow();

    bool IsBig();
    void ExtendGrow();

private:
    
    void DrawDebug(Screen& theScreen);
    
    void ResetGrowth();
    void ResetAmmoType();

    AsteroidsShipYawMovement mShipYawMovement;
    float mYaw; //radians
    AnimatedSprite mSprite;
    std::vector<std::unique_ptr<ShipAmmo>> mAmmo;
    float mSpeed;
    Circle mBoundingCircle;

    static const size_t AMMO_CAPACITY = 10;
    size_t mAmmoIdGen;
	bool mHit;
	bool mDead;
    ShipAmmo::Type mCurrentAmmoType;

    enum class GrowState
    {
        NONE,
        GROW,
        STAY,
        REGRESS
    };

    //Power Ups
    float mScale;
    GrowState mGrowState;
    int mScaleTimer;

    
    int mUseLaserTimer;
    
};

#endif /* defined(__PacMan__Ship__) */
