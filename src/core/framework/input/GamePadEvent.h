//
//  GamePadEvent.h
//  noctisgames
//
//  Created by Stephen Gowen on 1/21/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GamePadEvent__
#define __noctisgames__GamePadEvent__

#include <framework/input/GamePadEventType.h>

class GamePadEvent
{
public:
    GamePadEvent(GamePadEventType type = GamePadEventType_D_PAD_RIGHT, int index = 0, float x = 0, float y = 0);

    GamePadEventType getType();
    void setType(GamePadEventType type);
    int getIndex();
    void setIndex(int index);
	float getX();
	void setX(float x);
	float getY();
	void setY(float y);
    bool isPressed();
    
private:
    GamePadEventType _type;
    int _index;
    float _x;
    float _y;
};

#endif /* defined(__noctisgames__GamePadEvent__) */
