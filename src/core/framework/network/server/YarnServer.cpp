//
//  YarnServer.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "YarnServer.h"


YarnServer::YarnServer()
{
	//yarn lives a second...
	mTimeToDie = Timing::sInstance.GetFrameStartTime() + 1.f;
}

void YarnServer::HandleDying()
{
	NetworkManagerServer::sInstance->UnregisterGameObject(this);
}


void YarnServer::Update()
{
	Yarn::Update();

	if (Timing::sInstance.GetFrameStartTime() > mTimeToDie)
	{
		SetDoesWantToDie(true);
	}

}

bool YarnServer::HandleCollisionWithCat(RoboCat* inCat)
{
	if (inCat->GetPlayerId() != GetPlayerId())
	{
		//kill yourself!
		SetDoesWantToDie(true);

		static_cast< RoboCatServer* >(inCat)->TakeDamage(GetPlayerId());

	}

	return false;
}


