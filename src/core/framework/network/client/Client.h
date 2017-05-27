//
//  Client.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Client__
#define __noctisgames__Client__

#include "Engine.h"

class Client : public Engine
{
public:
    static Client* getInstance();
    
    void init(std::string& serverIPAddress, std::string& userID);
    
private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    Client();
    Client(const Client&);
    Client& operator=(const Client&);
};

#endif /* defined(__noctisgames__Client__) */
