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
    InputState();
    
    bool write(OutputMemoryBitStream& inOutputStream) const;
    bool read(InputMemoryBitStream& inInputStream);
    
    float getDesiredHorizontalDelta() const;
    float getDesiredVerticalDelta() const;
    
private:
    friend class InputManager;
    
    float m_fDesiredRightAmount;
    float m_fDesiredLeftAmount;
    float m_fDesiredForwardAmount;
    float m_fDesiredBackAmount;
};

#endif /* defined(__noctisgames__InputState__) */
