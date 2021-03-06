//
//  NGRect.h
//  noctisgames
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGRect__
#define __noctisgames__NGRect__

#include <framework/math/Vector2.h>

class NGRect
{
public:
    NGRect(float x, float y, float width, float height, float angle = 0);
    
    Vector2& getLowerLeft();
    float getRight();
    float getTop();
    float getLeft();
    float getBottom();
    float getWidth();
    void setWidth(float width);
    float getHeight();
    void setHeight(float height);
    float getAngle();
    void setAngle(float angle);
    
private:
    Vector2 _lowerLeft;
    float _width;
    float _height;
    float _angle;
};

#endif /* defined(__noctisgames__NGRect__) */
