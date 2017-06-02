//
//  World.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__World__
#define __noctisgames__World__

#include <memory>
#include <vector>

class RoboCat;
class Entity;

class World
{
public:
    static std::unique_ptr<World> sInstance;
    
    static void staticInit();
    
    static void addEntityIfPossible(Entity* inEntity);
    
    static void removeEntityIfPossible(Entity* inEntity);
    
    void AddRoboCat(RoboCat* inRoboCat);
    void RemoveRoboCat(RoboCat* inRoboCat);
    
    void update();
    
    const std::vector<RoboCat*>& GetRoboCats() const { return mRoboCats; }
    
private:
    std::vector<RoboCat*> mRoboCats;
    
    int	GetIndexOfRoboCat(RoboCat* inRoboCat);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    World();
    World(const World&);
    World& operator=(const World&);
};

#endif /* defined(__noctisgames__World__) */
