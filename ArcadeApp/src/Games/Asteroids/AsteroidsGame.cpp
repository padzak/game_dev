//
//  AsteroidsGame.cpp
//  PacMan
//
//  Created by Serge Lansiquot on 2018-03-31.
//  Copyright (c) 2018 Serge Lansiquot. All rights reserved.
//

#include "AsteroidsGame.h"
#include "App.h"
#include "AsteroidsConstants.h"
#include "GameController.h"
#include "Utils.h"
#include <SDL2/SDL.h>
#include <cassert>


namespace {
    const size_t STARTING_NUM_ASTEROIDS = 16;
    const unsigned int NUM_LIVES = 3;
    const int SCORE_VAL = 50;
    const int LEVEL_START_TIME = 5000; //in milliseconds

    const int READY_TIME_TRANSITION_THRESHOLD = 5000; //in milliseconds
    const int READY_TIME_DISPLAY_THRESHOLD = 3000; //in milliseconds
    const int GO_TIME_TRANSITION_THRESHOLD = 2000; //in milliseconds
    const int GO_TIME_DISPLAY_THRESHOLD = 1000; //in milliseconds

    const std::string GAME_OVER_STR = "Game Over";
    const std::string READY_STR = "Ready?";
    const std::string GO_STR = "GO!";
    const std::string SCORE_STR = "Score";

    const std::string SPACE_SHIP_SPRITE_NAME = "space_ship";
    const std::string MISSILE_SPRITE_NAME = "missile_1";
    std::vector<std::string> asteroidNames = {  "big_rock", "medium_rock", "small_rock" };

    const float POWER_UP_LIFE_TIME = 5.f; //in seconds
    const float POWER_UP_SPEED = 5.0f;
    
    const float SCREEN_SHAKE_POWER = 10.0f;
    const float SCREEN_SHAKE_TIME = 0.3f; //in seconds
}

AsteroidsGame::AsteroidsGame()
{
    std::random_device r;
    mRandomGenerator.seed(r());
}

void AsteroidsGame::Init(GameController& controller)
{
    mAsteroidsSprites.Load("AsteroidsSprites");
    
    bool loaded = mBGImage.Load(App::Singleton().GetBasePath() + std::string("assets/") + "StarrySpace.bmp");
    
    assert(loaded);

    mShip.Init(mAsteroidsSprites, Vec2D(static_cast<float>(App::Singleton().Width()) /2.0f, static_cast<float>(App::Singleton().Height()) /2.0f));
    
    mStringRect = AARectangle(Vec2D(0.0f, static_cast<float>(App::Singleton().Height())/2.0f - 50.0f), App::Singleton().Width(), 100);

    ResetGame();
    
	ButtonAction leftKeyAction;
	leftKeyAction.key = GameController::LeftKey();
    leftKeyAction.action = [this](uint32_t dt, InputState state)
    {
        if (GameController::IsPressed(state))
        {
            mShip.UpdateYaw(SHIP_HEADING_TURN_LEFT);
        }
        else
        {
            mShip.UpdateYaw(SHIP_HEADING_NONE);
        }
    };

    controller.AddInputActionForKey(leftKeyAction);

    ButtonAction rightKeyAction;
    rightKeyAction.key = GameController::RightKey();
    rightKeyAction.action = [this](uint32_t dt, InputState state)
    {
		if (GameController::IsPressed(state))
		{
			mShip.UpdateYaw(SHIP_HEADING_TURN_RIGHT);
		}
		else
		{
			mShip.UpdateYaw(SHIP_HEADING_NONE);
		}
    };

    controller.AddInputActionForKey(rightKeyAction);

    ButtonAction upKeyAction;
    upKeyAction.key = GameController::UpKey();
    upKeyAction.action = [this](uint32_t dt, InputState state)
    {
		if (GameController::IsPressed(state))
		{
			mShip.EngageThrusters();
		}
		else
		{
			mShip.DisengageThrusters();
		}
    };
    
    controller.AddInputActionForKey(upKeyAction);

    ButtonAction actionKey;
    actionKey.key = GameController::ActionKey();
    actionKey.action = [this](uint32_t dt, InputState state)
    {
		if (GameController::IsPressed(state) && mGameState == PLAY_GAME)
		{
			mShip.Fire();
		}
        else if (GameController::IsPressed(state) && mGameState == GAME_OVER)
        {
            ResetGame();
        }
    };

    controller.AddInputActionForKey(actionKey);


    ButtonAction cancelKey;
    cancelKey.key = GameController::CancelKey();
    cancelKey.action = [this](uint32_t dt, InputState state)
    {
        if (GameController::IsPressed(state) && mGameState == GAME_OVER)
        {
            App::Singleton().PopScene();
        }
        
    };

    controller.AddInputActionForKey(cancelKey);

	Sprite bgSprite;
	bgSprite.width = mBGImage.GetWidth();
	bgSprite.height = mBGImage.GetHeight();

	App::Singleton().GetScreen().DrawBackground(mBGImage, bgSprite, Vec2D::Zero);

}

