//
//  Messages.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/19/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Messages__
#define __noctisgames__Messages__

#include "NGSteam.h"
#include "MathUtil.h"

#include <map>

#pragma pack(push, 1)

// Network message types
enum EMessage
{
	// Server messages
	k_EMsgServerBegin = 0,
	k_EMsgServerSendInfo = k_EMsgServerBegin+1,
	k_EMsgServerFailAuthentication = k_EMsgServerBegin+2,
	k_EMsgServerPassAuthentication = k_EMsgServerBegin+3,
	k_EMsgServerExiting = k_EMsgServerBegin+4,
	k_EMsgServerPingResponse = k_EMsgServerBegin+5,

	// Client messages
	k_EMsgClientBegin = 500,
	k_EMsgClientInitiateConnection = k_EMsgClientBegin+1, // Msg from client to server when trying to connect
	k_EMsgClientBeginAuthentication = k_EMsgClientBegin+2,
	k_EMsgClientLeavingServer = k_EMsgClientBegin+3,
	k_EMsgClientPing = k_EMsgClientBegin+4,

	// P2P authentication messages
	k_EMsgP2PBegin = 600, 
	k_EMsgP2PSendingTicket = k_EMsgP2PBegin+1,

	// force 32-bit size enum so the wire protocol doesn't get outgrown later
	k_EForceDWORD  = 0x7fffffff, 
};

// Msg from the server to the client which is sent right after communications are established
// and tells the client what SteamID the game server is using as well as whether the server is secure
struct MsgServerSendInfo_t
{
	MsgServerSendInfo_t() : m_dwMessageType( k_EMsgServerSendInfo ) {}
	DWORD GetMessageType() { return m_dwMessageType; }

	void SetSteamIDServer( uint64 SteamID ) { m_ulSteamIDServer = SteamID; }
	uint64 GetSteamIDServer() { return m_ulSteamIDServer; }

	void SetSecure( bool bSecure ) { m_bIsVACSecure = bSecure; }
	bool GetSecure() { return m_bIsVACSecure; }

	void SetServerName( const char *pchName ) { strncpy( m_rgchServerName, pchName, sizeof( m_rgchServerName ) ); }
	const char *GetServerName() { return m_rgchServerName; }

private:
	const DWORD m_dwMessageType;
	uint64 m_ulSteamIDServer;
	bool m_bIsVACSecure;
	char m_rgchServerName[128];
};

// Msg from the server to the client when refusing a connection
struct MsgServerFailAuthentication_t
{
	MsgServerFailAuthentication_t() : m_dwMessageType( k_EMsgServerFailAuthentication ) {}
	DWORD GetMessageType() { return m_dwMessageType; }
private:
	const DWORD m_dwMessageType;
};

// Msg from the server to client when accepting a pending connection
struct MsgServerPassAuthentication_t
{
	MsgServerPassAuthentication_t() : m_dwMessageType( k_EMsgServerPassAuthentication ) {}
	DWORD GetMessageType() { return m_dwMessageType; }

	void SetPlayerPosition ( uint32 pos ) { m_uPlayerPosition = pos; }
	uint32 GetPlayerPosition() { return m_uPlayerPosition; }

private:
	const DWORD m_dwMessageType;
	uint32 m_uPlayerPosition;
};

// Msg from server to clients when it is exiting
struct MsgServerExiting_t
{
	MsgServerExiting_t() : m_dwMessageType( k_EMsgServerExiting ) {}
	DWORD GetMessageType() { return m_dwMessageType; }

private:
	const DWORD m_dwMessageType;
};

// Msg from server to clients when it is exiting
struct MsgServerPingResponse_t
{
	MsgServerPingResponse_t() : m_dwMessageType( k_EMsgServerPingResponse ) {}
	DWORD GetMessageType() { return m_dwMessageType; }

private:
	const DWORD m_dwMessageType;
};

// Msg from client to server when initiating authentication
struct MsgClientBeginAuthentication_t
{
	MsgClientBeginAuthentication_t() : m_dwMessageType( k_EMsgClientBeginAuthentication ) {}
	DWORD GetMessageType() { return m_dwMessageType; }

	void SetToken( const char *pchToken, uint32 unLen )
    {
        m_uTokenLen = unLen;
        memcpy(m_rgchToken, pchToken, MIN( unLen, sizeof(m_rgchToken)));
    }
    
	uint32 GetTokenLen() { return m_uTokenLen; }
	const char *GetTokenPtr() { return m_rgchToken; }

private:
	const DWORD m_dwMessageType;
	
	uint32 m_uTokenLen;
    char m_rgchToken[1024];
};

// Msg from the client telling the server it is about to leave
struct MsgClientLeavingServer_t
{
	MsgClientLeavingServer_t() : m_dwMessageType( k_EMsgClientLeavingServer ) {}
	DWORD GetMessageType() { return m_dwMessageType; }

private:
	const DWORD m_dwMessageType;
};

// server ping
struct MsgClientPing_t
{
	MsgClientPing_t() : m_dwMessageType( k_EMsgClientPing ) {}
	DWORD GetMessageType() { return m_dwMessageType; }

private:
	const DWORD m_dwMessageType;
};

// Msg from client to server when trying to connect
struct MsgP2PSendingTicket_t
{
	MsgP2PSendingTicket_t() : m_dwMessageType( k_EMsgP2PSendingTicket ) {}
	DWORD GetMessageType() { return m_dwMessageType; }

	void SetToken( const char *pchToken, uint32 unLen ) { m_uTokenLen = unLen; memcpy( m_rgchToken, pchToken, MIN( unLen, sizeof( m_rgchToken ) ) ); }
	uint32 GetTokenLen() { return m_uTokenLen; }
	const char *GetTokenPtr() { return m_rgchToken; }

	void SetSteamID( uint64 ulSteamID ) { m_ulSteamID = ulSteamID; }
	uint64 GetSteamID() { return m_ulSteamID; }

private:
	const DWORD m_dwMessageType;
	uint32 m_uTokenLen;
	char m_rgchToken[1024];
	uint64 m_ulSteamID;
};

#pragma pack(pop)

#endif /* defined(__noctisgames__NGSteam__) */
