//
//  OutputMemoryBitStream.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OutputMemoryBitStream__
#define __noctisgames__OutputMemoryBitStream__

#include "Network.h"

#include <cstdint>
#include <cstdlib>
#include <string>

class Vector2;
struct b2Vec2;
class Color;

class OutputMemoryBitStream
{
public:
    OutputMemoryBitStream();
    
    ~OutputMemoryBitStream();
    
    void writeBits(uint8_t inData, uint32_t inBitCount);
    
    void writeBits(const void* inData, uint32_t inBitCount);
    
    const char*	getBufferPtr() const;
    
    uint32_t getBitLength() const;
    
    uint32_t getByteLength() const;
    
    void writeBytes(const void* inData, uint32_t inByteCount);
    
    void write(bool inData);
    
    void write(const Vector2& inVector);
    
    void write(const b2Vec2& inVector);
    
    void write(Color& inColor);
    
    void write(const std::string& inString);
    
    void writeSignedBinaryValue(float inValue);
    
    template <typename T>
    void write(T inData, uint32_t inBitCount = sizeof(T) * 8)
    {
        static_assert(std::is_arithmetic< T >::value ||
                      std::is_enum< T >::value,
                      "Generic Write only supports primitive data types");
        
        T data = inData;
        if (inBitCount == 16)
        {
            data = htons(data);
        }
        else if (inBitCount == 32)
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
        else if (inBitCount == 64)
        {
            data = htonll(data);
        }
        
        writeBits(&data, inBitCount);
    }
    
private:
    void reallocBuffer(uint32_t inNewBitCapacity);
    
    char* m_buffer;
    uint32_t m_iBitHead;
    uint32_t m_iBitCapacity;
};

#endif /* defined(__noctisgames__OutputMemoryBitStream__) */