void AsteroidsGame::Update(unsigned int dt)
{
    if (mGameState == GAME_OVER)
        return;

    if (mGameState == PLAY_GAME)
    {
        PlayGame(dt);
    }
    else if (mGameState == LEVEL_STARTING)
    {
        mLevelStartTimer -= dt;

        if (mLevelStartTimer <= 0)
        {
            mGameState = PLAY_GAME;
        }
    }
   
}

void AsteroidsGame::Draw(Screen& screen)
{

    if (mGameState != LEVEL_STARTING)
    {
		mShip.Draw(screen);

		for (auto& asteroid : mAsteroids)
		{
			asteroid.Draw(screen);
		}

        for (auto& powerUp : mPowerUps)
        {
            powerUp.Draw(screen);
        }
    }
    
    Vec2D textDrawPosition;
    const auto& font = App::Singleton().GetFont();

    //draw the score

    float screenWidth = static_cast<float>(App::Singleton().Width());
    float screenHeight = static_cast<float>(App::Singleton().Height());


	DrawTransform transform;
	transform.pos = Vec2D::Zero;
	transform.rotationAngle = 0.0f;
	transform.scale = 1.0f;

	ColorParams colorParams;
	colorParams.alpha = 1.0f;
	colorParams.bilinearFiltering = false;
	colorParams.gradient.xParam = GradientXParam::NO_X_GRADIENT;
	colorParams.gradient.yParam = GradientYParam::NO_Y_GRADIENT;
    colorParams.overlay = Color::White();

	UVParams uvParams;

    {
        AARectangle highScoreRect = AARectangle(Vec2D(0.0f, 2.0f), static_cast<uint32_t>(screenWidth / 2.0f), 20);

		textDrawPosition = font.GetDrawPosition(SCORE_STR, highScoreRect, BFXA_LEFT, BFYA_CENTER);
        textDrawPosition = textDrawPosition + Vec2D(5.0f, 0.0f);

        transform.pos = textDrawPosition;

		screen.Draw(font, SCORE_STR, transform, colorParams, uvParams);

        highScoreRect = AARectangle(Vec2D(screenWidth /2.0f, 2.0f), static_cast<uint32_t>(screenWidth / 2.0f), 20);

        std::string scoreStr = std::to_string(mScore);

		textDrawPosition = font.GetDrawPosition(scoreStr, highScoreRect, BFXA_RIGHT, BFYA_CENTER);
        textDrawPosition = textDrawPosition - Vec2D(5, 0);

        transform.pos = textDrawPosition;

		screen.Draw(font, scoreStr, transform, colorParams, uvParams);

    }


	const float X_PAD = 1.0f;

	auto sprite = mAsteroidsSprites.GetSprite(SPACE_SHIP_SPRITE_NAME);
    float xPos = X_PAD;
    const float scale = 0.75f;

    for (int i = 0; i < mLives; ++i)
    {
        transform.pos = Vec2D(xPos, screenHeight - static_cast<float>(sprite.height));
        transform.scale = scale;
        
		screen.Draw(mAsteroidsSprites, SPACE_SHIP_SPRITE_NAME, transform, colorParams, uvParams);
		xPos += X_PAD + (uint32_t)round(static_cast<float>(sprite.width) * scale);
    }

    sprite = mAsteroidsSprites.GetSprite(MISSILE_SPRITE_NAME);
    xPos = screenWidth - mShip.AmmoLeft() * (X_PAD + sprite.width);

    for (uint32_t i = 0; i < mShip.AmmoLeft(); ++i)
    {
        transform.pos = Vec2D(xPos, screenHeight - static_cast<float>(sprite.height) - X_PAD);
        transform.scale = 1.0f;
        transform.rotationAngle = 0.0f;

        screen.Draw(mAsteroidsSprites, MISSILE_SPRITE_NAME, transform, colorParams, uvParams);
        xPos += X_PAD + sprite.width;
    }

    if (mGameState == GAME_OVER)
    {
		textDrawPosition = font.GetDrawPosition(GAME_OVER_STR, mStringRect, BFXA_CENTER, BFYA_CENTER);

        transform.pos = textDrawPosition;

        colorParams.overlay = Color::Red();

		screen.Draw(font, GAME_OVER_STR, transform, colorParams, uvParams);
    }

    if (mGameState == LEVEL_STARTING)
    {
        if (mLevelStartTimer <= READY_TIME_TRANSITION_THRESHOLD && mLevelStartTimer > READY_TIME_DISPLAY_THRESHOLD)
        {
			textDrawPosition = font.GetDrawPosition(READY_STR, mStringRect, BFXA_CENTER, BFYA_CENTER);

            transform.pos = textDrawPosition;
            colorParams.overlay = Color::Red();

			screen.Draw(font, READY_STR, transform, colorParams, uvParams);
        }
        else if (mLevelStartTimer <= READY_TIME_DISPLAY_THRESHOLD && mLevelStartTimer > GO_TIME_TRANSITION_THRESHOLD)
        {
			textDrawPosition = font.GetDrawPosition(READY_STR, mStringRect, BFXA_CENTER, BFYA_CENTER);

			transform.pos = textDrawPosition;
			colorParams.overlay = Color::Red();

			screen.Draw(font, READY_STR, transform, colorParams, uvParams);
        }
        else if (mLevelStartTimer <= GO_TIME_TRANSITION_THRESHOLD && mLevelStartTimer > GO_TIME_DISPLAY_THRESHOLD)
        {
			textDrawPosition = font.GetDrawPosition(GO_STR, mStringRect, BFXA_CENTER, BFYA_CENTER);
			
            transform.pos = textDrawPosition;
			colorParams.overlay = Color::Red();

			screen.Draw(font, GO_STR, transform, colorParams, uvParams);
        }
        else if (mLevelStartTimer <= GO_TIME_DISPLAY_THRESHOLD)
        {
			textDrawPosition = font.GetDrawPosition(GO_STR, mStringRect, BFXA_CENTER, BFYA_CENTER);

			transform.pos = textDrawPosition;
			colorParams.overlay = Color::Red();

			screen.Draw(font, GO_STR, transform, colorParams, uvParams);
        }
    }
}

