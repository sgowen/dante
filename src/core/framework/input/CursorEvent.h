//
//  CursorEvent.h
//  noctisgames
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__CursorEvent__
#define __noctisgames__CursorEvent__

#include <framework/input/CursorEventType.h>

class CursorEvent
{
public:
    CursorEvent(CursorEventType type = CursorEventType_DOWN, float x = 0, float y = 0, bool isAlt = false);

	CursorEventType getType();
	void setType(CursorEventType type);
	float getX();
	void setX(float x);
	float getY();
	void setY(float y);
    bool isAlt();
    void setAlt(bool value);
    
private:
    CursorEventType _type;
    float _x;
    float _y;
    bool _isAlt;
};

#endif /* defined(__noctisgames__CursorEvent__) */
