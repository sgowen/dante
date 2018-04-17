//
//  Timing.h
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Timing__
#define __noctisgames__Timing__

class Timing
{
public:
    Timing();
    ~Timing();
    
    void setTime(float stateTime);
    float getTime() const;
    
private:
    float _frameStartTime;
};

#endif /* defined(__noctisgames__Timing__) */
