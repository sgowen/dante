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
