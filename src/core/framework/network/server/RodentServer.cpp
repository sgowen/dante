//
//  RodentServer.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "RodentServer.h"


RodentServer::RodentServer()
{
}

void RodentServer::HandleDying()
{
	NetworkManagerServer::sInstance->UnregisterGameObject(this);
}


bool RodentServer::HandleCollisionWithCat(RoboCat* inCat)
{
	//kill yourself!
	SetDoesWantToDie(true);

	ScoreBoardManager::sInstance->IncScore(inCat->GetPlayerId(), 1);

	return false;
}


