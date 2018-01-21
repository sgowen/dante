//
//  Util.cpp
//  dante
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/Util.h"

#include "Box2D/Box2D.h"

#include "framework/network/client/NetworkManagerClient.h"
#include "game/logic/InstanceManager.h"
#include "game/logic/World.h"
#include "framework/audio/portable/NGAudioEngine.h"
#include "framework/entity/Entity.h"

void Util::playSound(int soundId, const b2Vec2& position)
{
    float volume = 1;
    float robotVolume = 0;
    
    std::map<uint8_t, uint8_t> indexToPlayerIdMap = NG_CLIENT->getPlayerIds();
    
    for (auto const &entry : indexToPlayerIdMap)
    {
        uint8_t playerId = entry.second;
        
        Entity* playerRobot = InstanceManager::getClientWorld()->getPlayerWithId(playerId);
        
        if (playerRobot)
        {
            float distance = b2Distance(playerRobot->getPosition(), position);
            
            float factor = distance / 5.0f;
            
            if (distance > 0 && factor > 0)
            {
                float newRobotVolume = 1.0f / (factor * factor);
                
                if (newRobotVolume > robotVolume)
                {
                    robotVolume = newRobotVolume;
                }
            }
            else
            {
                robotVolume = 1;
            }
        }
    }
    
    volume = robotVolume;
    
    NG_AUDIO_ENGINE->playSound(soundId, volume);
}

Util::Util()
{
    // Empty
}

Util::~Util()
{
    // Empty
}
