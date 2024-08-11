#ifndef __POWER_UP_H__
#define __POWER_UP_H__

#include "Circle.h"
#include "AARectangle.h"
#include <functional>

class Vec2D;
class SpriteSheet;
class Screen;

class PowerUp
{
public:
	using PowerUpEffect = std::function<void(void)>;

	PowerUp();

	void Init(const SpriteSheet& spriteSheet, const std::string& spriteName, const Vec2D& initialPosition, const Vec2D& velocity, float lifetime, PowerUpEffect effect);

	bool Update(uint32_t dt);

	void Draw(Screen& theScreen);

	void ExecuteEffect();

	void SetPosition(const Vec2D& newPosition);

	inline bool IsActive() const { return mLifetime > 0; }
	inline Vec2D GetPosition() const { return mBoundingCircle.GetCenterPoint(); }
	inline const float GetWidth() const { return mBBox.GetWidth(); }
	inline const float GetHeight() const { return mBBox.GetHeight(); }
	inline const Circle& GetBoundingCircle()const { return mBoundingCircle; }
	inline const AARectangle& GetBoundingBox() const { return mBBox; }

private:
	const SpriteSheet* mnoptrSpriteSheet;
	std::string mSpriteName;
	AARectangle mBBox;
	Circle mBoundingCircle;
	Vec2D mVelocity;
	int mLifetime; //in milliseconds

	PowerUpEffect mEffect;
};

#endif // __POWER_UP_H__
