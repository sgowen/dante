//
//  YarnClient.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "RoboCatClientPCH.h"

YarnClient::YarnClient()
{
    // Empty
}

void YarnClient::Read(InputMemoryBitStream& inInputStream)
{
	bool stateBit;

	inInputStream.Read(stateBit);
	if (stateBit)
	{
		Vector3 location;
		inInputStream.Read(location.mX);
		inInputStream.Read(location.mY);

		Vector3 velocity;
		inInputStream.Read(velocity.mX);
		inInputStream.Read(velocity.mY);
		SetVelocity(velocity);

		//dead reckon ahead by rtt, since this was spawned a while ago!
		SetLocation(location + velocity * NetworkManagerClient::sInstance->GetRoundTripTime());

		float rotation;
		inInputStream.Read(rotation);
		SetRotation(rotation);
	}

	inInputStream.Read(stateBit);
	if (stateBit)
	{	
		Vector3 color;
		inInputStream.Read(color);
		SetColor(color);
	}

	inInputStream.Read(stateBit);
	if (stateBit)
	{	
		inInputStream.Read(mPlayerId, 8);
	}
}

bool YarnClient::HandleCollisionWithCat(RoboCat* inCat)
{
	if (GetPlayerId() != inCat->GetPlayerId())
	{
        // you look like you hit a cat on the client, so disappear (whether server registered or not
	}
    
	return false;
}
