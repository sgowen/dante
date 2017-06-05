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
    static World* getInstance();
    
    static void staticAddEntity(Entity* inEntity);
    
    static void staticRemoveEntity(Entity* inEntity);
    
    static Robot* staticGetRobotWithPlayerId(int inPlayerID);
    
    static bool staticHasSpacePirates();
    
    void addEntity(Entity* inEntity);
    
    void removeEntity(Entity* inEntity);
    
    void update();
    
    std::vector<Entity*>& getEntities();
    
private:
    std::vector<Entity*> m_objects;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    World();
    World(const World&);
    World& operator=(const World&);
};

#endif /* defined(__noctisgames__World__) */
