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
    InputMemoryBitStream(char* inBuffer, uint32_t inBitCount) :
    mBuffer(inBuffer),
    mBitCapacity(inBitCount),
    mBitHead(0),
    mIsBufferOwner(false) {}
    
    InputMemoryBitStream(const InputMemoryBitStream& inOther) :
    mBitCapacity(inOther.mBitCapacity),
    mBitHead(inOther.mBitHead),
    mIsBufferOwner(true)
    {
        //allocate buffer of right size
        int byteCount = mBitCapacity / 8;
        mBuffer = static_cast<char*>(malloc(byteCount));
        //copy
        memcpy(mBuffer, inOther.mBuffer, byteCount);
    }
    
    ~InputMemoryBitStream()	{ if (mIsBufferOwner) { free(mBuffer); }; }
    
    const char*	getBufferPtr() const { return mBuffer; }
    uint32_t GetRemainingBitCount() const { return mBitCapacity - mBitHead; }
    
    void ReadBits(uint8_t& outData, uint32_t inBitCount);
    void ReadBits(void* outData, uint32_t inBitCount);
    
    void ReadBytes(void* outData, uint32_t inByteCount) { ReadBits(outData, inByteCount << 3); }
    
    template <typename T>
    void read(T& inData, uint32_t inBitCount = sizeof(T) * 8)
    {
        static_assert(std::is_arithmetic< T >::value ||
                      std::is_enum< T >::value,
                      "Generic Read only supports primitive data types");
        ReadBits(&inData, inBitCount);
    }
    
    void read(uint32_t& outData, uint32_t inBitCount = 32) { ReadBits(&outData, inBitCount); }
    void read(int& outData, uint32_t inBitCount = 32) { ReadBits(&outData, inBitCount); }
    void read(float& outData) { ReadBits(&outData, 32); }
    
    void read(uint16_t& outData, uint32_t inBitCount = 16) { ReadBits(&outData, inBitCount); }
    void read(int16_t& outData, uint32_t inBitCount = 16) { ReadBits(&outData, inBitCount); }
    
    void read(uint8_t& outData, uint32_t inBitCount = 8) { ReadBits(&outData, inBitCount); }
    void read(bool& outData) { ReadBits(&outData, 1); }
    
    void ResetToCapacity(uint32_t inByteCapacity) { mBitCapacity = inByteCapacity << 3; mBitHead = 0; }
    
    void read(std::string& inString)
    {
        uint32_t elementCount;
        read(elementCount);
        inString.resize(elementCount);
        for (auto& element : inString)
        {
            read(element);
        }
    }
    
    void read(Vector2& outVector);
    
    void read(Color& outColor);
    
private:
    char* mBuffer;
    uint32_t mBitHead;
    uint32_t mBitCapacity;
    bool mIsBufferOwner;
};

#endif /* defined(__noctisgames__InputMemoryBitStream__) */