void AsteroidsGame::Shutdown()
{

}

void AsteroidsGame::AddRandomAsteroid()
{
    static std::uniform_int_distribution<size_t> spriteNamesDistribution(0, asteroidNames.size()-1);
    
    Vec2D spawnLocation = GetSpawnLocation();
    Vec2D vel = GetSpawnVelocity(spawnLocation);
    
    size_t asteroidIndex = static_cast<size_t>(spriteNamesDistribution(mRandomGenerator));
    std::string name = asteroidNames[asteroidIndex];
    Asteroid::Size size = static_cast<Asteroid::Size>(asteroidIndex + 1);

    assert(size != Asteroid::NONE);

    AddAsteroid(mAsteroids, spawnLocation, vel, size);
}

PowerUp AsteroidsGame::MakePowerUp(AsteroidsPowerUpType type, const Vec2D& spawnLocation, const Vec2D& velocity)
{
    switch (type)
    {
    case AsteroidsPowerUpType::GROW:
    {
		PowerUp growPowerUp;

		growPowerUp.Init(mAsteroidsSprites, "grow", spawnLocation, velocity, POWER_UP_LIFE_TIME, [this]() {
            mShip.Grow();
	    });

		return growPowerUp;
    }
    case AsteroidsPowerUpType::LASER:
    {
        PowerUp laserPowerUp;

        laserPowerUp.Init(mAsteroidsSprites, "laser", spawnLocation, velocity, POWER_UP_LIFE_TIME, [this]() {
            mShip.SwitchToLaser();
        });

        return laserPowerUp;
    }
    default:
        assert(false && "Invalid power up type");
        break;
    }

    return PowerUp{};
}

