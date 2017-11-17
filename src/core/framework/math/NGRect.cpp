//
//  NGRect.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NGRect.h"

NGRect::NGRect(float x, float y, float width, float height, float angle)
{
    _lowerLeft = Vector2(x, y);
    _width = width;
    _height = height;
    _angle = angle;
}

Vector2& NGRect::getLowerLeft()
{
    return _lowerLeft;
}

float NGRect::getRight()
{
    return _lowerLeft.getX() + _width;
}

float NGRect::getTop()
{
    return _lowerLeft.getY() + _height;
}

float NGRect::getLeft()
{
    return _lowerLeft.getX();
}

float NGRect::getBottom()
{
    return _lowerLeft.getY();
}

float NGRect::getWidth()
{
    return _width;
}

void NGRect::setWidth(float width)   
{
    _width = width;
}

float NGRect::getHeight()
{
    return _height;
}

void NGRect::setHeight(float height)
{
    _height = height;
}

float NGRect::getAngle()
{
    return _angle;
}

void NGRect::setAngle(float angle)
{
    _angle = angle;
}
