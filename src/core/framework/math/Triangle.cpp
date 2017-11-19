//
//  Triangle.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/math/Triangle.h"

#include "framework/math/Vector2.h"

Triangle::Triangle(float x1, float y1, float x2, float y2, float x3, float y3) :
_sideA(x1, y1, x2, y2),
_sideB(x2, y2, x3, y3),
_sideC(x3, y3, x1, y1)
{
    // Empty
}

void Triangle::set(float x1, float y1, float x2, float y2, float x3, float y3)
{
    _sideA.getOrigin().set(x1, y1);
    _sideA.getEnd().set(x2, y2);
    _sideB.getOrigin().set(x2, y2);
    _sideB.getEnd().set(x3, y3);
    _sideC.getOrigin().set(x3, y3);
    _sideC.getEnd().set(x1, y1);
}

Vector2& Triangle::getPointA()
{
    return _sideA.getOrigin();
}

Vector2& Triangle::getPointB()
{
    return _sideB.getOrigin();
}

Vector2& Triangle::getPointC()
{
    return _sideC.getOrigin();
}

Line& Triangle::getSideA()
{
    return _sideA;
}

Line& Triangle::getSideB()
{
    return _sideB;
}

Line& Triangle::getSideC()
{
    return _sideC;
}