void AsteroidsGame::SpawnPowerUp(const Vec2D& spawnLocation, const Vec2D& velocity)
{
    static std::uniform_int_distribution<size_t> powerUpTypesDistribution(AsteroidsPowerUpType::GROW, AsteroidsPowerUpType::NUM_POWER_UP_TYPES-1);

    AsteroidsPowerUpType type = static_cast<AsteroidsPowerUpType>(powerUpTypesDistribution(mRandomGenerator));
    mPowerUps.push_back(MakePowerUp(type, spawnLocation, velocity));
}

void AsteroidsGame::MaybeSpawnPowerUp(const Vec2D& spawnLocation)
{
    static std::uniform_int_distribution<size_t> powerUpSpawnDistribution(0, 99);

    size_t spawnRNG = powerUpSpawnDistribution(mRandomGenerator);

    if (spawnRNG >= 80)
    {
        //spawn the power up 
        //and figure out the velocity
        static std::uniform_real_distribution<float> powerUpDirectionDistribution(0.0f, TWO_PI);

        float angle = powerUpDirectionDistribution(mRandomGenerator);

        Vec2D velocity{ POWER_UP_SPEED * static_cast<float>(cos(angle)), POWER_UP_SPEED * static_cast<float>(sin(angle)) };

        SpawnPowerUp(spawnLocation, velocity);
    }
}

void AsteroidsGame::AddAsteroid(std::vector<Asteroid>& asteroids, const Vec2D& spawnLocation, const Vec2D& velocity, Asteroid::Size size)
{
    static std::uniform_real_distribution<float> slowRotationDistribution(PI / 8.0f, PI / 5.0f);
    static std::uniform_real_distribution<float> mediumRotationDistribution(PI / 3.0f, PI);
    static std::uniform_real_distribution<float> fastRotationDistribution(1.1f* PI , 1.9f * PI);

    if (size > Asteroid::NONE)
    {
		std::string name = asteroidNames[static_cast<size_t>(size) - 1];

        float rotationRate = 0;

        if (size == Asteroid::LARGE)
        {
            rotationRate = static_cast<float>(slowRotationDistribution(mRandomGenerator));
        }
        else if (size == Asteroid::MEDIUM)
        {
            rotationRate = static_cast<float>(mediumRotationDistribution(mRandomGenerator));
        }
        else
        {
            rotationRate = static_cast<float>(fastRotationDistribution(mRandomGenerator));
        }

		Asteroid asteroid;
		asteroid.Init(mAsteroidsSprites, name, size, spawnLocation, velocity, rotationRate);

        
		asteroids.push_back(asteroid);
    }
    
}

Vec2D AsteroidsGame::GetSpawnLocation()
{
    Sprite largestSprite = mAsteroidsSprites.GetSprite("big_rock");
    static std::vector<int> xSpawnLocations = {-static_cast<int>(largestSprite.width), static_cast<int>(largestSprite.width + App::Singleton().Width())};
    static std::uniform_int_distribution<size_t> xSpawnDistribution(0, xSpawnLocations.size()-1);
    static std::uniform_int_distribution<int> ySpawnDistribution(-static_cast<int>(largestSprite.height), static_cast<int>(App::Singleton().Height() + largestSprite.height));
    
    int xSpawnLocation = xSpawnLocations[static_cast<size_t>(xSpawnDistribution(mRandomGenerator))];
    int ySpawnLocation = static_cast<int>(ySpawnDistribution(mRandomGenerator));
    
    return Vec2D(static_cast<float>(xSpawnLocation), static_cast<float>(ySpawnLocation));
}

Vec2D AsteroidsGame::GetSpawnVelocity(const Vec2D& position)
{
    static std::uniform_real_distribution<float> positiveVelComp(10, 30);
    static std::uniform_real_distribution < float > negativeVelComp(-30, -10);
    
    Vec2D vel;
    Vec2D spawnLocation = GetSpawnLocation();
    
    if (spawnLocation.GetX() > 0)
    {
        vel.SetX(static_cast<float>(negativeVelComp(mRandomGenerator)));
    }
    else
    {
        vel.SetX(static_cast<float>(positiveVelComp(mRandomGenerator)));
    }
    
    
    if (spawnLocation.GetY() > App::Singleton().Height() /2)
    {
        vel.SetY(static_cast<float>(negativeVelComp(mRandomGenerator)));
    }
    else
    {
        vel.SetY(static_cast<float>(positiveVelComp(mRandomGenerator)));
    }
    
    return vel;
}

