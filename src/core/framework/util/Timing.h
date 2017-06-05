//
//  Timing.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Timing__
#define __noctisgames__Timing__

class Timing
{
public:
    static Timing* getInstance();
    
    void update();
    
    void updateManual(float stateTime, float deltaTime);
    
    float getDeltaTime() const;
    
    float getFrameStartTime() const;
    
private:
    double m_dLastFrameStartTime;
    double m_dPerfCountDuration;
    float m_fFrameStartTimef;
    float m_fDeltaTime;
    
    float getTime() const;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    Timing();
    Timing(const Timing&);
    Timing& operator=(const Timing&);
};

#endif /* defined(__noctisgames__Timing__) */
