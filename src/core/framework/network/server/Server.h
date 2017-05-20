//
//  Server.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Server__
#define __noctisgames__Server__

#include "Engine.h"

#include "ClientProxy.h"
#include "RoboCatServer.h"

class Server : public Engine
{
public:
	static bool StaticInit();

	virtual int Run();

	void HandleNewClient(ClientProxyPtr inClientProxy);
	void HandleLostClient(ClientProxyPtr inClientProxy);

	RoboCatPtr GetCatForPlayer(int inPlayerId);
	void SpawnCatForPlayer(int inPlayerId);

private:
    float m_fFrameStateTime;
    
	Server();

	bool InitNetworkManager();
	void SetupWorld();
};

#endif /* defined(__noctisgames__Server__) */