bool AsteroidsGame::AmmoOutOfBounds(const std::unique_ptr<ShipAmmo>& ammo)
{
    return ammo->IsOutOfBounds(0, 0, App::Singleton().Width(), App::Singleton().Height());
}

Vec2D AsteroidsGame::WrapAround(const Vec2D& objectPosition, const Vec2D& objectSize)
{
    Vec2D newPos = objectPosition;

    if (objectPosition.GetX() > App::Singleton().Width() + objectSize.GetX())
    {
        newPos.SetX(-objectSize.GetX()+0.01f);
    }
    else if (objectPosition.GetX() < -objectSize.GetX())
    {
        newPos.SetX(App::Singleton().Width() + objectSize.GetX() - 0.01f);
    }
    
    if (objectPosition.GetY() < -objectSize.GetY())
    {
        newPos.SetY(App::Singleton().Height() + objectSize.GetY() - 0.01f);
    }
    else if (objectPosition.GetY() > App::Singleton().Height() + objectSize.GetY())
    {
        newPos.SetY(-objectSize.GetY() + 0.01f);
    }

    return newPos;
}

void AsteroidsGame::ResetLevel()
{
	mAsteroids.clear();
	mShip.Respawn(Vec2D(static_cast<float>(App::Singleton().Width()) / 2.0f, static_cast<float>(App::Singleton().Height()) / 2.0f));
	SetupAsteroids();
    mGameState = LEVEL_STARTING;
    mLevelStartTimer = LEVEL_START_TIME;
}

void AsteroidsGame::ResetGame()
{
    mLives = NUM_LIVES;
    mScore = 0;
    ResetLevel();
}


void AsteroidsGame::PlayGame(unsigned int dt)
{
	if (mShip.IsDestroyed())
	{
		--mLives;

		if (mLives >= 0)
		{
			ResetLevel();
		}
        else
        {
            mGameState = GAME_OVER;
        }
		return;
	}

	mShip.Update(dt);

    Vec2D newShipPosition = WrapAround(mShip.GetPosition(), Vec2D(mShip.GetWidth(), mShip.GetHeight()));
    mShip.SetPosition(newShipPosition);

    int64_t powerUpHitIndex = -1;
    for (size_t i = 0; i < mPowerUps.size(); ++i)
    {
        PowerUp& powerUp = mPowerUps[i];

        if (powerUp.IsActive())
        {
			powerUp.Update(dt);

			Vec2D newPowerUpPosition = WrapAround(powerUp.GetPosition(), Vec2D(powerUp.GetWidth(), powerUp.GetHeight()));
			powerUp.SetPosition(newPowerUpPosition);

			if (mShip.GetBoundingCircle().Intersects(powerUp.GetBoundingCircle()))
			{
                powerUp.ExecuteEffect();
                powerUpHitIndex = i;
                break;
			}
        }
    }

    if (powerUpHitIndex >= 0)
    {
        mPowerUps.erase(mPowerUps.begin() + powerUpHitIndex);
    }

    RemoveDestroyedAsteroids(dt);
	Sprite largestSprite = mAsteroidsSprites.GetSprite("big_rock");

	Vec2D largestSpriteSize = Vec2D(static_cast<float>(largestSprite.width), static_cast<float>(largestSprite.height));
	for (auto& asteroid : mAsteroids)
	{
		asteroid.Update(dt);

		if (!asteroid.IsDestroyed() && !asteroid.IsExploding() &&  mShip.GetBoundingCircle().Intersects(asteroid.GetBoundingCircle()))
		{
            if (COLLISIONS)
            {
				mShip.HitByAsteroid();
				mPowerUps.clear();
            }
		}

		if (asteroid.IsOutOfBounds(largestSpriteSize))
		{
			Vec2D pos = GetSpawnLocation();
			asteroid.SetPosition(pos);
			asteroid.SetVelocity(GetSpawnVelocity(pos));
		}
	}

	const auto& ammo = mShip.GetAmmoInFlight();
	std::vector<size_t> ammoToAddBack;

	for (const auto& ammoInstance : ammo)
	{
		std::vector<Asteroid> asteroidsToAdd;

		bool missileHit = false;
		size_t asteroidIndex = 0;

        std::vector<Vec2D> intersections = {};
        intersections.reserve(10);

		for (size_t i = 0; i < mAsteroids.size(); ++i)
		{
			const Asteroid& asteroid = mAsteroids[i];
            
			if (!asteroid.IsDestroyed() && !asteroid.IsExploding() && ammoInstance->Intersects(asteroid.GetBoundingCircle(), intersections))
			{
				missileHit = true;
				asteroidsToAdd = BreakAsteroid(intersections[0], ammoInstance->GetVelocity(), asteroid, ammoInstance->Scale());
				asteroidIndex = i;

				//add score
                mScore += ScoreValue(asteroid.GetSize());
				break;
			}
            else
            {
                intersections.clear();
            }
		}

		if (missileHit)
		{
            if (SCREEN_SHAKE)
            {
                App::Singleton().GetScreen().Shake(SCREEN_SHAKE_POWER, SCREEN_SHAKE_TIME);
            }

            mAsteroids[asteroidIndex].HitByAmmo(intersections);
		}
        

		if (missileHit && asteroidsToAdd.size() == 0)
		{
			//A small asteroid was hit
            MaybeSpawnPowerUp(intersections[0]);
		}

		if ((missileHit && ammoInstance->DisappearsOnHit()) || AmmoOutOfBounds(ammoInstance))
		{
			ammoToAddBack.push_back(ammoInstance->GetID());
		}

		mAsteroids.insert(mAsteroids.end(), asteroidsToAdd.begin(), asteroidsToAdd.end());
	}

	for (auto ammoInstance : ammoToAddBack)
	{
		mShip.RegainAmmo(ammoInstance);
	}


	SetupAsteroids();
}

