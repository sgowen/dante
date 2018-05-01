//
//  NGSteamClientHelper.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/steam/NGSteamClientHelper.h>

#include <framework/network/steam/NGSteamP2PAuth.h>
#include <framework/network/steam/NGSteamAddress.h>

#include <framework/network/steam/NGSteamPacketHandler.h>
#include <framework/util/macros.h>
#include <framework/util/StringUtil.h>
#include <framework/util/Timing.h>
#include <framework/network/portable/OutputMemoryBitStream.h>
#include <framework/network/steam/NGSteamGameServices.h>
#include <framework/network/server/NetworkManagerServer.h>
#include <framework/network/steam/NGSteamServerHelper.h>
#include <framework/util/InstanceManager.h>

NGSteamClientHelper::NGSteamClientHelper(CSteamID inServerSteamID, GetPlayerAddressHashFunc inGetPlayerAddressHashFunc, ProcessPacketFunc inProcessPacketFunc, HandleNoResponseFunc inHandleNoResponseFunc, HandleConnectionResetFunc inHandleConnectionResetFunc) : ClientHelper(new NGSteamPacketHandler(static_cast<Timing*>(INSTANCE_MANAGER->get(INSTANCE_TIME_CLIENT)), false, inProcessPacketFunc, inHandleNoResponseFunc, inHandleConnectionResetFunc)),
_steamP2PAuth(new NGSteamP2PAuth(this)),
_getPlayerAddressHashFunc(inGetPlayerAddressHashFunc),
_eConnectedStatus(k_EClientNotConnected),
_serverSteamAddress(new NGSteamAddress(inServerSteamID)),
_timeOfLastMsgClientBeginAuthentication(0.0f),
_unServerIP(0),
_usServerPort(0),
_hAuthTicket(k_HAuthTicketInvalid)
{
    _name = std::string(SteamFriends()->GetFriendPersonaName(SteamUser()->GetSteamID()));

    LOG("Client %s is connecting to Game Server with Steam ID: %s", _name.c_str(), _serverSteamAddress->toString().c_str());

    NG_STEAM_GAME_SERVICES->onServerJoined();
}

NGSteamClientHelper::~NGSteamClientHelper()
{
    if (_hAuthTicket != k_HAuthTicketInvalid)
    {
        SteamUser()->CancelAuthTicket(_hAuthTicket);
    }

    OutputMemoryBitStream packet;
    packet.write(static_cast<uint8_t>(k_EMsgClientLeavingServer));
    sendPacket(packet);

    _steamP2PAuth->endGame();

    delete _steamP2PAuth;
    _steamP2PAuth = NULL;

    if (_eConnectedStatus == k_EClientConnectedAndAuthenticated)
    {
        SteamUser()->TerminateGameConnection(_unServerIP, _usServerPort);
    }

    if (_serverSteamAddress->getSteamID().IsValid())
    {
        SteamNetworking()->CloseP2PSessionWithUser(_serverSteamAddress->getSteamID());
    }

    delete _serverSteamAddress;
}

