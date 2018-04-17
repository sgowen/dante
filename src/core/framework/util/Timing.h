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
    
    void onFrame();
    void reset();
    float getTime() const;
    
private:
    double _time;
};

#endif /* defined(__noctisgames__Timing__) */
