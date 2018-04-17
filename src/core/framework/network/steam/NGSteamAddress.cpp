//
//  NGSteamAddress.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/steam/NGSteamAddress.h>

#include <framework/util/StringUtil.h>

NGSteamAddress::NGSteamAddress(CSteamID steamID) : MachineAddress(), _steamID(steamID), _isReliable(false)
{
    // Empty
}

NGSteamAddress::NGSteamAddress() : MachineAddress(), _steamID(CSteamID()), _isReliable(false)
{
    // Empty
}

NGSteamAddress::~NGSteamAddress()
{
    // Empty
}

MachineAddress* NGSteamAddress::createNewCopy()
{
    return new NGSteamAddress(_steamID);
}

uint64_t NGSteamAddress::getHash() const
{
    return _steamID.ConvertToUint64();
}

std::string NGSteamAddress::toString() const
{
    return StringUtil::format("Steam ID %llu", _steamID.ConvertToUint64());
}

void NGSteamAddress::setSteamID(CSteamID steamID)
{
    _steamID = steamID;
}

CSteamID& NGSteamAddress::getSteamID()
{
    return _steamID;
}

void NGSteamAddress::setReliable(bool isReliable)
{
    _isReliable = isReliable;
}

bool NGSteamAddress::isReliable()
{
    return _isReliable;
}
