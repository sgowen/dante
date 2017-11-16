//
//  NGSteamP2PAuth.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/20/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSteamP2PAuth__
#define __noctisgames__NGSteamP2PAuth__

#include "NGSteam.h"
#include "FrameworkConstants.h"
#include "MathUtil.h"

class InputMemoryBitStream;
class INetworkHelper;
class NGSteamAddress;

class NGSteamP2PAuthPlayer;
class NGSteamP2PNetworkTransport;
class MsgP2PSendingTicket;

class NGSteamP2PAuth
{
public:
    NGSteamP2PAuthPlayer *m_rgpP2PAuthPlayer[MAX_NUM_PLAYERS_PER_SERVER];
    MsgP2PSendingTicket *m_rgpQueuedMessage[MAX_NUM_PLAYERS_PER_SERVER];
    NGSteamP2PNetworkTransport *m_networkTransport;
    
    NGSteamP2PAuth(INetworkHelper* networkHelper);
    
    ~NGSteamP2PAuth();
    
    void playerDisconnect(uint8_t iSlot);
    void endGame();
    void startAuthPlayer(uint8_t iSlot, CSteamID steamID);
    void registerPlayer(uint8_t iSlot, CSteamID steamID);
    bool handleMessage(uint32_t packetType, InputMemoryBitStream& inInputStream);
    void internalinitPlayer(uint8_t iSlot, CSteamID steamID, bool bStartAuthProcess);
};

class NGSteamP2PAuthPlayer
{
public:
    CSteamID m_steamID;
    
	NGSteamP2PAuthPlayer(NGSteamP2PNetworkTransport *pNetworkTransport);
    
	~NGSteamP2PAuthPlayer();
	
    void endGame();
	void initPlayer(CSteamID steamID);
	void startAuthPlayer();
	bool isAuthOk();
	bool handleMessage(MsgP2PSendingTicket* msg);

	STEAM_CALLBACK(NGSteamP2PAuthPlayer, OnBeginAuthResponse, ValidateAuthTicketResponse_t, m_CallbackBeginAuthResponse);
    
private:
	bool m_bSentTicket;
	bool m_bSubmittedHisTicket;
	bool m_bHaveAnswer;
	float m_fConnectTime;
	float m_fTicketTime;
	float m_fAnswerTime;
	uint32 m_cubTicketIGaveThisUser;
	uint8 m_rgubTicketIGaveThisUser[1024];
	uint32 m_cubTicketHeGaveMe;
	uint8 m_rgubTicketHeGaveMe[1024];
	HAuthTicket m_hAuthTicketIGaveThisUser;
	EBeginAuthSessionResult m_eBeginAuthSessionResult;
	EAuthSessionResponse m_eAuthSessionResponse;

	NGSteamP2PNetworkTransport *m_networkTransport;
};

class NGSteamP2PNetworkTransport
{
public:
	NGSteamP2PNetworkTransport(INetworkHelper* networkHelper);
    
    ~NGSteamP2PNetworkTransport();
    
	void sendTicket(CSteamID steamIDFrom, CSteamID steamIDTo, uint32 cubTicket, uint8 *pubTicket);
	void closeConnection(CSteamID steamID);

	STEAM_CALLBACK(NGSteamP2PNetworkTransport, onP2PSessionRequest, P2PSessionRequest_t, m_CallbackP2PSessionRequest);
	STEAM_CALLBACK(NGSteamP2PNetworkTransport, onP2PSessionConnectFail, P2PSessionConnectFail_t, m_CallbackP2PSessionConnectFail);
    
private:
    INetworkHelper* m_networkHelper;
    NGSteamAddress* m_outgoingPacketAddress;
};

class MsgP2PSendingTicket
{
public:
    MsgP2PSendingTicket();
    
    DWORD getMessageType();
    
    void setToken(const char *pchToken, uint32 unLen);
    uint32 getTokenLen();
    const char* getTokenPtr();
    
    void setSteamID(uint64 ulSteamID);
    uint64 getSteamID();
    
private:
    const DWORD m_dwMessageType;
    uint32 m_uTokenLen;
    char m_rgchToken[1024];
    uint64 m_ulSteamID;
};

#endif /* defined(__noctisgames__NGSteamP2PAuth__) */
