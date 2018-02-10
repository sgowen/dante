//
//  Line.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Line__
#define __noctisgames__Line__

#include <framework/math/Vector2.h>

class Line
{
public:
    Line(float oX, float oY, float eX, float eY);
    
    Vector2& getOrigin();
    Vector2& getEnd();
    
private:
    Vector2 _origin;
    Vector2 _end;
};

#endif /* defined(__noctisgames__Line__) */
