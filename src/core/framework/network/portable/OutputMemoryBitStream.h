//
//  OutputMemoryBitStream.h
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OutputMemoryBitStream__
#define __noctisgames__OutputMemoryBitStream__

#include <framework/network/portable/Network.h>

#include <cstdint>
#include <cstdlib>
#include <string>

class OutputMemoryBitStream
{
public:
    OutputMemoryBitStream();
    ~OutputMemoryBitStream();
    
    const char* getBufferPtr() const;
    uint32_t getBitLength() const;
    uint32_t getByteLength() const;
    
    void writeBits(uint8_t inData, uint32_t inBitCount);
    void writeBits(const void* inData, uint32_t inBitCount);
    void writeBytes(const void* inData, uint32_t inByteCount);
    void write(bool inData);
    void writeLarge(const std::string& inString);
    void writeSmall(const std::string& inString);
    void writeSignedBinaryValue(float inValue);
    
    template <typename T, uint32_t BIT_COUNT = sizeof(T) * 8>
    void write(T inData)
    {
        static_assert(std::is_arithmetic< T >::value, "Generic Write only supports primitive data types");
        
        static_assert(BIT_COUNT == 64 || BIT_COUNT == 32 || BIT_COUNT == 16 || BIT_COUNT <= 8, "BIT_COUNT must be 64, 32, 16, or less than or equal to 8");
        
        T data = inData;
        
        if (BIT_COUNT == 64)
        {
            data = htonll(data);
        }
        else if (BIT_COUNT == 32)
        {
            if (std::is_floating_point<T>::value)
            {
                data = float_swap(data, true);
            }
            else
            {
                data = htonl(data);
            }
        }
        else if (BIT_COUNT == 16)
        {
            data = htons(data);
        }
        
        writeBits(&data, BIT_COUNT);
    }
    
private:
    void reallocBuffer(uint32_t inNewBitCapacity);
    
    char* _buffer;
    uint32_t _bitHead;
    uint32_t _bitCapacity;
};

#endif /* defined(__noctisgames__OutputMemoryBitStream__) */
