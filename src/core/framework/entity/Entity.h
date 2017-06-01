//
//  Entity.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/3/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Entity__
#define __noctisgames__Entity__

#include "RTTI.h"

class Entity
{
    RTTI_DECL;
    
public:
    Entity();
    
    virtual ~Entity();
    
    virtual void update();
    
    virtual void onDeletion();
    
    int getID();
    
    void setID(int inID);
    
    float getStateTime();
    
    void setStateTime(float stateTime);
    
    void requestDeletion();
    
    bool isRequestingDeletion();
    
protected:
    float m_fStateTime;
    int m_ID;
    bool m_isRequestingDeletion;
    
private:
    static int getUniqueEntityID();
};

#endif /* defined(__noctisgames__Entity__) */
