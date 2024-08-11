#include "PowerUp.h"
#include "Vec2D.h"
#include "Utils.h"
#include "SpriteSheet.h"
#include "Screen.h"
#include "AsteroidsConstants.h"


namespace
{
	const float PULSE_TIME = 1000.f; //1 second in milliseconds
}

PowerUp::PowerUp()
	: mnoptrSpriteSheet(nullptr)
	, mSpriteName("")
	, mBBox()
	, mBoundingCircle()
	, mVelocity(Vec2D::Zero)
	, mLifetime(0)
	, mEffect(nullptr)
{
}

void PowerUp::Init(const SpriteSheet& spriteSheet, const std::string& spriteName, const Vec2D& initialPosition, const Vec2D& velocity, float lifetime, PowerUpEffect effect)
{
	mnoptrSpriteSheet = &spriteSheet;
	mSpriteName = spriteName;
	const auto& sprite = spriteSheet.GetSprite(spriteName);
	float spriteWidth = static_cast<float>(sprite.width);
	float spriteHeight = static_cast<float>(sprite.height);

	mBoundingCircle = Circle(initialPosition, spriteWidth / 2.0f);
	mVelocity = velocity;
	mLifetime = SecondsToMilliseconds(lifetime);
	mEffect = effect;

	mBBox = AARectangle(Vec2D(initialPosition.GetX() - spriteWidth /2.0f, initialPosition.GetY() - spriteHeight/2.0f), static_cast<uint32_t>(spriteWidth), static_cast<uint32_t>(spriteHeight));
}

bool PowerUp::Update(uint32_t dt)
{
	if (mLifetime > 0)
	{
		mLifetime -= dt;

		if (mLifetime <= 0)
		{
			mLifetime = 0;
		}

		mBoundingCircle.MoveBy(mVelocity * MillisecondsToSeconds(dt));
		mBBox.MoveBy(mVelocity * MillisecondsToSeconds(dt));
	}

	return IsActive();
}

void PowerUp::Draw(Screen& theScreen)
{
	if (IsActive())
	{
		float alpha = 1.0f;
		if (IsLessThanOrEqual(static_cast<float>(mLifetime), PULSE_TIME))
		{
			alpha = Pulse(5.0f);
		}

		const auto& sprite = mnoptrSpriteSheet->GetSprite(mSpriteName);

		DrawTransform transform;
		transform.pos = mBoundingCircle.GetCenterPoint() - Vec2D(roundf(static_cast<float>(sprite.width / 2)), roundf(static_cast<float>(sprite.height / 2)));
		transform.scale = 1.0f;
		transform.rotationAngle = 0.0f;

		ColorParams colorParams;
		colorParams.alpha = alpha;
		colorParams.bilinearFiltering = BILINEAR_FILTERING;
		colorParams.gradient.xParam = GradientXParam::NO_X_GRADIENT;
		colorParams.gradient.yParam = GradientYParam::NO_Y_GRADIENT;
		colorParams.overlay = Color::White();

		UVParams uvParams;

		theScreen.Draw(*mnoptrSpriteSheet, mSpriteName, transform, colorParams, uvParams);
	}

}

void PowerUp::ExecuteEffect()
{
	mEffect();
	mLifetime = 0;
}

void PowerUp::SetPosition(const Vec2D& newPosition)
{
	mBoundingCircle.MoveTo(newPosition);
	mBBox.MoveTo(Vec2D(newPosition.GetX() - mBoundingCircle.GetRadius(), newPosition.GetY() - mBoundingCircle.GetRadius()));
}