//
//  Util.cpp
//  dante
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Util.h"

#include "Vector2.h"

#include "NetworkManagerClient.h"
#include "Robot.h"
#include "InstanceManager.h"
#include "World.h"
#include "NGAudioEngine.h"

void Util::playSound(int soundId, Vector2& position, bool isServer)
{
    if (isServer)
    {
        // Don't play sounds on the server
        return;
    }
    
    float volume = 1;
    float robotVolume = 0;
    
    if (InstanceManager::getClientWorld())
    {
        std::vector<uint8_t> playerIds = NG_CLIENT->getPlayerIds();
        
        for (uint8_t playerId : playerIds)
        {
            Robot* playerRobot = InstanceManager::getClientWorld()->getRobotWithPlayerId(playerId);
            
            if (playerRobot)
            {
                float distance = playerRobot->getPosition().dist(position);
                
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
    }
    
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
