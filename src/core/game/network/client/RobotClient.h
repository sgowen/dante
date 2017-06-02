//
//  RobotClient.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RobotClient__
#define __noctisgames__RobotClient__

#include "Robot.h"

class RobotClient : public Robot
{
    RTTI_DECL;
    
public:
    static Entity* create();
    
    virtual void update();
    
    virtual void onDeletion();
    
    virtual void read(InputMemoryBitStream& inInputStream) override;
    
    void doClientSidePredictionAfterReplicationForLocalCat(uint32_t inReadState);
    
    void doClientSidePredictionAfterReplicationForRemoteCat(uint32_t inReadState);
    
private:
    float m_fTimeLocationBecameOutOfSync;
    float m_fTimeVelocityBecameOutOfSync;
    
    void interpolateClientSidePrediction(Vector2& inOldLocation, Vector2& inOldVelocity, bool inIsForRemoteCat);
    
    RobotClient();
};

#endif /* defined(__noctisgames__RobotClient__) */
