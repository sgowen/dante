//
//  AckRange.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__AckRange__
#define __noctisgames__AckRange__

#include <stdint.h>

class OutputMemoryBitStream;
class InputMemoryBitStream;

class AckRange
{
public:
    AckRange();
    
    AckRange(uint16_t inStart);
    
    void write(OutputMemoryBitStream& inOutputStream) const;
    
    void read(InputMemoryBitStream& inInputStream);
    
    //if this is the next in sequence, just extend the range
    bool extendIfShould(uint16_t inSequenceNumber);
    
    uint16_t getStart() const;
    
    uint32_t getCount() const;
    
private:
    uint16_t _start;
    uint32_t _count;
};

#endif /* defined(__noctisgames__AckRange__) */
