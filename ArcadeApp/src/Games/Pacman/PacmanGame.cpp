/*
 * PacmanGame.cpp
 *
 *  Created on: Jan. 18, 2019
 *      Author: serge
 */

#include "PacmanGame.h"
#include "App.h"
#include "PacmanGameUtils.h"
/*
 Overall Design

 - Pacman
    - Can eat ghosts if ate a super pellet
    - Eats pellets
    - Move in 4 directions (up, down, left, right) by player input
    - Animates
    - Can be eaten by ghosts
    - Lives
    - Score
 - Ghost
    - Can eat pacman if not in a vulnerable state
    - Moves in 4 directions (up, down, left, right)
    - Animates
    - Has different states it can be in (Alive, Vulnerable, Vulnerable-ending, dead)
    - Has AI
    - Has a specific spot on the level that it starts at (spawn location)
    - Has a score value for when it's eaten
 - AI
    - Chase - Each ghost has a specific personality
        - Chase (Blinky)
        - Cut off/Ambush (Pinky)
        - Pinser attack with Blinky (Inky)
        - Wander (Clyde)
    - Scatter mode
 - Level
    - Contains all the walls (maze)
    - Contains pellets
    - Wraps Pacman/Ghosts at specific locations
    - Gate the releases the ghosts
    - Single background image
 - Pellet
    - Super pellet - turns ghosts to vulnerable state if pacman eats it
    - Has a specific score value
 - Special Items
    - Different ones appear with differring score values
 - UI
    - Score
    - Lives left
    - Game over/Ready label
 - Game
 	 -Level
 	 -Pacman
 	 -Ghosts
    - Different states
        - Level starting
        - Play Game
        - Lost life
        - Game Over
 */

namespace
{
	const std::string SCORE_STR = "Score  ";
	const std::string PACMAN_LIFE_SPRITE_NAME = "pac_man_left_idle";
	const size_t MAX_NUM_LIVES = 3;
	const uint32_t RELEASE_GHOST_TIME = 5000;
	const std::string READY_STR = "Ready!";
	const std::string GAME_OVER_STR = "Game Over";
	const uint32_t LEVEL_STARTING_TIME = 3000;
}


void PacmanGame::Init(GameController& controller)
{
	mReleaseGhostTimer = 0;
	mPacmanSpriteSheet.Load("PacmanSprites");
	mPacman.Init(mPacmanSpriteSheet, App::Singleton().GetBasePath() + "assets/Pacman_animations.txt", Vec2D::Zero, PACMAN_MOVEMENT_SPEED, false);

	mLevel.Init(App::Singleton().GetBasePath() + "assets/Pacman_level.txt", &mPacmanSpriteSheet);


	mStringRect = AARectangle(Vec2D(0.0f, mLevel.GetInGameTextYPos()), App::Singleton().Width(), static_cast<unsigned int>(mPacman.GetBoundingBox().GetHeight()));

	SetupGhosts();
	ResetGame();

	ButtonAction leftAction;
	leftAction.key = GameController::LeftKey();
	leftAction.action = [this](uint32_t dt, InputState state)
		{
			HandleGameControllerState(dt, state, PACMAN_MOVEMENT_LEFT);
		};
	controller.AddInputActionForKey(leftAction);

	ButtonAction rightAction;
	rightAction.key = GameController::RightKey();
	rightAction.action = [this](uint32_t dt, InputState state)
		{
			HandleGameControllerState(dt, state, PACMAN_MOVEMENT_RIGHT);
		};
	controller.AddInputActionForKey(rightAction);

	ButtonAction upAction;
	upAction.key = GameController::UpKey();
	upAction.action = [this](uint32_t dt, InputState state)
		{
			HandleGameControllerState(dt, state, PACMAN_MOVEMENT_UP);
		};
	controller.AddInputActionForKey(upAction);

	ButtonAction downAction;
	downAction.key = GameController::DownKey();
	downAction.action = [this](uint32_t dt, InputState state)
		{
			HandleGameControllerState(dt, state, PACMAN_MOVEMENT_DOWN);
		};
	controller.AddInputActionForKey(downAction);

	ButtonAction backAction;
	backAction.key = GameController::CancelKey();
	backAction.action = [this](uint32_t dt, InputState state)
		{
			if(mGameState == GAME_OVER && GameController::IsPressed(state))
			{
				App::Singleton().PopScene();
			}
		};

	controller.AddInputActionForKey(backAction);
}

