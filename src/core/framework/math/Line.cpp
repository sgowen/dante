//
//  Line.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Line.h"

Line::Line(float x1, float y1, float x2, float y2) : _origin(x1, y1), _end(x2, y2)
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
