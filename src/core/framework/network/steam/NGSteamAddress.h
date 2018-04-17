//
//  NGSteamAddress.h
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSteamAddress__
#define __noctisgames__NGSteamAddress__

#include <framework/network/portable/MachineAddress.h>

#include <framework/network/steam/NGSteam.h>

class NGSteamAddress : public MachineAddress
{
public:
    NGSteamAddress(CSteamID steamID);
    
    NGSteamAddress();
    
    virtual ~NGSteamAddress();
    
    virtual MachineAddress* createNewCopy();
    
    virtual uint64_t getHash() const;
    
    virtual std::string toString() const;
    
    void setSteamID(CSteamID steamID);
    
    CSteamID& getSteamID();
    
    void setReliable(bool isReliable);
    
    bool isReliable();
    
private:
    CSteamID _steamID;
    bool _isReliable;
};

#endif /* defined(__noctisgames__NGSteamAddress__) */
