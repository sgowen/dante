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

class Robot;
class Entity;

class World
{
public:
    static World* getInstance();
    
    static void addEntityIfPossible(Entity* inEntity);
    
    static void removeEntityIfPossible(Entity* inEntity);
    
    void addRobot(Robot* inRobot);
    
    void removeRobot(Robot* inRobot);
    
    void update();
    
    const std::vector<Robot*>& getRobots() const;
    
private:
    std::vector<Robot*> m_robots;
    
    int	getIndexOfRobot(Robot* inRobot);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    World();
    World(const World&);
    World& operator=(const World&);
};

#endif /* defined(__noctisgames__World__) */
