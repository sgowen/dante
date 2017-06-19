//
//  NGSteamClientHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NGSteamClientHelper.h"

#include "NGSteamAddress.h"

#include "NGSteamPacketHandler.h"
#include "macros.h"
#include "StringUtil.h"
#include "Timing.h"
#include "OutputMemoryBitStream.h"

NGSteamClientHelper::NGSteamClientHelper(CSteamID serverSteamID, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc) : IClientHelper(new NGSteamPacketHandler(false, processPacketFunc, handleNoResponseFunc, handleConnectionResetFunc)),
m_serverSteamAddress(new NGSteamAddress(serverSteamID)),
m_clientSteamAddress(new NGSteamAddress(SteamUser()->GetSteamID()))
{
    m_name = std::string(SteamFriends()->GetFriendPersonaName(m_clientSteamAddress->getSteamID()));
}

NGSteamClientHelper::~NGSteamClientHelper()
{
    delete m_serverSteamAddress;
    delete m_clientSteamAddress;
}

void NGSteamClientHelper::processIncomingPackets()
{
    INetworkHelper::processIncomingPackets();
    
    // without this, callbacks will never fire
    SteamAPI_RunCallbacks();
}

void NGSteamClientHelper::sendPacket(const OutputMemoryBitStream& inOutputStream)
{
    if (m_serverSteamAddress)
    {
        INetworkHelper::sendPacket(inOutputStream, m_serverSteamAddress);
    }
}

std::string& NGSteamClientHelper::getName()
{
    return m_name;
}

//-----------------------------------------------------------------------------
// Purpose: Handle the server telling us it is exiting
//-----------------------------------------------------------------------------
void NGSteamClientHelper::onReceiveServerExiting()
{
    LOG("onReceiveServerExiting");
    
//    if (m_pP2PAuthedGame)
//    {
//        m_pP2PAuthedGame->EndGame();
//    }
//    
//    if (m_hAuthTicket != k_HAuthTicketInvalid)
//    {
//        SteamUser()->CancelAuthTicket(m_hAuthTicket);
//    }
//    
//    m_hAuthTicket = k_HAuthTicketInvalid;
//    
//    if (m_eGameState != k_EClientGameActive)
//    {
//        return;
//    }
//    
//    m_eConnectedStatus = k_EClientNotConnected;
//    
//    SetGameState(k_EClientGameConnectionFailure);
}

void NGSteamClientHelper::onSteamServersConnected(SteamServersConnected_t *callback)
{
    LOG("onSteamServersConnected");
}

void NGSteamClientHelper::onSteamServersDisconnected(SteamServersDisconnected_t *callback)
{
    LOG("onSteamServersDisconnected");
}

void NGSteamClientHelper::onSteamServerConnectFailure(SteamServerConnectFailure_t *callback)
{
    LOG("onSteamServerConnectFailure: %d", callback->m_eResult);
}

void NGSteamClientHelper::onP2PSessionConnectFail(P2PSessionConnectFail_t *pCallback)
{
    if (pCallback->m_steamIDRemote == m_serverSteamAddress->getSteamID())
    {
        // failed, error out
        LOG("Failed to make P2P connection, quiting server");
        onReceiveServerExiting();
    }
}