void PacmanGame::Update(uint32_t dt)
{
	if(mGameState == LEVEL_STARTING)
	{
		mPacman.ResetGhostEatenMultiplier();
		mLevelStartingTimer += dt;

		if(mLevelStartingTimer >= LEVEL_STARTING_TIME)
		{
			mLevelStartingTimer = 0;
			mGameState = PLAY_GAME;
		}
	}
	else if(mGameState == PLAY_GAME)
	{
		UpdatePacmanMovement();
		mPacman.Update(dt);

		mReleaseGhostTimer += dt;

		for(size_t i = 0; i < NUM_GHOSTS; ++i)
		{

			Ghost& ghost = mGhosts[i];
			GhostAI& ghostAI = mGhostAIs[i];

			if(mReleaseGhostTimer >= RELEASE_GHOST_TIME && ghostAI.IsInPen() && !ghost.IsReleased())
			{
				mReleaseGhostTimer = 0;
				ghost.ReleaseFromPen();
			}

			auto direction = ghostAI.Update(dt, mPacman, mLevel, mGhosts);

			if((ghost.IsReleased() && ghost.CanChangeDirection()) || (ghost.IsReleased() && ghostAI.WantsToLeavePen() && direction != PACMAN_MOVEMENT_NONE))
			{
				if(direction != ghost.GetMovementDirection())
				{
					ghost.SetMovementDirection(direction);
					ghost.LockCanChangeDirection();
				}
			}

			ghost.Update(dt);

			if(ghost.IsVulnerable() && mPacman.GetEatingBoundingBox().Intersects(ghost.GetBoundingBox()))
			{
				ghost.EatenByPacman();
				mPacman.AteGhost(ghost.GetPoints());
			}
			else if(ghost.IsAlive() && ghost.GetEatingBoundingBox().Intersects(mPacman.GetBoundingBox()))
			{
				mNumLives--;
				mGameState = LOST_LIFE;
				mPacman.EatenByGhost();
				mPressedDirection = PACMAN_MOVEMENT_NONE;
				mPacman.SetMovementDirection(PACMAN_MOVEMENT_NONE);
				return;
			}

		}

		mLevel.Update(dt, mPacman, mGhosts, mGhostAIs);

		if(mLevel.IsLevelOver())
		{
			mLevel.IncreaseLevel();
			ResetLevel();
		}
	}
	else if(mGameState == LOST_LIFE)
	{
		mPacman.Update(dt); //to update the death animation

		if(mPacman.IsFinishedAnimation())
		{
			if(mNumLives >= 0)
			{
				ResetLevel();
			}
			else
			{
				mGameState = GAME_OVER;
			}
		}
	}
}

void PacmanGame::Draw(Screen& screen)
{
	mLevel.Draw(screen);
	//printf("Draw pacman\n");
	mPacman.Draw(screen);

	for(auto& ghost: mGhosts)
	{
		ghost.Draw(screen);
	}

	/*
	for(auto& ghostAI: mGhostAIs)
	{
		ghostAI.Draw(screen);
	}
	*/

	const auto& font = App::Singleton().GetFont();
	Vec2D textDrawPosition;

	//Draw Score
	{
		Vec2D levelOffset = mLevel.GetLayoutOffset();

		AARectangle highScoreRect = AARectangle(Vec2D(0.0f, 4.0f), App::Singleton().Width(), static_cast<unsigned int>(levelOffset.GetY()));

		std::string scoreStr = std::to_string(mPacman.Score());

		textDrawPosition = font.GetDrawPosition(SCORE_STR + scoreStr, highScoreRect, BFXA_CENTER, BFYA_CENTER);

		screen.Draw(font, SCORE_STR + scoreStr, textDrawPosition);
	}

	if(mGameState == LEVEL_STARTING)
	{
		textDrawPosition = font.GetDrawPosition(READY_STR, mStringRect, BFXA_CENTER, BFYA_CENTER);

		screen.Draw(font, READY_STR, textDrawPosition, Color::Yellow());
	}
	else if(mGameState == GAME_OVER)
	{
		textDrawPosition = font.GetDrawPosition(GAME_OVER_STR, mStringRect, BFXA_CENTER, BFYA_CENTER);

		screen.Draw(font, GAME_OVER_STR, textDrawPosition, Color::Yellow());
	}


	DrawLives(screen);
}

void PacmanGame::DrawLives(Screen& screen)
{
	const float X_PAD = 1.0f;

	auto sprite = mPacmanSpriteSheet.GetSprite(PACMAN_LIFE_SPRITE_NAME);

	float xPos = X_PAD;

	for(int i = 0; i < mNumLives; ++i)
	{
		screen.Draw(mPacmanSpriteSheet, PACMAN_LIFE_SPRITE_NAME, Vec2D(xPos, App::Singleton().Height() - static_cast<float>(sprite.height)));
		xPos += X_PAD + static_cast<float>(sprite.width);
	}
}

void PacmanGame::Shutdown()
{

}

const std::string& PacmanGame::GetName() const
{
	static std::string name = "!Pacman!";
	return name;
}

void PacmanGame::ResetGame()
{
	mNumLives = MAX_NUM_LIVES;
	mPressedDirection = PACMAN_MOVEMENT_NONE;
	mPacman.ResetScore();
	mLevel.ResetToFirstLevel();
	ResetLevel();
}

