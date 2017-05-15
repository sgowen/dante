//
//  Move.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "RoboCatShared.h"

bool Move::Write(OutputMemoryBitStream& inOutputStream) const
{
	mInputState.Write(inOutputStream);
	inOutputStream.Write(mTimestamp);

	return true;
}

bool Move::Read(InputMemoryBitStream& inInputStream)
{
	mInputState.Read(inInputStream);
	inInputStream.Read(mTimestamp);

	return true;
}
