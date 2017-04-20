//
//  client.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/5/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__client__
#define __noctisgames__client__

#define _CRT_SECURE_NO_WARNINGS

#define MAX_DATA_SIZE 512 // max number of bytes we can get at once

void updateCoords(float x, float y);

char * getLatestMessage();

int getClientStatus();

int startClientUDP();

int sendCoords();

int readFromServer();

void shutDownClient();

#endif /* defined(__noctisgames__client__) */
