/*
 * GameScene.cpp
 *
 *  Created on: Jan. 14, 2019
 *      Author: serge
 */


#include "GameScene.h"
#include "Screen.h"

GameScene::GameScene(std::unique_ptr<Game> optrGame):mGame(std::move(optrGame))
{

}

void GameScene::Init()
{
	mGame->Init(mGameController);
}

void GameScene::Update(uint32_t dt)
{
	mGame->Update(dt);
}

void GameScene::Draw(Screen& screen)
{
	mGame->Draw(screen);
}

void GameScene::Shutdown(Screen& theScreen)
{
	theScreen.ClearBackground();
}

const std::string& GameScene::GetSceneName() const
{
	return mGame->GetName();
}

