//
//  RodentClient.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RodentClient__
#define __noctisgames__RodentClient__

#include "RoboCatClientPCH.h"

class RodentClient : public Rodent
{
public:
	static	GameObjectPtr	StaticCreate()		{ return GameObjectPtr(new RodentClient()); }

protected:
	RodentClient();
};

#endif /* defined(__noctisgames__RodentClient__) */
