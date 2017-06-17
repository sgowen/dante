//
//  NGSteamAddress.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSteamAddress__
#define __noctisgames__NGSteamAddress__

#include "IMachineAddress.h"

#include "NGSteam.h"

class NGSteamAddress : public IMachineAddress
{
public:
    NGSteamAddress(CSteamID steamID);
    
    virtual ~NGSteamAddress();
    
    virtual IMachineAddress* createCopy();
    
    virtual size_t getHash() const;
    
    virtual std::string toString() const;
    
private:
    CSteamID m_steamID;
};

#endif /* defined(__noctisgames__NGSteamAddress__) */
