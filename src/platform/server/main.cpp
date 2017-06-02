//
//  main.cpp
//  dante server
//
//  Created by Stephen Gowen on 5/9/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Server.h"

int main(int argc, const char * argv[])
{
    Server* server = Server::getInstance();
    
    if (server->isInitialized())
    {
        return server->run();
    }
    
    return -1;
}
