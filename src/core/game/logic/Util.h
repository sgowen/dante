//
//  Util.h
//  dante
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Util__
#define __noctisgames__Util__

#include "Network.h"

class Vector2;

class Util
{
public:
    static void playSound(int soundId, uint32_t playerId, Vector2& position, bool isServer);
    
private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    Util();
    ~Util();
    Util(const Util&);
    Util& operator=(const Util&);
};

#endif /* defined(__noctisgames__Util__) */
