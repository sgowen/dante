//
//  MemoryBitStreamUtil.h
//  noctisgames
//
//  Created by Stephen Gowen on 4/17/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef ___noctisgames___MemoryBitStreamUtil__
#define ___noctisgames___MemoryBitStreamUtil__

#include <stdint.h>

class InputMemoryBitStream;
class OutputMemoryBitStream;
struct b2Vec2;

class MemoryBitStreamUtil
{
public:
    static void read(InputMemoryBitStream& ip, b2Vec2& v);
    static void write(OutputMemoryBitStream& op, b2Vec2& v);
    
private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    MemoryBitStreamUtil();
    MemoryBitStreamUtil(const MemoryBitStreamUtil&);
    MemoryBitStreamUtil& operator=(const MemoryBitStreamUtil&);
};

#endif /* defined(___noctisgames___MemoryBitStreamUtil__) */
