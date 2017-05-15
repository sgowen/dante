//
//  Timing.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Timing__
#define __noctisgames__Timing__

#include "RoboCatShared.h"

class Timing
{
public:

	Timing();
	
	void Update();
    
    void updateManual(float stateTime, float deltaTime);

	float GetDeltaTime() const { return mDeltaTime; }
    
    void setDeltaTime(float deltaTime) { mDeltaTime = deltaTime; }

	double GetTime() const;

	float GetTimef() const
	{
		return static_cast< float >(GetTime());
	}

	float GetFrameStartTime() const { return mFrameStartTimef; }


	static Timing sInstance; 

private:
	float		mDeltaTime;
	uint64_t	mDeltaTick;

	double		mLastFrameStartTime;
	float		mFrameStartTimef;
	double		mPerfCountDuration;
};

#endif /* defined(__noctisgames__Timing__) */
