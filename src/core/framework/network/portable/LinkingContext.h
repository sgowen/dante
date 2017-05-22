//
//  LinkingContext.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__LinkingContext__
#define __noctisgames__LinkingContext__

#include <unordered_map>

class GameObject;

class LinkingContext
{
public:
    LinkingContext();
    
    uint32_t GetNetworkId(GameObject* inGameObject, bool inShouldCreateIfNotFound);
    
    GameObject* GetGameObject(uint32_t inNetworkId) const;
    
    void AddGameObject(GameObject* inGameObject, uint32_t inNetworkId);
    
    void RemoveGameObject(GameObject *inGameObject);
    
private:
    std::unordered_map<uint32_t, GameObject*> mNetworkIdToGameObjectMap;
    std::unordered_map<const GameObject*, uint32_t> mGameObjectToNetworkIdMap;
    
    uint32_t mNextNetworkId;
};

#endif /* defined(__noctisgames__LinkingContext__) */
