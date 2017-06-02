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

class Robot;
class Entity;

class World
{
public:
    static std::unique_ptr<World> sInstance;
    
    static void staticInit();
    
    static void addEntityIfPossible(Entity* inEntity);
    
    static void removeEntityIfPossible(Entity* inEntity);
    
    void AddRobot(Robot* inRobot);
    void RemoveRobot(Robot* inRobot);
    
    void update();
    
    const std::vector<Robot*>& GetRobots() const { return mRobots; }
    
private:
    std::vector<Robot*> mRobots;
    
    int	GetIndexOfRobot(Robot* inRobot);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    World();
    World(const World&);
    World& operator=(const World&);
};

#endif /* defined(__noctisgames__World__) */
