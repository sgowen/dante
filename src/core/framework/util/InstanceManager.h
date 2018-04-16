//
//  InstanceManager.h
//  dante
//
//  Created by Stephen Gowen on 3/28/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__InstanceManager__
#define __noctisgames__InstanceManager__

#define INSTANCE_MANAGER (InstanceManager::getInstance())

#include <map>

class InstanceManager
{
public:
    static InstanceManager* getInstance();
    
    void* getInstance(uint32_t key);
    void registerInstance(uint32_t key, void* instance);
    
private:
    std::map<uint32_t, void*> _instanceMap;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    InstanceManager();
    ~InstanceManager();
    InstanceManager(const InstanceManager&);
    InstanceManager& operator=(const InstanceManager&);
};

#endif /* defined(__noctisgames__InstanceManager__) */
