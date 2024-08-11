//
//  Missile.h
//  PacMan
//
//  Created by Serge Lansiquot on 2018-04-01.
//  Copyright (c) 2018 Serge Lansiquot. All rights reserved.
//

#ifndef __PacMan__Missile__
#define __PacMan__Missile__

#include "Circle.h"
#include "AnimatedSprite.h"
#include "ShipAmmo.h"

class Missile : public ShipAmmo
{
public:
    Missile(const SpriteSheet& spriteSheet, const Vec2D& initialPos, const Vec2D& vel, float yaw, float scale);

	virtual ShipAmmo::Type GetType() const override;
	virtual bool IsOutOfBounds(int leftSide, int top, int screenWidth, int screenHeight) const override;
	virtual bool Intersects(const Circle& circle, std::vector<Vec2D>& intersections) const override;
	virtual const Vec2D& GetVelocity() const override;
	virtual float Scale() const override;
	virtual void Update(uint32_t dt) override;
	virtual void Draw(Screen& theScreen) override;
    virtual bool DisappearsOnHit() const override;

    inline float GetWidth() const { return mBoundingCircle.GetRadius() * 2.0f; }

private:
    void DrawDebug(Screen& theScreen);
    AnimatedSprite mSprite;
    Circle mBoundingCircle;
    Vec2D mVelocity;
    float mYaw;
    float mScale;
};

#endif /* defined(__PacMan__Missile__) */
