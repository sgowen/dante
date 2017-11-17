//
//  CursorEvent.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__CursorEvent__
#define __noctisgames__CursorEvent__

#include "CursorEventType.h"

class CursorEvent
{
private:
    CursorEventType m_type;
    float m_fX;
    float m_fY;
    
public:
    CursorEvent(CursorEventType type = CursorEventType_DOWN, float x = 0, float y = 0);

	CursorEventType getType();

	void setType(CursorEventType type);

	float getX();

	void setX(float x);

	float getY();

	void setY(float y);
};

#endif /* defined(__noctisgames__CursorEvent__) */
