//
//  NGSteamAddress.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NGSteamAddress.h"

#include "StringUtil.h"

NGSteamAddress::NGSteamAddress(CSteamID steamID) : IMachineAddress(), m_steamID(steamID), m_isReliable(false)
{
    // Empty
}

NGSteamAddress::NGSteamAddress() : IMachineAddress(), m_steamID(CSteamID()), m_isReliable(false)
{
    // Empty
}

NGSteamAddress::~NGSteamAddress()
{
    // Empty
}

IMachineAddress* NGSteamAddress::createNewCopy()
{
    return new NGSteamAddress(m_steamID);
}

uint64_t NGSteamAddress::getHash() const
{
    return m_steamID.ConvertToUint64();
}

std::string NGSteamAddress::toString() const
{
    return StringUtil::format("Steam ID %llu", m_steamID.ConvertToUint64());
}

void NGSteamAddress::setSteamID(CSteamID steamID)
{
    m_steamID = steamID;
}

CSteamID& NGSteamAddress::getSteamID()
{
    return m_steamID;
}

void NGSteamAddress::setReliable(bool isReliable)
{
    m_isReliable = isReliable;
}

bool NGSteamAddress::isReliable()
{
    return m_isReliable;
}
