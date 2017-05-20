//
//  AckRange.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__AckRange__
#define __noctisgames__AckRange__

#include "InFlightPacket.h"

class OutputMemoryBitStream;
class InputMemoryBitStream;

class AckRange
{
public:
	AckRange() : mStart(0), mCount(0) {}
	
	AckRange(PacketSequenceNumber inStart) : mStart(inStart), mCount(1) {}
	
	//if this is the next in sequence, just extend the range
	inline bool ExtendIfShould(PacketSequenceNumber inSequenceNumber);
	
	PacketSequenceNumber	GetStart() const { return mStart; }
	uint32_t		GetCount() const { return mCount; }
	
	void Write(OutputMemoryBitStream& inOutputStream) const;
	void Read(InputMemoryBitStream& inInputStream);
	
private:
	PacketSequenceNumber	mStart;
	uint32_t		mCount;
};

inline bool AckRange::ExtendIfShould(PacketSequenceNumber inSequenceNumber)
{
	if (inSequenceNumber == mStart + mCount)
	{
		++mCount;
		return true;
	}
	else
	{
		return false;
	}
}

#endif /* defined(__noctisgames__AckRange__) */
