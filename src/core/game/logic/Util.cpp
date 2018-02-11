//
//  Util.cpp
//  dante
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/logic/Util.h>

#include <Box2D/Box2D.h>
#include <game/logic/World.h>

#include <framework/audio/portable/NGAudioEngine.h>
#include <framework/entity/Entity.h>
#include <game/entity/PlayerController.h>

Util* Util::getInstance()
{
    static Util instance = Util();
    return &instance;
}

void Util::playSound(int soundId, const b2Vec2& position)
{
    float volume = 1;
    float robotVolume = 1;
    
    std::vector<Entity*>& players = _world->getPlayers();
    
    for (Entity* e : players)
    {
        PlayerController* robot = static_cast<PlayerController*>(e->getController());
        assert(robot);
        
        if (!robot->isLocalPlayer())
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

void Util::setWorld(World* world)
{
    _world = world;
}

Util::Util()
{
    // Empty
}

Util::~Util()
{
    // Empty
}
