//
//  NGSteamP2PAuth.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/20/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NGSteamP2PAuth.h"

#include "InputMemoryBitStream.h"
#include "INetworkHelper.h"
#include "NGSteamAddress.h"

#include "Timing.h"
#include "StringUtil.h"
#include "OutputMemoryBitStream.h"

NGSteamP2PAuth::NGSteamP2PAuth(INetworkHelper* networkHelper) :
m_networkTransport(new NGSteamP2PNetworkTransport(networkHelper))
{
    // no players yet
	for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; i++)
	{
		m_rgpP2PAuthPlayer[i] = nullptr;
	}
    
	// no queued messages
	for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; i++)
	{
		m_rgpQueuedMessage[i] = nullptr;
	}
}

NGSteamP2PAuth::~NGSteamP2PAuth()
{
    delete m_networkTransport;
}

void NGSteamP2PAuth::playerDisconnect(int iSlot)
{
	if (m_rgpP2PAuthPlayer[iSlot])
	{
		m_rgpP2PAuthPlayer[iSlot]->endGame();
		delete m_rgpP2PAuthPlayer[iSlot];
		m_rgpP2PAuthPlayer[iSlot] = nullptr;
	}
}

void NGSteamP2PAuth::endGame()
{
	for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
	{
		if (m_rgpP2PAuthPlayer[i])
		{
			m_rgpP2PAuthPlayer[i]->endGame();
            
			delete m_rgpP2PAuthPlayer[i];
			m_rgpP2PAuthPlayer[i] = nullptr;
		}
	}
}

void NGSteamP2PAuth::internalinitPlayer(int iSlot, CSteamID steamID, bool bStartAuthProcess)
{
	char rgch[128];
	sprintf(rgch, "P2P:: startAuthPlayer slot=%d account=%d \n", iSlot, steamID.GetAccountID());
	LOG(rgch);
	m_rgpP2PAuthPlayer[iSlot] = new NGSteamP2PAuthPlayer(m_networkTransport);
	m_rgpP2PAuthPlayer[iSlot]->initPlayer(steamID);
	if (bStartAuthProcess)
    {
        m_rgpP2PAuthPlayer[iSlot]->startAuthPlayer();
    }

	// check our queued messages list to see if this guys ticket message arrived before we noticed him
	for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; i++)
	{
		if (m_rgpQueuedMessage[i] != NULL)
		{
			if (m_rgpP2PAuthPlayer[i]->handleMessage(m_rgpQueuedMessage[i]))
			{
				LOG("P2P:: Consumed queued message");
                delete m_rgpQueuedMessage[i];
				m_rgpQueuedMessage[i] = nullptr;
			}
		}
	}
}

void NGSteamP2PAuth::registerPlayer(int iSlot, CSteamID steamID)
{
	if (iSlot < MAX_NUM_PLAYERS_PER_SERVER)
    {
        internalinitPlayer(iSlot, steamID, false);
    }
}

void NGSteamP2PAuth::startAuthPlayer(int iSlot, CSteamID steamID)
{
	if (iSlot < MAX_NUM_PLAYERS_PER_SERVER)
    {
        internalinitPlayer(iSlot, steamID, true);
    }
}

