#ifndef __ANIMATED_BUTTON_H__
#define __ANIMATED_BUTTON_H__


#include "Button.h"

class AnimatedButton : public Button
{
public:
	AnimatedButton(const BitmapFont& bitmapFont, const Color& textColor, const Color& highlightColor = Color::White());
	virtual void Init(const std::string& text, Vec2D topLeft, unsigned int width, unsigned height) override;
	virtual void Update(uint32_t dt) override;
	virtual void ExecuteAction() override;
	virtual void Draw(Screen& theScreen) override;

	virtual void SetHighlighted(bool highlighted) override;

private:

	float UpdateAnimationTime(uint32_t dt, float totalAnimTimeMod = 1.0f);
	void SetupLetterAnimationData(const std::string& text);
	void UpdateLetterAnimationData();
	void DrawButtonAnimation(Screen& theScreen);


	Color mOriginalColor;
	int mTotalAnimationTime;
	int mAnimationTime;
	bool mIsAnimating;

	bool mExecuteTriggered;

	struct LetterAnimationData
	{
		std::string letter;
		Vec2D endPosition;
		Vec2D startPosition;
		int delay; //in ms

		Vec2D drawPosition;
	};

	std::vector<LetterAnimationData> mLetterAnimations;
	

};

#endif // __ANIMATED_BUTTON_H__
