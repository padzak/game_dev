/*
 * Button.h
 *
 *  Created on: Jan. 17, 2019
 *      Author: serge
 */

#ifndef APP_BUTTON_H_
#define APP_BUTTON_H_

#include <string>
#include "AARectangle.h"
#include <functional>
#include "Color.h"

#include "BitmapFont.h"


class Screen;

class Button
{
public:
	using ButtonAction = std::function<void(void)>;

	Button(const BitmapFont& bitmapFont, const Color& textColor, const Color& highlightColor = Color::White());
	virtual void Init(const std::string& text, Vec2D topLeft, unsigned int width, unsigned height);
	virtual void Update(uint32_t dt) {}
	virtual void Draw(Screen& theScreen);
	virtual void ExecuteAction();

	inline void SetButtonText(const std::string& text) {mTitle = text;}
	inline const std::string& GetButtonText() const {return mTitle;}
	virtual void SetHighlighted(bool highlighted) {mHighlighted = highlighted;}
	inline bool IsHighlighted() const {return mHighlighted;}
	inline void SetButtonAction(Button::ButtonAction action) {mAction = action;}

protected:

	void DrawHighlight(Screen& theScreen);

	Vec2D mDrawPosition;
	float mTextScale;
	const BitmapFont& mBitmapFont;
	std::string mTitle;
	AARectangle mBBox;
	Color mTextColor;
	Color mHighlightColor;
	ButtonAction mAction;
	bool mHighlighted;
};



#endif /* APP_BUTTON_H_ */
