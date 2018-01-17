//
//  Line.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/math/Line.h"

Line::Line(float oX, float oY, float eX, float eY) : _origin(oX, oY), _end(eX, eY)
{
    // Empty
}

Vector2& Line::getOrigin()
{
    return _origin;
}

Vector2& Line::getEnd()
{
    return _end;
}
