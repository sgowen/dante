//
//  Circle.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/6/13.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Circle.h"

Circle::Circle(float x, float y, float radius) : _center(x, y), _radius(radius)
{
    // Empty
}

Vector2& Circle::getCenter()
{
    return _center;
}

float Circle::getRadius()
{
    return _radius;
}
