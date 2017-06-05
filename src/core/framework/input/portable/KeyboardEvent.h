//
//  KeyboardEvent.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__KeyboardEvent__
#define __noctisgames__KeyboardEvent__

#include "KeyboardEventType.h"

#define KEYBOARD_STATUS_DOWN 1
#define KEYBOARD_STATUS_HELD 2
#define KEYBOARD_STATUS_UP 3

class KeyboardEvent
{
public:
    KeyboardEvent(KeyboardEventType type = KeyboardEventType_ARROW_KEY_RIGHT, int status = KEYBOARD_STATUS_DOWN);

    KeyboardEventType getType();
    
    void setType(KeyboardEventType type);
    
    void setStatus(int status);
    
    int getStatus();
    
    bool isDown();
    
    bool isHeld();
    
    bool isUp();
    
private:
    KeyboardEventType m_type;
    int m_iStatus;
};

#endif /* defined(__noctisgames__KeyboardEvent__) */
