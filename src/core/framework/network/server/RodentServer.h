//
//  RodentServer.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RodentServer__
#define __noctisgames__RodentServer__

class RodentServer : public Rodent
{
public:
	static GameObjectPtr	StaticCreate() { return NetworkManagerServer::sInstance->RegisterAndReturn(new RodentServer()); }
	void HandleDying() override;
	virtual bool		HandleCollisionWithCat(RoboCat* inCat) override;

protected:
	RodentServer();
};

#endif /* defined(__noctisgames__RodentServer__) */
