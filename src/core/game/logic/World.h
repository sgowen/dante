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

class World
{
public:
    static void StaticInit();
    
    static std::unique_ptr<World> sInstance;
    
    void AddRoboCat(RoboCat* inRoboCat);
    void RemoveRoboCat(RoboCat* inRoboCat);
    
    void update();
    
    const std::vector<RoboCat*>& GetRoboCats() const { return mRoboCats; }
    
private:
    World();
    
    int	GetIndexOfRoboCat(RoboCat* inRoboCat);
    
    std::vector<RoboCat*> mRoboCats;
};

#endif /* defined(__noctisgames__World__) */