int AsteroidsGame::ScoreValue(Asteroid::Size asteroidSizeHit)
{
    if (asteroidSizeHit == Asteroid::NONE || asteroidSizeHit == Asteroid::NUM_SIZES)
    {
        return 0;
    }

    return asteroidSizeHit * SCORE_VAL;
}

std::vector<Asteroid> AsteroidsGame::BreakAsteroid(const Vec2D& position, const Vec2D& velocity, const Asteroid& asteroid, float missileScale)
{
    std::vector<Asteroid> asteroidsToAdd;

    //spawn new asteroids
    size_t numAsteroidsToSpawn = static_cast<size_t>(asteroid.GetSize()) + 1;

    if (numAsteroidsToSpawn < Asteroid::NUM_SIZES && IsEqual(missileScale, 1.0f))
    {

        float dot = velocity.GetUnitVec().Dot(-asteroid.GetVelocity().GetUnitVec());

        float deltaAngle = acosf(dot);


        if (numAsteroidsToSpawn > 0)
        {
            Vec2D newVec2 = asteroid.GetVelocity().RotationResult(deltaAngle);

            Asteroid::Size asteroidSize = static_cast<Asteroid::Size>(numAsteroidsToSpawn);
            
            AddAsteroid(asteroidsToAdd, asteroid.GetPosition(), newVec2, asteroidSize);

			for (size_t i = 0; i < numAsteroidsToSpawn - 1; ++i)
			{
                newVec2 = newVec2.RotationResult(deltaAngle);

				AddAsteroid(asteroidsToAdd, asteroid.GetPosition(), newVec2, asteroidSize);
			}
        }
        
    }

    return asteroidsToAdd;
}

void AsteroidsGame::RemoveDestroyedAsteroids(unsigned int dt)
{
    auto iter = std::remove_if(mAsteroids.begin(), mAsteroids.end(), [](const Asteroid& a) {
        return a.IsDestroyed();
    });

    if (iter != mAsteroids.end())
    {
        mAsteroids.erase(iter, mAsteroids.end());
    }
}

void AsteroidsGame::SetupAsteroids()
{
    for (size_t i = mAsteroids.size(); i < STARTING_NUM_ASTEROIDS; ++i)
    {
        AddRandomAsteroid();
    }
}

const std::string& AsteroidsGame::GetName()const
{
    static std::string name = "Asteroids";
    return name;
}
