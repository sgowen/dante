//
//  Move.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Move__
#define __noctisgames__Move__

#include "RoboCatShared.h"

class Move
{
public:

	Move() {}

	Move(const InputState& inInputState, float inTimestamp, float inDeltaTime) :
		mInputState(inInputState),
		mTimestamp(inTimestamp),
		mDeltaTime(inDeltaTime)
	{}


	const InputState&	GetInputState()	const		{ return mInputState; }
	float				GetTimestamp()	const		{ return mTimestamp; }
	float				GetDeltaTime()	const		{ return mDeltaTime; }

	bool Write(OutputMemoryBitStream& inOutputStream) const;
	bool Read(InputMemoryBitStream& inInputStream);

private:
	InputState	mInputState;
	float		mTimestamp;
	float		mDeltaTime;

};

#endif /* defined(__noctisgames__Move__) */
