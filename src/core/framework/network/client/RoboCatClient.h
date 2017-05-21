//
//  RoboCatClient.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RoboCatClient__
#define __noctisgames__RoboCatClient__

#include "RoboCat.h"

class RoboCatClient : public RoboCat
{
public:
	static GameObjectPtr StaticCreate() { return GameObjectPtr(new RoboCatClient()); }

	virtual void Update();
	virtual void HandleDying() override;

	virtual void Read(InputMemoryBitStream& inInputStream) override;

	void DoClientSidePredictionAfterReplicationForLocalCat(uint32_t inReadState);
	void DoClientSidePredictionAfterReplicationForRemoteCat(uint32_t inReadState);

protected:
	RoboCatClient();

private:
	void InterpolateClientSidePrediction(float inOldRotation, const Vector3& inOldLocation, const Vector3& inOldVelocity, bool inIsForRemoteCat);

	float mTimeLocationBecameOutOfSync;
	float mTimeVelocityBecameOutOfSync;
};

#endif /* defined(__noctisgames__RoboCatClient__) */
