//
//  RoboCatServer.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RoboCatServer__
#define __noctisgames__RoboCatServer__

#include "RoboCat.h"

#include "NetworkManagerServer.h"

class RoboCatServer : public RoboCat
{
public:
    static GameObjectPtr StaticCreate() { return NetworkManagerServer::sInstance->RegisterAndReturn(new RoboCatServer()); }
    virtual void HandleDying() override;
    
    virtual void Update() override;
    
protected:
    RoboCatServer();
    
private:
    float m_fTimeOfNextShot;
    float m_fTimeBetweenShots;
};

#endif /* defined(__noctisgames__RoboCatServer__) */
