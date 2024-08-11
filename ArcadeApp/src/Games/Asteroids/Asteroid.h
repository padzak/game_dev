//
//  Asteroid.h
//  PacMan
//
//  Created by Serge Lansiquot on 2018-03-31.
//  Copyright (c) 2018 Serge Lansiquot. All rights reserved.
//

#ifndef __PacMan__Asteroid__
#define __PacMan__Asteroid__

#include <string>
#include "Circle.h"
#include "AnimatedSprite.h"

class Vec2D;
class Screen;
class SpriteSheet;




class Asteroid
{
public:

    enum Size
    {
        NONE = 0,
        LARGE,
        MEDIUM,
        SMALL,
        NUM_SIZES
    };


    Asteroid();
    void Init(const SpriteSheet& spriteSheet, const std::string& spriteName, Size size, const Vec2D& initialPos, const Vec2D& velocity, float rotationRate);
    void Update(unsigned int dt);
    void Draw(Screen& theScreen);
    
    void SetPosition(const Vec2D& position) {mBoundingCircle.MoveTo(position);}
    void SetVelocity(const Vec2D& vel) {mVelocity = vel;}
    
    Circle GetBoundingCircle()const {return mBoundingCircle;}
    Vec2D GetPosition() const {return mBoundingCircle.GetCenterPoint();}
    Vec2D GetVelocity() const { return mVelocity; }
    bool IsOutOfBounds(const Vec2D& largestSprite) const;

    inline Size GetSize() const { return mSize; }

    bool IsExploding() const { return mHit; }
    bool IsDestroyed() const { return mDestroyed; }
    void HitByAmmo(const std::vector<Vec2D>& intersections);

private:

    void DrawDebug(Screen& theScreen);
    void DrawAsteroidSplit(Screen& theScreen, float alpha);


    AnimatedSprite mSprite;
    std::string mSpriteName;
    Circle mBoundingCircle;
    Vec2D mVelocity;
    Vec2D mSpawnPosition;
    Size mSize;
    float mRotationRate;//in radians
    float mRotation;
    bool mHit;
    bool mDestroyed;

    bool mSplit;
    Vec2D mSplitPoint1; //in worldspace
    Vec2D mSplitPoint2; // in worldspace
    uint32_t mSplitTime;
    Vec2D mPosAtSplitTime;
    float mRotationAtSplit;

};

#endif /* defined(__PacMan__Asteroid__) */
