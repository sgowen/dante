//
//  MemoryBitStreamUtil.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 4/17/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/portable/MemoryBitStreamUtil.h>

#include <framework/network/portable/InputMemoryBitStream.h>
#include <framework/network/portable/OutputMemoryBitStream.h>
#include <Box2D/Common/b2Math.h>

void MemoryBitStreamUtil::read(InputMemoryBitStream& ip, b2Vec2& v)
{
#ifdef NG_LOG
    // Disable optimizations
    bool isZero;
    ip.read(isZero);
    ip.read(v.x);
    ip.read(isZero);
    ip.read(v.y);
#else
    bool isZero;
    ip.read(isZero);
    if (isZero)
    {
        v.x = 0;
    }
    else
    {
        ip.read(v.x);
    }
    
    ip.read(isZero);
    if (isZero)
    {
        v.y = 0;
    }
    else
    {
        ip.read(v.y);
    }
#endif
}

void MemoryBitStreamUtil::write(OutputMemoryBitStream& op, b2Vec2& v)
{
#ifdef NG_LOG
    // Disable optimizations
    op.write(false);
    op.write(v.x);
    op.write(false);
    op.write(v.y);
#else
    bool isZero = v.x == 0;
    op.write(isZero);
    if (!isZero)
    {
        op.write(v.x);
    }
    
    isZero = v.y == 0;
    op.write(isZero);
    if (!isZero)
    {
        op.write(v.y);
    }
#endif
}
