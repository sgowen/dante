//
//  Util.cpp
//  dante
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/logic/Util.h>

#include <framework/entity/Entity.h>
#include <Box2D/Box2D.h>
#include <game/logic/World.h>

#include <framework/audio/portable/NGAudioEngine.h>
#include <game/entity/PlayerController.h>
#include <game/game/GameEngine.h>

void Util::handleSound(Entity* e, uint8_t fromState, uint8_t toState)
{
    if (fromState == toState)
    {
        return;
    }
    
    playSound(e->getSoundMapping(toState), e->getPosition());
}

void Util::playSound(int soundId, const b2Vec2& position)
{
    if (soundId <= 0)
    {
        return;
    }
    
    World* w = GameEngine::getInstance()->getWorld();
    assert(w);
    std::vector<Entity*>& players = w->getPlayers();
    
    float volume = 1;
    float robotVolume = 1;
    
    for (Entity* e : players)
    {
        PlayerNetworkController* p = static_cast<PlayerNetworkController*>(e->getNetworkController());
        assert(p);
        if (!p->isLocalPlayer())
        {
            continue;
        }
        
        float distance = b2Distance(e->getPosition(), position);
        float factor = distance / 5.0f;
        
        if (distance > 0 && factor > 0)
        {
            float newRobotVolume = 1.0f / (factor * factor);
            if (newRobotVolume < robotVolume)
            {
                robotVolume = newRobotVolume;
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
