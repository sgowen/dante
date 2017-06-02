//
//  InputMemoryBitStream.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__InputMemoryBitStream__
#define __noctisgames__InputMemoryBitStream__

#include <cstdint>
#include <cstdlib>
#include <string>

class Vector2;
struct Color;

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
    
    void read(uint32_t& outData, uint32_t inBitCount = 32);
    
    void read(int& outData, uint32_t inBitCount = 32);
    
    void read(float& outData);
    
    void read(uint16_t& outData, uint32_t inBitCount = 16);
    
    void read(int16_t& outData, uint32_t inBitCount = 16);
    
    void read(uint8_t& outData, uint32_t inBitCount = 8);
    
    void read(bool& outData);
    
    void resetToCapacity(uint32_t inByteCapacity);
    
    void read(std::string& inString);
    
    void read(Vector2& outVector);
    
    void read(Color& outColor);
    
    template <typename T>
    void read(T& inData, uint32_t inBitCount = sizeof(T) * 8)
    {
        static_assert(std::is_arithmetic< T >::value ||
                      std::is_enum< T >::value,
                      "Generic Read only supports primitive data types");
        readBits(&inData, inBitCount);
    }
    
private:
    char* m_buffer;
    uint32_t m_iBitHead;
    uint32_t m_iBitCapacity;
    bool m_isBufferOwner;
};

#endif /* defined(__noctisgames__InputMemoryBitStream__) */
