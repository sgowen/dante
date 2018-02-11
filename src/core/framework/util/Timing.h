//
//  Timing.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Timing__
#define __noctisgames__Timing__

#define NG_TIME (Timing::getInstance())

class Timing
{
public:
    static Timing* getInstance();
    
    void setTime(float stateTime);
    float getTime() const;
    
private:
    float _frameStartTime;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    Timing();
    ~Timing();
    Timing(const Timing&);
    Timing& operator=(const Timing&);
};

#endif /* defined(__noctisgames__Timing__) */
