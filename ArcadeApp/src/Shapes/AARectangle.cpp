/*
 * AARectangle.cpp
 *
 *  Created on: Jan. 11, 2019
 *      Author: serge
 */

#include "AARectangle.h"
#include "Utils.h"
#include <cmath>

AARectangle::AARectangle(): AARectangle(Vec2D::Zero, Vec2D::Zero)
{

}

AARectangle::AARectangle(const Vec2D& topLeft, unsigned int width, unsigned int height)
{
	mPoints.push_back(topLeft);
	mPoints.push_back(Vec2D(topLeft.GetX() + width - 1, topLeft.GetY() + height-1));
}

AARectangle::AARectangle(const Vec2D& topLeft, const Vec2D& bottomRight)
{
	mPoints.push_back(topLeft);
	mPoints.push_back(bottomRight);
}

float AARectangle::GetWidth() const
{
	return GetBottomRightPoint().GetX() - GetTopLeftPoint().GetX() + 1;
}

float AARectangle::GetHeight() const
{
	return GetBottomRightPoint().GetY() - GetTopLeftPoint().GetY() + 1;
}

void AARectangle::MoveTo(const Vec2D& position)
{
	float width = GetWidth();
	float height = GetHeight();

	SetTopLeftPoint(position);
	SetBottomRightPoint(Vec2D(position.GetX() + width - 1, position.GetY() + height - 1));
}

Vec2D AARectangle::GetCenterPoint() const
{
	return Vec2D(GetTopLeftPoint().GetX() + GetWidth()/2.0f, GetTopLeftPoint().GetY() + GetHeight()/2.0f);
}

bool AARectangle::Intersects(const AARectangle& otherRect) const
{
	Vec2D otherRectBtmRight = otherRect.GetBottomRightPoint();
	Vec2D otherRectTopLeft = otherRect.GetTopLeftPoint();

	Vec2D btmRight = GetBottomRightPoint();
	Vec2D topLeft = GetTopLeftPoint();

	return !(otherRectBtmRight.GetX() < topLeft.GetX() ||
			otherRectTopLeft.GetX() > btmRight.GetX() ||
			otherRectBtmRight.GetY() < topLeft.GetY() ||
			otherRectTopLeft.GetY() > btmRight.GetY());
}

bool AARectangle::ContainsPoint(const Vec2D& point) const
{
	bool withinX = point.GetX() >= GetTopLeftPoint().GetX() && point.GetX() <= GetBottomRightPoint().GetX();
	bool withinY = point.GetY() >= GetTopLeftPoint().GetY() && point.GetY() <= GetBottomRightPoint().GetY();

	return withinX && withinY;
}

AARectangle AARectangle::Inset(const AARectangle& rect, const Vec2D& insets)
{
	return AARectangle(rect.GetTopLeftPoint() + insets, static_cast<unsigned int>(rect.GetWidth() - 2.0f*insets.GetX()), static_cast<unsigned int>(rect.GetHeight() - 2*insets.GetY()));
}

std::vector<Vec2D> AARectangle::GetPoints() const
{
	return { mPoints[0], Vec2D(mPoints[1].GetX(), mPoints[0].GetY()), mPoints[1], Vec2D(mPoints[0].GetX(), mPoints[1].GetY()) };
}
