//
//  Rodent.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Rodent__
#define __noctisgames__Rodent__

#include "RoboCatShared.h"

class Rodent : public GameObject
{
public:
	CLASS_IDENTIFICATION('MOUS', GameObject)

	enum ERodentReplicationState
	{
		EMRS_Pose		= 1 << 0,
		EMRS_Color		= 1 << 1,

		EMRS_AllState	= EMRS_Pose | EMRS_Color
	};

	static	GameObject*	StaticCreate() { return new Rodent(); }

	virtual uint32_t	GetAllStateMask()	const override	{ return EMRS_AllState; }

	virtual uint32_t	Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const override;
	virtual void		Read(InputMemoryBitStream& inInputStream) override;

	virtual bool HandleCollisionWithCat(RoboCat* inCat) override;

protected:
	Rodent();
};

#endif /* defined(__noctisgames__Rodent__) */
