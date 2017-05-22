//
//  InputState.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__InputState__
#define __noctisgames__InputState__

class OutputMemoryBitStream;
class InputMemoryBitStream;

class InputState
{
public:
    InputState() :
    mDesiredRightAmount(0),
    mDesiredLeftAmount(0),
    mDesiredForwardAmount(0),
    mDesiredBackAmount(0)
    {}
    
    float GetDesiredHorizontalDelta()	const { return mDesiredRightAmount - mDesiredLeftAmount; }
    float GetDesiredVerticalDelta()		const { return mDesiredForwardAmount - mDesiredBackAmount; }
    
    bool Write(OutputMemoryBitStream& inOutputStream) const;
    bool Read(InputMemoryBitStream& inInputStream);
    
private:
    friend class InputManager;
    
    float	mDesiredRightAmount, mDesiredLeftAmount;
    float	mDesiredForwardAmount, mDesiredBackAmount;
};

#endif /* defined(__noctisgames__InputState__) */
