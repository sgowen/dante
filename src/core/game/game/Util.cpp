//
//  Util.cpp
//  dante
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/game/Util.h>

#include <framework/entity/Entity.h>
#include <Box2D/Box2D.h>

#include <framework/studio/World.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <game/entity/PlayerController.h>
#include <game/game/GameEngine.h>
#include <framework/util/Config.h>

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
    
    float volume = 1;
    float playerVolume = 1;
    
    World* w = GameEngine::getInstance()->getWorld();
    assert(w);
    
    std::vector<Entity*>& players = w->getPlayers();
    for (Entity* e : players)
    {
        PlayerNetworkController* p = static_cast<PlayerNetworkController*>(e->getNetworkController());
        assert(p);
        if (!p->isLocalPlayer())
        {
            continue;
        }
        
        float distance = b2Distance(e->getPosition(), position);
        if (distance > 0)
        {
            float factor = distance / (FW_CFG->_camWidth / 8);
            float newPlayerVolume = 1.0f / (factor * factor);
            if (newPlayerVolume < playerVolume)
            {
                playerVolume = newPlayerVolume;
            }
        }
    }
    
    volume = playerVolume;
    
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
