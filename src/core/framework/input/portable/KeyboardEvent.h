//
//  KeyboardEvent.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__KeyboardEvent__
#define __noctisgames__KeyboardEvent__

#define KEYBOARD_STATUS_DOWN 1
#define KEYBOARD_STATUS_HELD 2
#define KEYBOARD_STATUS_UP 3

class KeyboardEvent
{
public:
    KeyboardEvent(unsigned short key = 0, int status = KEYBOARD_STATUS_DOWN);

    void setKey(unsigned short key);
    
    unsigned short getKey();
    
    void setStatus(int status);
    
    int getStatus();
    
    bool isDown();
    
    bool isHeld();
    
    bool isUp();
    
private:
    unsigned short _sKey;
    int _status;
};

#endif /* defined(__noctisgames__KeyboardEvent__) */
