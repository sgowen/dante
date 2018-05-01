//
//  NGSteamP2PAuth.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 6/20/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/steam/NGSteamP2PAuth.h>

#include <framework/network/portable/InputMemoryBitStream.h>
#include <framework/network/portable/NetworkHelper.h>
#include <framework/network/steam/NGSteamAddress.h>
#include <framework/util/Timing.h>

#include <framework/util/StringUtil.h>
#include <framework/network/portable/OutputMemoryBitStream.h>
#include <framework/util/InstanceManager.h>

NGSteamP2PAuth::NGSteamP2PAuth(NetworkHelper* networkHelper) :
_networkTransport(new NGSteamP2PNetworkTransport(networkHelper))
{
    // no players yet
	for (uint8 i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
	{
		_rgpP2PAuthPlayer[i] = NULL;
	}

	// no queued messages
	for (uint8 i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
	{
		_rgpQueuedMessage[i] = NULL;
	}
}

NGSteamP2PAuth::~NGSteamP2PAuth()
{
    delete _networkTransport;
}

void NGSteamP2PAuth::playerDisconnect(uint8 iSlot)
{
	if (_rgpP2PAuthPlayer[iSlot])
	{
		_rgpP2PAuthPlayer[iSlot]->endGame();
		delete _rgpP2PAuthPlayer[iSlot];
		_rgpP2PAuthPlayer[iSlot] = NULL;
	}
}

void NGSteamP2PAuth::endGame()
{
	for (uint8 i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
	{
		if (_rgpP2PAuthPlayer[i])
		{
			_rgpP2PAuthPlayer[i]->endGame();

			delete _rgpP2PAuthPlayer[i];
			_rgpP2PAuthPlayer[i] = NULL;
		}
	}
}

void NGSteamP2PAuth::internalinitPlayer(uint8 iSlot, CSteamID steamID, bool bStartAuthProcess)
{
	char rgch[128];
	sprintf(rgch, "P2P:: startAuthPlayer slot=%d account=%d \n", iSlot, steamID.GetAccountID());
	LOG(rgch);
	_rgpP2PAuthPlayer[iSlot] = new NGSteamP2PAuthPlayer(_networkTransport);
	_rgpP2PAuthPlayer[iSlot]->initPlayer(steamID);
	if (bStartAuthProcess)
    {
        _rgpP2PAuthPlayer[iSlot]->startAuthPlayer();
    }

	// check our queued messages list to see if this guys ticket message arrived before we noticed him
	for (uint8 i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
	{
		if (_rgpQueuedMessage[i] != NULL)
		{
			if (_rgpP2PAuthPlayer[i]->handleMessage(_rgpQueuedMessage[i]))
			{
				LOG("P2P:: Consumed queued message");
                delete _rgpQueuedMessage[i];
				_rgpQueuedMessage[i] = NULL;
			}
		}
	}
}

void NGSteamP2PAuth::registerPlayer(uint8 iSlot, CSteamID steamID)
{
	if (iSlot < MAX_NUM_PLAYERS_PER_SERVER)
    {
        internalinitPlayer(iSlot, steamID, false);
    }
}

void NGSteamP2PAuth::startAuthPlayer(uint8 iSlot, CSteamID steamID)
{
	if (iSlot < MAX_NUM_PLAYERS_PER_SERVER)
    {
        internalinitPlayer(iSlot, steamID, true);
    }
}

bool NGSteamP2PAuth::handleMessage(uint8 packetType, InputMemoryBitStream& inInputStream)
{
	switch (packetType)
	{
		// message from another player providing his ticket
        case k_EMsgP2PSendingTicket:
        {
            uint64 steamID;
            inInputStream.read(steamID);

            uint32 uTokenLen;
            inInputStream.read(uTokenLen);

            char token[1024];
            inInputStream.readBytes(&token, uTokenLen);

            MsgP2PSendingTicket *msg = new MsgP2PSendingTicket();
            msg->setSteamID(steamID);
            msg->setToken(token, uTokenLen);

            for (uint8 i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
            {
                if (_rgpP2PAuthPlayer[i])
                {
                    if (_rgpP2PAuthPlayer[i]->handleMessage(msg))
                    {
                        return true; // message handled
                    }
                }
            }

            // if we dont have the player in our list yet, lets queue the message and assume he will show up soon
            LOG("P2P:: handleMessage queueing message");
            for (uint8 i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
            {
                if (_rgpQueuedMessage[i] == NULL)
                {
                    _rgpQueuedMessage[i] = msg;
                    return true; // message handled
                }
            }

            delete msg;

            break;
        }
        default:
            // this is not a P2P auth message
            break;
	}

	// message didn't get handled here
	return false;
}

NGSteamP2PAuthPlayer::NGSteamP2PAuthPlayer(NGSteamP2PNetworkTransport *pNetworkTransport) :
_callbackBeginAuthResponse(this, &NGSteamP2PAuthPlayer::OnBeginAuthResponse),
_timing(static_cast<Timing*>(INSTANCE_MANAGER->get(INSTANCE_TIME_CLIENT)))
{
    _networkTransport = pNetworkTransport;
    _bSentTicket = false;
    _bSubmittedHisTicket = false;
    _bHaveAnswer = false;
    _connectTime = _timing->getTime();
    _cubTicketIGaveThisUser = 0;
    _cubTicketHeGaveMe = 0;
}

NGSteamP2PAuthPlayer::~NGSteamP2PAuthPlayer()
{
    endGame();

    _networkTransport->closeConnection(SteamUser()->GetSteamID());
    _networkTransport = NULL;
}

void NGSteamP2PAuthPlayer::OnBeginAuthResponse(ValidateAuthTicketResponse_t *pCallback)
{
    if (_steamID == pCallback->m_SteamID)
    {
        char rgch[128];
        sprintf(rgch, "P2P:: Received steam response for account=%d\n", _steamID.GetAccountID());
        LOG(rgch);
        _answerTime = _timing->getTime();
        _bHaveAnswer = true;
        _eAuthSessionResponse = pCallback->m_eAuthSessionResponse;
    }
}

void NGSteamP2PAuthPlayer::initPlayer(CSteamID steamID)
{
    _steamID = steamID;
    _bSentTicket = false;
    _bSubmittedHisTicket = false;
    _bHaveAnswer = false;
    _connectTime = _timing->getTime();
    _cubTicketIGaveThisUser = 0;
    _cubTicketHeGaveMe = 0;
}

void NGSteamP2PAuthPlayer::startAuthPlayer()
{
    // send him the ticket if we havent yet
    if (_cubTicketIGaveThisUser == 0)
    {
        _hAuthTicketIGaveThisUser = SteamUser()->GetAuthSessionTicket(_rgubTicketIGaveThisUser, sizeof(_rgubTicketIGaveThisUser), &_cubTicketIGaveThisUser);
    }

    // sample has no retry here
    _networkTransport->sendTicket(SteamUser()->GetSteamID(), _steamID, _cubTicketIGaveThisUser, _rgubTicketIGaveThisUser);

    _bSentTicket = true;

    // start a timer on this, if we dont get a ticket back within reasonable time, mark him timed out
    _ticketTime = _timing->getTime();
}

bool NGSteamP2PAuthPlayer::isAuthOk()
{
    if (_steamID.IsValid())
    {
        // Timeout if we fail to establish communication with this player
        if (!_bSentTicket && !_bSubmittedHisTicket)
        {
            if (_timing->getTime() - _connectTime > 30)
            {
                char rgch[128];
                sprintf(rgch, "P2P:: Nothing received for account=%d\n", _steamID.GetAccountID());
                LOG(rgch);
                return false;
            }
        }

        // first ticket check: if i submitted his ticket - was it good?
        if (_bSubmittedHisTicket && _eBeginAuthSessionResult != k_EBeginAuthSessionResultOK)
        {
            char rgch[128];
            sprintf(rgch, "P2P:: Ticket from account=%d was bad\n", _steamID.GetAccountID());
            LOG(rgch);
            return false;
        }

        // second ticket check: if the steam backend replied, was that good?
        if (_bHaveAnswer && _eAuthSessionResponse != k_EAuthSessionResponseOK)
        {
            char rgch[128];
            sprintf(rgch, "P2P:: Steam response for account=%d was bad\n", _steamID.GetAccountID());
            LOG(rgch);
            return false;
        }

        // last: if i sent him a ticket and he has not reciprocated, time out after 30 sec
        if (_bSentTicket && !_bSubmittedHisTicket)
        {
            if (_timing->getTime() - _ticketTime > 30)
            {
                char rgch[128];
                sprintf(rgch, "P2P:: No ticket received for account=%d\n", _steamID.GetAccountID());
                LOG(rgch);
                return false;
            }
        }
    }
    return true;
}

void NGSteamP2PAuthPlayer::endGame()
{
    if (_bSentTicket)
    {
        SteamUser()->CancelAuthTicket(_hAuthTicketIGaveThisUser);
        _bSentTicket = false;
    }

    if (_bSubmittedHisTicket)
    {
        SteamUser()->EndAuthSession(_steamID);
        _bSubmittedHisTicket = false;
    }

    if (_steamID.IsValid())
    {
        _networkTransport->closeConnection(_steamID);
    }
}

bool NGSteamP2PAuthPlayer::handleMessage(MsgP2PSendingTicket* msg)
{
    // message from another player providing his ticket
    // is this message for me?
    if (msg->getSteamID() != _steamID.ConvertToUint64())
    {
        return false;
    }

    _cubTicketHeGaveMe = msg->getTokenLen();
    memcpy(_rgubTicketHeGaveMe, msg->getTokenPtr(), _cubTicketHeGaveMe);
    _eBeginAuthSessionResult = SteamUser()->BeginAuthSession(_rgubTicketHeGaveMe, _cubTicketHeGaveMe, _steamID);
    _bSubmittedHisTicket = true;
    char rgch[128];
    sprintf(rgch, "P2P:: ReceivedTicket from account=%d \n", _steamID.GetAccountID());
    LOG(rgch);
    if (!_bSentTicket)
    {
        startAuthPlayer();
    }
    return true;
}

NGSteamP2PNetworkTransport::NGSteamP2PNetworkTransport(NetworkHelper* networkHelper) :
_networkHelper(networkHelper),
_outgoingPacketAddress(new NGSteamAddress()),
_CallbackP2PSessionRequest(this, &NGSteamP2PNetworkTransport::onP2PSessionRequest),
_CallbackP2PSessionConnectFail(this, &NGSteamP2PNetworkTransport::onP2PSessionConnectFail)
{
    // Empty
}

NGSteamP2PNetworkTransport::~NGSteamP2PNetworkTransport()
{
    delete _outgoingPacketAddress;
}

void NGSteamP2PNetworkTransport::sendTicket(CSteamID steamIDFrom, CSteamID steamIDTo, uint32 cubTicket, uint8 *pubTicket)
{
    MsgP2PSendingTicket msg;
    msg.setToken((char *)pubTicket, cubTicket);
    msg.setSteamID(steamIDFrom.ConvertToUint64());

    OutputMemoryBitStream packet;
    packet.write(static_cast<uint8_t>(msg.getMessageType()));
    packet.write(msg.getSteamID());
    packet.write(msg.getTokenLen());
    packet.writeBytes(msg.getTokenPtr(), msg.getTokenLen());

    _outgoingPacketAddress->setSteamID(steamIDTo);
    _outgoingPacketAddress->setReliable(true);

    _networkHelper->sendPacket(packet, _outgoingPacketAddress);
}

void NGSteamP2PNetworkTransport::closeConnection(CSteamID steamID)
{
    SteamNetworking()->CloseP2PSessionWithUser(steamID);
}

void NGSteamP2PNetworkTransport::onP2PSessionRequest(P2PSessionRequest_t *pP2PSessionRequest)
{
    // always accept packets
    // the packet itself will come through when you call SteamNetworking()->ReadP2PPacket()
    SteamNetworking()->AcceptP2PSessionWithUser(pP2PSessionRequest->m_steamIDRemote);
}

void NGSteamP2PNetworkTransport::onP2PSessionConnectFail(P2PSessionConnectFail_t *pP2PSessionConnectFail)
{
    // we've sent a packet to the user, but it never got through
    // we can just use the normal timeout
}

MsgP2PSendingTicket::MsgP2PSendingTicket() : _messageType(k_EMsgP2PSendingTicket)
{
    // Empty
}

uint8 MsgP2PSendingTicket::getMessageType()
{
    return _messageType;
}

void MsgP2PSendingTicket::setToken(const char *pchToken, uint32 unLen)
{
    _uTokenLen = unLen;

    memcpy(_rgchToken, pchToken, MIN(unLen, sizeof(_rgchToken)));
}

uint32 MsgP2PSendingTicket::getTokenLen()
{
    return _uTokenLen;
}

const char* MsgP2PSendingTicket::getTokenPtr()
{
    return _rgchToken;
}

void MsgP2PSendingTicket::setSteamID(uint64 ulSteamID)
{
    _ulSteamID = ulSteamID;
}

uint64 MsgP2PSendingTicket::getSteamID()
{
    return _ulSteamID;
}
