//
//  AsteroidsGame.h
//  PacMan
//
//  Created by Serge Lansiquot on 2018-03-31.
//  Copyright (c) 2018 Serge Lansiquot. All rights reserved.
//

#ifndef __PacMan__AsteroidsGame__
#define __PacMan__AsteroidsGame__

#include "Game.h"
#include "Ship.h"
#include "SpriteSheet.h"
#include "Asteroid.h"
#include "PowerUp.h"
#include "AsteroidsPowerUpTypes.h"
#include <vector>
#include <random>

enum AsteroidsGameState
{
	LEVEL_STARTING = 0,
	PLAY_GAME,
	GAME_OVER
};

class AsteroidsGame : public Game
{
public:
    AsteroidsGame();
    virtual void Init(GameController& controller) override;
    virtual void Update(unsigned int dt) override;
    virtual void Draw(Screen& screen) override;
    virtual void Shutdown() override;
    virtual const std::string& GetName()const override;
    
private:
    


    void SetupAsteroids();
    void AddRandomAsteroid();
    void RemoveDestroyedAsteroids(unsigned int dt);
    
    Vec2D GetSpawnLocation();
    Vec2D GetSpawnVelocity(const Vec2D& position);
    bool AmmoOutOfBounds(const std::unique_ptr<ShipAmmo>& ammo);
	std::vector<Asteroid> BreakAsteroid(const Vec2D& position, const Vec2D& velocity, const Asteroid& asteroid, float ammoScale);

    void AddAsteroid(std::vector<Asteroid>& asteroids, const Vec2D& spawnLocation, const Vec2D& velocity, Asteroid::Size size);
    Vec2D WrapAround(const Vec2D& objectPosition, const Vec2D& objectSize);
    void ResetLevel();
    void ResetGame();
    int ScoreValue(Asteroid::Size asteroidSizeHit);
    void PlayGame(unsigned int dt);


    PowerUp MakePowerUp(AsteroidsPowerUpType type, const Vec2D& spawnLocation, const Vec2D& velocity);
    void SpawnPowerUp(const Vec2D& spawnLocation, const Vec2D& velocity);
    void MaybeSpawnPowerUp(const Vec2D& spawnLocation);

    AsteroidsGameState mGameState;
    BMPImage mBGImage;
    Ship mShip;
    SpriteSheet mAsteroidsSprites;
    std::vector<Asteroid> mAsteroids;
    std::vector<PowerUp> mPowerUps;
    std::default_random_engine mRandomGenerator;
    AARectangle mStringRect;
    int mLives;
    int mScore;

    int mLevelStartTimer;

};


#endif /* defined(__PacMan__AsteroidsGame__) */
