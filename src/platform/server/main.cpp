//
//  main.cpp
//  dante server
//
//  Created by Stephen Gowen on 5/9/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

int main(int argc, const char * argv[])
{
    if( Server::StaticInit() )
    {
        return Server::sInstance->Run();
    }
    
    return -1;
}
