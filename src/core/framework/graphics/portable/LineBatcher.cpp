//
//  LineBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/15/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "LineBatcher.h"

#include "Line.h"
#include "Vector2D.h"
#include "Color.h"

LineBatcher::LineBatcher() : m_iNumLines(0)
{
    // Empty
}

LineBatcher::~LineBatcher()
{
    // Empty
}

void LineBatcher::renderLine(Line &line, Color &c)
{
    float oX = line.getOrigin().getX();
    float oY = line.getOrigin().getY();
    float eX = line.getEnd().getX();
    float eY = line.getEnd().getY();
    
    renderLine(oX, oY, eX, eY, c);
}
