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

class KeyboardEvent
{
public:
    KeyboardEvent(KeyboardEventType type = KeyboardEventType_ARROW_KEY_RIGHT, bool isUp = false);

    KeyboardEventType getType();
    
    void setType(KeyboardEventType type);
    
    bool isUp();
    
    void setUp(bool isUp);
    
private:
    KeyboardEventType m_type;
    bool m_isUp;
};

#endif /* defined(__noctisgames__KeyboardEvent__) */
