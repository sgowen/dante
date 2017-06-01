//
//  NetworkType.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NetworkType__
#define __noctisgames__NetworkType__

#include <stdint.h>

//in case we decide to change the type of the sequence number to use fewer or more bits
typedef uint16_t PacketSequenceNumber;

#define NETWORK_TYPE_DECL(inCode) \
public: \
enum { kClassId = inCode }; \
virtual uint32_t getNetworkType();

#define NETWORK_TYPE_IMPL(name) \
uint32_t name::getNetworkType() { return kClassId; }

#endif /* defined(__noctisgames__NetworkType__) */
