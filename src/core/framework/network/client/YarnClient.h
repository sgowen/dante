//
//  YarnClient.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__YarnClient__
#define __noctisgames__YarnClient__

#include "RoboCatClientPCH.h"

class YarnClient : public Yarn
{
public:
	static	GameObjectPtr	StaticCreate()		{ return GameObjectPtr(new YarnClient()); }

	virtual void		Read(InputMemoryBitStream& inInputStream) override;
	virtual bool		HandleCollisionWithCat(RoboCat* inCat) override;

protected:
	YarnClient();
};

#endif /* defined(__noctisgames__YarnClient__) */
