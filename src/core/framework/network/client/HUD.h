//
//  HUD.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__HUD__
#define __noctisgames__HUD__

#include "RoboMath.h"

#include <memory>

class HUD
{
public:
    static void StaticInit();
    static std::unique_ptr<HUD> sInstance;
    
    void SetPlayerHealth(int inHealth) { mHealth = inHealth; }
    
    Vector3 mBandwidthOrigin;
    Vector3 mRoundTripTimeOrigin;
    Vector3 mScoreBoardOrigin;
    Vector3 mScoreOffset;
    Vector3 mHealthOffset;
    int mHealth;
    
private:
    HUD();
};

#endif /* defined(__noctisgames__HUD__) */
