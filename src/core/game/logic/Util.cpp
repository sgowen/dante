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

void Util::playSound(int soundId, uint32_t playerId, Vector2& position, bool isServer)
{
    if (isServer)
    {
        // Don't play sounds on the server
        return;
    }
    
    float volume = 1;
    
    uint32_t clientPlayerId = NG_CLIENT->getPlayerId();
    if (playerId != clientPlayerId)
    {
        Robot* playerRobot = InstanceManager::getClientWorld()->getRobotWithPlayerId(clientPlayerId);
        if (playerRobot)
        {
            float distance = playerRobot->getPosition().dist(position);
            float factor = distance / 4.0f;
            volume = 1.0f / (factor * factor);
        }
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