void PacmanGame::ResetLevel()
{
	mReleaseGhostTimer = 0;
	mGameState = LEVEL_STARTING;
	mPacman.MoveTo(mLevel.GetPacmanSpawnLocation());
	mPacman.ResetToFirstAnimation();

	for(auto& ghost : mGhosts)
	{
		ghost.ResetToFirstPosition();
	}

	mGhosts[BLINKY].ReleaseFromPen();
}

void PacmanGame::UpdatePacmanMovement()
{
	if(mPressedDirection != PACMAN_MOVEMENT_NONE)
	{
		if(!mLevel.WillCollide(mPacman.GetBoundingBox(), mPressedDirection))
		{
			mPacman.SetMovementDirection(mPressedDirection);
		}
	}
}

void PacmanGame::HandleGameControllerState(uint32_t dt, InputState state, PacmanMovement direction)
{
	if(GameController::IsPressed(state))
	{
		mPressedDirection = direction;
	}
	else if(GameController::IsReleased(state) && mPressedDirection == direction)
	{
		mPressedDirection = PACMAN_MOVEMENT_NONE;
	}
}

void PacmanGame::SetupGhosts()
{
	float screenWidth = static_cast<float>(App::Singleton().Width());
	float screenHeight = static_cast<float>(App::Singleton().Height());

	const Vec2D BLINKY_SCATTER_POS = Vec2D(screenWidth - 24.0f, 0.0f);
	const Vec2D INKY_SCATTER_POS = Vec2D(screenWidth, screenHeight);
	const Vec2D PINKY_SCATTER_POS = Vec2D(24.0f, 0.0f);
	const Vec2D CLYDE_SCATTER_POS = Vec2D(0.0f, screenHeight);

	mGhosts.resize(NUM_GHOSTS);
	mGhostAIs.resize(NUM_GHOSTS);

	Ghost blinky;
	blinky.Init(mPacmanSpriteSheet, App::Singleton().GetBasePath() + "assets/Ghost_animations.txt", mLevel.GetGhostSpawnPoints()[BLINKY], GHOST_MOVEMENT_SPEED, true, Color::Red());
	blinky.SetMovementDirection(PACMAN_MOVEMENT_LEFT);
	mGhosts[BLINKY] = blinky;

	auto ghostWidth = static_cast<uint32_t>(blinky.GetBoundingBox().GetWidth());

	auto blinkyAI = GhostAI();
	blinkyAI.Init(mGhosts[BLINKY], ghostWidth, BLINKY_SCATTER_POS, mLevel.GetGhostSpawnPoints()[PINKY], mLevel.GetGhostSpawnPoints()[BLINKY], BLINKY);

	mGhostAIs[BLINKY] = blinkyAI;

	Ghost pinky;
	pinky.Init(mPacmanSpriteSheet, App::Singleton().GetBasePath() + "assets/Ghost_animations.txt", mLevel.GetGhostSpawnPoints()[PINKY], GHOST_MOVEMENT_SPEED, true, Color::Pink());
	pinky.SetMovementDirection(PACMAN_MOVEMENT_DOWN);
	mGhosts[PINKY] = pinky;

	auto pinkyAI = GhostAI();
	pinkyAI.Init(mGhosts[PINKY], ghostWidth, PINKY_SCATTER_POS, mLevel.GetGhostSpawnPoints()[PINKY], mLevel.GetGhostSpawnPoints()[BLINKY], PINKY);

	mGhostAIs[PINKY] = pinkyAI;

	Ghost inky;
	inky.Init(mPacmanSpriteSheet, App::Singleton().GetBasePath() + "assets/Ghost_animations.txt", mLevel.GetGhostSpawnPoints()[INKY], GHOST_MOVEMENT_SPEED, true, Color::Cyan());
	inky.SetMovementDirection(PACMAN_MOVEMENT_UP);
	mGhosts[INKY] = inky;

	auto inkyAI = GhostAI();
	inkyAI.Init(mGhosts[INKY], ghostWidth, INKY_SCATTER_POS, mLevel.GetGhostSpawnPoints()[PINKY], mLevel.GetGhostSpawnPoints()[BLINKY], INKY);

	mGhostAIs[INKY] = inkyAI;

	Ghost clyde;
	clyde.Init(mPacmanSpriteSheet, App::Singleton().GetBasePath() + "assets/Ghost_animations.txt", mLevel.GetGhostSpawnPoints()[CLYDE], GHOST_MOVEMENT_SPEED, true, Color::Orange());
	clyde.SetMovementDirection(PACMAN_MOVEMENT_UP);
	mGhosts[CLYDE] = clyde;

	auto clydeAI = GhostAI();
	clydeAI.Init(mGhosts[CLYDE], ghostWidth, CLYDE_SCATTER_POS, mLevel.GetGhostSpawnPoints()[PINKY], mLevel.GetGhostSpawnPoints()[BLINKY], CLYDE);

	mGhostAIs[CLYDE] = clydeAI;

	for(size_t i = 0; i < NUM_GHOSTS; ++i)
	{
		mGhosts[i].SetGhostDelegate(mGhostAIs[i]);
	}
}

