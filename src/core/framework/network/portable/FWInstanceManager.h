//
//  FWInstanceManager.h
//  dante
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__FWInstanceManager__
#define __noctisgames__FWInstanceManager__

class EntityManager;

class FWInstanceManager
{
public:
    static EntityManager* getClientEntityManager();
    static EntityManager* getServerEntityManager();
    
private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    FWInstanceManager();
    ~FWInstanceManager();
    FWInstanceManager(const FWInstanceManager&);
    FWInstanceManager& operator=(const FWInstanceManager&);
};

#endif /* defined(__noctisgames__FWInstanceManager__) */
