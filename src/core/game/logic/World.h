//
//  World.h
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__World__
#define __noctisgames__World__

#include <memory>
#include <vector>

class Entity;
class Robot;

class World
{
public:
    World();
    
    ~World();
    
    void addEntity(Entity* inEntity);
    
    void removeEntity(Entity* inEntity);
    
    void update();
    
    Robot* getRobotWithPlayerId(int inPlayerID);
    
    bool hasSpacePirates();
    
    std::vector<Entity*>& getEntities();
    
private:
    std::vector<Entity*> m_entities;
};

#endif /* defined(__noctisgames__World__) */
