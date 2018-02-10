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

Util* Util::getInstance()
{
    static Util instance = Util();
    return &instance;
}

void Util::playSound(int soundId, const b2Vec2& position)
{
    float volume = 1;
    float robotVolume = 0;
    
    std::vector<Entity*>& players = _world->getPlayers();
    
    for (Entity* e : players)
    {
        float distance = b2Distance(e->getPosition(), position);
        
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
