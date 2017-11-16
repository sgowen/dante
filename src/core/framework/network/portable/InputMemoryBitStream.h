//
//  InputMemoryBitStream.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__InputMemoryBitStream__
#define __noctisgames__InputMemoryBitStream__

#include "Network.h"

#include <cstdint>
#include <cstdlib>
#include <string>

class Vector2;
struct b2Vec2;
class Color;

class InputMemoryBitStream
{
public:
    InputMemoryBitStream(char* inBuffer, uint32_t inBitCount);
    
    InputMemoryBitStream(const InputMemoryBitStream& inOther);
    
    ~InputMemoryBitStream();
    
    const char*	getBufferPtr() const;
    
    uint32_t getRemainingBitCount() const;
    
    void readBits(uint8_t& outData, uint32_t inBitCount);
    
    void readBits(void* outData, uint32_t inBitCount);
    
    void readBytes(void* outData, uint32_t inByteCount);
    
    void readSignedBinaryValue(float& outData);
    
    void read(bool& outData);
    
    void resetToCapacity(uint32_t inByteCapacity);
    
    void read(std::string& inString);
    
    void read(Vector2& outVector);
    
    void read(b2Vec2& outVector);
    
    void read(Color& outColor);
    
    template <typename T, uint32_t BIT_COUNT = sizeof(T) * 8>
    void read(T& outData)
    {
        static_assert(std::is_arithmetic< T >::value ||
                      std::is_enum< T >::value,
                      "Generic Read only supports primitive data types");
        
        static_assert(BIT_COUNT == 64 || BIT_COUNT == 32 || BIT_COUNT == 16 || BIT_COUNT <= 8, "Overriden BIT_COUNT must be 8 or less");
        
        readBits(&outData, BIT_COUNT);
        
        if (BIT_COUNT == 64)
        {
            outData = ntohll(outData);
        }
        else if (BIT_COUNT == 32)
        {
            if (std::is_floating_point<T>::value)
            {
                outData = float_swap(outData, false);
            }
            else
            {
                outData = ntohl(outData);
            }
        }
        else if (BIT_COUNT == 16)
        {
            outData = ntohs(outData);
        }
    }
    
private:
    char* m_buffer;
    uint32_t m_iBitHead;
    uint32_t m_iBitCapacity;
    bool m_isBufferOwner;
};

#endif /* defined(__noctisgames__InputMemoryBitStream__) */
