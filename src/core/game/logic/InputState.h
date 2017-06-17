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

#include "IInputState.h"

#include "RTTI.h"

class InputState : public IInputState
{
    RTTI_DECL;
    
public:
    InputState();
    
    virtual ~InputState();
    
    virtual bool write(OutputMemoryBitStream& inOutputStream) const;
    virtual bool read(InputMemoryBitStream& inInputStream);
    
    virtual void reset();
    
    void copyTo(InputState* inInputState);
    
    bool isEqualTo(InputState* inInputState);
    
    float getDesiredHorizontalDelta() const;
    
    bool isJumping() const;
    
    bool isShooting() const;
    
    bool isSprinting() const;
    
    bool isStartingServer() const;
    
    bool isJoiningServer() const;
    
    bool isStartingSteamServer() const;
    
    bool isJoiningOnlineSteamServer() const;
    
    bool isJoiningLANSteamServer() const;
    
    bool isLeavingServer() const;
    
private:
    friend class InputManager;
    
    float m_fDesiredRightAmount;
    float m_fDesiredLeftAmount;
    bool m_isJumping;
    bool m_isShooting;
    bool m_isSprinting;
    bool m_isStartingServer;
    bool m_isJoiningServer;
    bool m_isStartingSteamServer;
    bool m_isJoiningOnlineSteamServer;
    bool m_isJoiningLANSteamServer;
    bool m_isLeavingServer;
};

#endif /* defined(__noctisgames__InputState__) */