bool NGSteamP2PAuth::handleMessage(uint32_t packetType, InputMemoryBitStream& inInputStream)
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
            
            for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; i++)
            {
                if (m_rgpP2PAuthPlayer[i])
                {
                    if (m_rgpP2PAuthPlayer[i]->handleMessage(msg))
                    {
                        return true; // message handled
                    }
                }
            }
            
            // if we dont have the player in our list yet, lets queue the message and assume he will show up soon
            LOG("P2P:: handleMessage queueing message");
            for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; i++)
            {
                if (m_rgpQueuedMessage[i] == NULL)
                {
                    m_rgpQueuedMessage[i] = msg;
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

NGSteamP2PAuthPlayer::NGSteamP2PAuthPlayer(NGSteamP2PNetworkTransport *pNetworkTransport)
: m_CallbackBeginAuthResponse(this, &NGSteamP2PAuthPlayer::OnBeginAuthResponse)
{
    m_networkTransport = pNetworkTransport;
    m_bSentTicket = false;
    m_bSubmittedHisTicket = false;
    m_bHaveAnswer = false;
    m_fConnectTime = Timing::getInstance()->getFrameStartTime();
    m_cubTicketIGaveThisUser = 0;
    m_cubTicketHeGaveMe = 0;
}

NGSteamP2PAuthPlayer::~NGSteamP2PAuthPlayer()
{
    endGame();
    
    m_networkTransport->closeConnection(SteamUser()->GetSteamID());
    m_networkTransport = nullptr;
}

void NGSteamP2PAuthPlayer::OnBeginAuthResponse(ValidateAuthTicketResponse_t *pCallback)
{
    if (m_steamID == pCallback->m_SteamID)
    {
        char rgch[128];
        sprintf(rgch, "P2P:: Received steam response for account=%d\n", m_steamID.GetAccountID());
        LOG(rgch);
        m_fAnswerTime = Timing::getInstance()->getFrameStartTime();
        m_bHaveAnswer = true;
        m_eAuthSessionResponse = pCallback->m_eAuthSessionResponse;
    }
}

void NGSteamP2PAuthPlayer::initPlayer(CSteamID steamID)
{
    m_steamID = steamID;
    m_bSentTicket = false;
    m_bSubmittedHisTicket = false;
    m_bHaveAnswer = false;
    m_fConnectTime = Timing::getInstance()->getFrameStartTime();
    m_cubTicketIGaveThisUser = 0;
    m_cubTicketHeGaveMe = 0;
}

void NGSteamP2PAuthPlayer::startAuthPlayer()
{
    // send him the ticket if we havent yet
    if (m_cubTicketIGaveThisUser == 0)
    {
        m_hAuthTicketIGaveThisUser = SteamUser()->GetAuthSessionTicket(m_rgubTicketIGaveThisUser, sizeof(m_rgubTicketIGaveThisUser), &m_cubTicketIGaveThisUser);
    }
    
    // sample has no retry here
    m_networkTransport->sendTicket(SteamUser()->GetSteamID(), m_steamID, m_cubTicketIGaveThisUser, m_rgubTicketIGaveThisUser);
    
    m_bSentTicket = true;
    
    // start a timer on this, if we dont get a ticket back within reasonable time, mark him timed out
    m_fTicketTime = Timing::getInstance()->getFrameStartTime();
}

bool NGSteamP2PAuthPlayer::isAuthOk()
{
    if (m_steamID.IsValid())
    {
        // Timeout if we fail to establish communication with this player
        if (!m_bSentTicket && !m_bSubmittedHisTicket)
        {
            if (Timing::getInstance()->getFrameStartTime() - m_fConnectTime > 30)
            {
                char rgch[128];
                sprintf(rgch, "P2P:: Nothing received for account=%d\n", m_steamID.GetAccountID());
                LOG(rgch);
                return false;
            }
        }
        
        // first ticket check: if i submitted his ticket - was it good?
        if (m_bSubmittedHisTicket && m_eBeginAuthSessionResult != k_EBeginAuthSessionResultOK)
        {
            char rgch[128];
            sprintf(rgch, "P2P:: Ticket from account=%d was bad\n", m_steamID.GetAccountID());
            LOG(rgch);
            return false;
        }
        
        // second ticket check: if the steam backend replied, was that good?
        if (m_bHaveAnswer && m_eAuthSessionResponse != k_EAuthSessionResponseOK)
        {
            char rgch[128];
            sprintf(rgch, "P2P:: Steam response for account=%d was bad\n", m_steamID.GetAccountID());
            LOG(rgch);
            return false;
        }
        
        // last: if i sent him a ticket and he has not reciprocated, time out after 30 sec
        if (m_bSentTicket && !m_bSubmittedHisTicket)
        {
            if (Timing::getInstance()->getFrameStartTime() - m_fTicketTime > 30)
            {
                char rgch[128];
                sprintf(rgch, "P2P:: No ticket received for account=%d\n", m_steamID.GetAccountID());
                LOG(rgch);
                return false;
            }
        }
    }
    return true;
}

void NGSteamP2PAuthPlayer::endGame()
{
    if (m_bSentTicket)
    {
        SteamUser()->CancelAuthTicket(m_hAuthTicketIGaveThisUser);
        m_bSentTicket = false;
    }
    
    if (m_bSubmittedHisTicket)
    {
        SteamUser()->EndAuthSession(m_steamID);
        m_bSubmittedHisTicket = false;
    }
    
    if (m_steamID.IsValid())
    {
        m_networkTransport->closeConnection(m_steamID);
    }
}

bool NGSteamP2PAuthPlayer::handleMessage(MsgP2PSendingTicket* msg)
{
    // message from another player providing his ticket
    // is this message for me?
    if (msg->getSteamID() != m_steamID.ConvertToUint64())
    {
        return false;
    }
    
    m_cubTicketHeGaveMe = msg->getTokenLen();
    memcpy(m_rgubTicketHeGaveMe, msg->getTokenPtr(), m_cubTicketHeGaveMe);
    m_eBeginAuthSessionResult = SteamUser()->BeginAuthSession(m_rgubTicketHeGaveMe, m_cubTicketHeGaveMe, m_steamID);
    m_bSubmittedHisTicket = true;
    char rgch[128];
    sprintf(rgch, "P2P:: ReceivedTicket from account=%d \n", m_steamID.GetAccountID());
    LOG(rgch);
    if (!m_bSentTicket)
    {
        startAuthPlayer();
    }
    return true;
}

NGSteamP2PNetworkTransport::NGSteamP2PNetworkTransport(INetworkHelper* networkHelper) :
m_networkHelper(networkHelper),
m_outgoingPacketAddress(new NGSteamAddress()),
m_CallbackP2PSessionRequest(this, &NGSteamP2PNetworkTransport::onP2PSessionRequest),
m_CallbackP2PSessionConnectFail(this, &NGSteamP2PNetworkTransport::onP2PSessionConnectFail)
{
    // Empty
}

NGSteamP2PNetworkTransport::~NGSteamP2PNetworkTransport()
{
    delete m_outgoingPacketAddress;
}

void NGSteamP2PNetworkTransport::sendTicket(CSteamID steamIDFrom, CSteamID steamIDTo, uint32 cubTicket, uint8 *pubTicket)
{
    MsgP2PSendingTicket msg;
    msg.setToken((char *)pubTicket, cubTicket);
    msg.setSteamID(steamIDFrom.ConvertToUint64());
    
    OutputMemoryBitStream packet;
    packet.write(msg.getMessageType());
    packet.write(msg.getSteamID());
    packet.write(msg.getTokenLen());
    packet.writeBytes(msg.getTokenPtr(), msg.getTokenLen());
    
    m_outgoingPacketAddress->setSteamID(steamIDTo);
    m_outgoingPacketAddress->setReliable(true);
    
    m_networkHelper->sendPacket(packet, m_outgoingPacketAddress);
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

MsgP2PSendingTicket::MsgP2PSendingTicket() : m_dwMessageType(k_EMsgP2PSendingTicket)
{
    // Empty
}

DWORD MsgP2PSendingTicket::getMessageType()
{
    return m_dwMessageType;
}

void MsgP2PSendingTicket::setToken(const char *pchToken, uint32 unLen)
{
    m_uTokenLen = unLen;
    
    memcpy(m_rgchToken, pchToken, MIN(unLen, sizeof(m_rgchToken)));
}

uint32 MsgP2PSendingTicket::getTokenLen()
{
    return m_uTokenLen;
}

const char* MsgP2PSendingTicket::getTokenPtr()
{
    return m_rgchToken;
}

void MsgP2PSendingTicket::setSteamID(uint64 ulSteamID)
{
    m_ulSteamID = ulSteamID;
}

uint64 MsgP2PSendingTicket::getSteamID()
{
    return m_ulSteamID;
}
