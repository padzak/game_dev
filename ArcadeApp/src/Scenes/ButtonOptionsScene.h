/*
 * ButtonOptionsScene.h
 *
 *  Created on: Jan. 17, 2019
 *      Author: serge
 */

#ifndef APP_BUTTONOPTIONSSCENE_H_
#define APP_BUTTONOPTIONSSCENE_H_

#include "Scene.h"
#include "AnimatedButton.h"
#include <string>
#include <vector>
#include <memory>

class Screen;

class ButtonOptionsScene: public Scene
{
public:

	ButtonOptionsScene(const std::vector<std::string>& optionNames, const Color& textColor, bool animated);

	virtual void Init() override;
	virtual void Update(uint32_t dt) override;
	virtual void Draw(Screen& theScreen) override;

	void SetButtonActions(const std::vector<Button::ButtonAction>& buttonActions);

private:

	void SetNextButtonHighlighted();
	void SetPreviousButtonHighlighted();
	void ExecuteCurrentButtonAction();
	void HighlightCurrentButton();

	std::vector<std::unique_ptr<Button>> mButtons;
	int mHighlightedOption;
};

#endif /* APP_BUTTONOPTIONSSCENE_H_ */
