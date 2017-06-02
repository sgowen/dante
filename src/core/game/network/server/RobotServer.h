//
//  RobotServer.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RobotServer__
#define __noctisgames__RobotServer__

#include "Robot.h"

class RobotServer : public Robot
{
    RTTI_DECL;
    
public:
    static Entity* create();
    
    virtual void onDeletion();
    
    virtual void update();
    
    void takeDamage();
    
private:
    float m_fTimeOfNextShot;
    
    void handleShooting();
    
    RobotServer();
};

#endif /* defined(__noctisgames__RobotServer__) */