void NGSteamClientHelper::processIncomingPackets()
{
    NetworkHelper::processIncomingPackets();

    // has the player list changed?
    if (NG_SERVER)
    {
        // if i am the server owner i need to auth everyone who wants to play
        // assume i am in slot 0, so start at slot 1
        for (uint32_t i = 1; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
        {
            CSteamID steamIDNew;
            steamIDNew.SetFromUint64(_getPlayerAddressHashFunc(i));
            if (steamIDNew == SteamUser()->GetSteamID())
            {
                LOG("Server player slot 0 is not server owner.");
            }
            else if (steamIDNew != _rgSteamIDPlayers[i])
            {
                if (_rgSteamIDPlayers[i].IsValid())
                {
                    _steamP2PAuth->playerDisconnect(i);
                }
                if (steamIDNew.IsValid())
                {
                    _steamP2PAuth->registerPlayer(i, steamIDNew);
                }
            }
        }
    }
    else
    {
        // i am just a client, i need to auth the game owner (slot 0)
        CSteamID steamIDNew;
        steamIDNew.SetFromUint64(_getPlayerAddressHashFunc(0));
        if (steamIDNew == SteamUser()->GetSteamID())
        {
            LOG("Server player slot 0 is not server owner.");
        }
        else if (steamIDNew != _rgSteamIDPlayers[0])
        {
            if (_rgSteamIDPlayers[0].IsValid())
            {
                LOG("Server player slot 0 has disconnected - but thats the server owner.");
                _steamP2PAuth->playerDisconnect(0);
            }
            if (steamIDNew.IsValid())
            {
                _steamP2PAuth->startAuthPlayer(0, steamIDNew);
            }
        }
    }

    for (uint32_t i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        // Update steamid array with data from server
        // This is actually sort of hacky...
        // It only works because we know that the hash of a NGSteamAddress
        // object is actually the CSteamID in uint64 form
        _rgSteamIDPlayers[i].SetFromUint64(_getPlayerAddressHashFunc(i));
    }

    if (NG_SERVER)
    {
        // Now if we are the owner of the game, lets make sure all of our players are legit.
        // if they are not, we tell the server to kick them off
        // Start at 1 to skip myself
        for (uint8_t i = 1; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
        {
            if (_steamP2PAuth->_rgpP2PAuthPlayer[i] && !_steamP2PAuth->_rgpP2PAuthPlayer[i]->isAuthOk())
            {
                NGSteamServerHelper* helper = static_cast<NGSteamServerHelper*>(NG_SERVER->getServerHelper());
                helper->kickPlayerOffServer(_steamP2PAuth->_rgpP2PAuthPlayer[i]->_steamID);
            }
        }
    }
    else
    {
        // If we are not the owner of the game, lets make sure the game owner is legit
        // if he is not, we leave the game
        if (_steamP2PAuth->_rgpP2PAuthPlayer[0])
        {
            if (!_steamP2PAuth->_rgpP2PAuthPlayer[0]->isAuthOk())
            {
                // leave the game
                _eConnectedStatus = k_EServerIsNotAuthorized;
                updateState();
            }
        }
    }
}

void NGSteamClientHelper::processSpecialPacket(uint8_t packetType, InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress)
{
    if (inFromAddress->getHash() == _serverSteamAddress->getHash())
    {
        switch (packetType)
        {
            case k_EMsgServerSendInfo:
            {
                if (_eConnectedStatus == k_EClientNotConnected)
                {
                    uint64 steamIDGameServer;
                    bool bVACSecure;
                    std::string serverName;
                    inInputStream.read(steamIDGameServer);
                    inInputStream.read(bVACSecure);
                    inInputStream.readLarge(serverName);

                    onReceiveServerInfo(CSteamID(steamIDGameServer), bVACSecure, serverName.c_str());
                }
            }
                break;
            case k_EMsgServerPassAuthentication:
                _eConnectedStatus = k_EClientConnectedAndAuthenticated;
                // set information so our friends can join the lobby
                updateRichPresenceConnectionInfo();
                break;
            case k_EMsgServerFailAuthentication:
                LOG("Connection failure. Multiplayer authentication failed");
                _eConnectedStatus = k_EClientConnectionFailure;
                break;
            case k_EMsgServerExiting:
                LOG("Server Shutting Down");
                _eConnectedStatus = k_EServerShuttingDown;

                _steamP2PAuth->endGame();
            default:
                break;
        }
    }
    else
    {
        _steamP2PAuth->handleMessage(packetType, inInputStream);
    }

    updateState();
}

void NGSteamClientHelper::handleUninitialized()
{
    switch (_eConnectedStatus)
    {
        case k_EClientNotConnected:
        {
            OutputMemoryBitStream packet;
            packet.write(static_cast<uint8_t>(k_EMsgClientInitiateConnection));
            sendPacket(packet);
        }
            break;
        case k_EClientConnectedPendingAuthentication:
        {
            Timing* timing = static_cast<Timing*>(INSTANCE_MANAGER->get(INSTANCE_TIME_CLIENT));
            float time = timing->getTime();

            if (time > _timeOfLastMsgClientBeginAuthentication + 7.0f)
            {
                char rgchToken[1024];
                uint32_t unTokenLen = 0;
                _hAuthTicket = SteamUser()->GetAuthSessionTicket(rgchToken, sizeof(rgchToken), &unTokenLen);

                Steamworks_TestSecret();

                if (unTokenLen < 1)
                {
                    LOG("Warning: Looks like GetAuthSessionTicket didn't give us a good ticket");
                }

                LOG("MsgClientBeginAuthentication_t, uTokenLen: %i", unTokenLen);

                OutputMemoryBitStream packet;
                packet.write(static_cast<uint8_t>(k_EMsgClientBeginAuthentication));
                packet.write(unTokenLen);
                packet.writeBytes(rgchToken, unTokenLen);
                sendPacket(packet);

                _timeOfLastMsgClientBeginAuthentication = time;
            }
        }
            break;
        default:
            break;
    }

    updateState();
}

void NGSteamClientHelper::sendPacket(const OutputMemoryBitStream& inOutputStream)
{
    NetworkHelper::sendPacket(inOutputStream, _serverSteamAddress);
}

std::string& NGSteamClientHelper::getName()
{
    return _name;
}

void NGSteamClientHelper::onReceiveServerInfo(CSteamID steamIDGameServer, bool bVACSecure, const char *pchServerName)
{
    _eConnectedStatus = k_EClientConnectedPendingAuthentication;

    _serverSteamAddress->setSteamID(steamIDGameServer);

    // look up the servers IP and Port from the connection
    P2PSessionState_t p2pSessionState;
    SteamNetworking()->GetP2PSessionState(steamIDGameServer, &p2pSessionState);
    _unServerIP = p2pSessionState.m_nRemoteIP;
    _usServerPort = p2pSessionState.m_nRemotePort;

    // set how to connect to the game server, using the Rich Presence API
    // this lets our friends connect to this game via their friends list
    updateRichPresenceConnectionInfo();
}

void NGSteamClientHelper::updateRichPresenceConnectionInfo()
{
    // connect string that will come back to us on the command line	when a friend tries to join our game
    char rgchConnectString[128];
    rgchConnectString[0] = 0;

    if (_eConnectedStatus == k_EClientConnectedAndAuthenticated && _unServerIP && _usServerPort)
    {
        // game server connection method
        StringUtil::sprintf_safe(rgchConnectString, "+connect %d:%d", _unServerIP, _usServerPort);
    }

    SteamFriends()->SetRichPresence("connect", rgchConnectString);
}

void NGSteamClientHelper::updateState()
{
    _state = _eConnectedStatus == k_EClientConnectedAndAuthenticated ? CLIENT_READY_TO_SAY_HELLO : (_eConnectedStatus == k_EClientConnectionFailure || _eConnectedStatus == k_EServerShuttingDown || _eConnectedStatus == k_EServerIsNotAuthorized) ? CLIENT_AUTH_FAILED : CLIENT_NOT_READY_TO_SAY_HELLO;
}
