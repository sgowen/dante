//
//  ScreenEvent.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ScreenEvent__
#define __noctisgames__ScreenEvent__

#include "ScreenEventType.h"

class ScreenEvent
{
private:
    ScreenEventType m_type;
    float m_fX;
    float m_fY;
    
public:
    ScreenEvent(ScreenEventType type = ScreenEventType_DOWN, float x = 0, float y = 0);

	ScreenEventType getType();

	void setType(ScreenEventType type);

	float getX();

	void setX(float x);

	float getY();

	void setY(float y);
};

#endif /* defined(__noctisgames__ScreenEvent__) */
