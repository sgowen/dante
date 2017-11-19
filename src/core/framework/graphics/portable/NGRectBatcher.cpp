//
//  NGRectBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/25/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/NGRectBatcher.h"

#include "framework/math/NGRect.h"
#include "framework/math/Vector2.h"
#include "framework/math/Color.h"

NGRectBatcher::NGRectBatcher(bool isFill) : _isFill(isFill), _numNGRects(0)
{
    // Empty
}

NGRectBatcher::~NGRectBatcher()
{
    // Empty
}

void NGRectBatcher::renderNGRect(NGRect &rectangle, Color &color)
{
    float x1 = rectangle.getLeft();
    float y1 = rectangle.getBottom();
    float x2 = x1 + rectangle.getWidth();
    float y2 = y1 + rectangle.getHeight();
    
    renderNGRect(x1, y1, x2, y2, color);
}
