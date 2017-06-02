//
//  OutputMemoryBitStream.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "OutputMemoryBitStream.h"

#include "Vector2.h"
#include "Color.h"

OutputMemoryBitStream::OutputMemoryBitStream() : mBitHead(0), mBuffer(nullptr)
{
    ReallocBuffer(1500 * 8);
}

OutputMemoryBitStream::~OutputMemoryBitStream()
{
    std::free(mBuffer);
}

void OutputMemoryBitStream::writeBits(uint8_t inData, uint32_t inBitCount)
{
    uint32_t nextBitHead = mBitHead + static_cast<uint32_t>(inBitCount);
    
    if (nextBitHead > mBitCapacity)
    {
        ReallocBuffer(std::max(mBitCapacity * 2, nextBitHead));
    }
    
    //calculate the byteOffset into our buffer
    //by dividing the head by 8
    //and the bitOffset by taking the last 3 bits
    uint32_t byteOffset = mBitHead >> 3;
    uint32_t bitOffset = mBitHead & 0x7;
    
    uint8_t currentMask = ~(0xff << bitOffset);
    mBuffer[byteOffset] = (mBuffer[byteOffset] & currentMask) | (inData << bitOffset);
    
    //calculate how many bits were not yet used in
    //our target byte in the buffer
    uint32_t bitsFreeThisByte = 8 - bitOffset;
    
    //if we needed more than that, carry to the next byte
    if (bitsFreeThisByte < inBitCount)
    {
        //we need another byte
        mBuffer[byteOffset + 1] = inData >> bitsFreeThisByte;
    }
    
    mBitHead = nextBitHead;
}

void OutputMemoryBitStream::writeBits(const void* inData, uint32_t inBitCount)
{
    const char* srcByte = static_cast<const char*>(inData);
    //write all the bytes
    while (inBitCount > 8)
    {
        writeBits(*srcByte, 8);
        ++srcByte;
        inBitCount -= 8;
    }
    //write anything left
    if (inBitCount > 0)
    {
        writeBits(*srcByte, inBitCount);
    }
}

const char*	OutputMemoryBitStream::getBufferPtr() const
{
    return mBuffer;
}

uint32_t OutputMemoryBitStream::getBitLength() const
{
    return mBitHead;
}

uint32_t OutputMemoryBitStream::getByteLength() const
{
    return (mBitHead + 7) >> 3;
}

void OutputMemoryBitStream::writeBytes(const void* inData, uint32_t inByteCount)
{
    writeBits(inData, inByteCount << 3);
}

void OutputMemoryBitStream::write(bool inData)
{
    writeBits(&inData, 1);
}

void OutputMemoryBitStream::write(const Vector2& inVector)
{
    write(inVector.getX());
    write(inVector.getY());
}

void OutputMemoryBitStream::write(Color& inColor)
{
    write(inColor.red);
    write(inColor.green);
    write(inColor.blue);
    write(inColor.alpha);
}

void OutputMemoryBitStream::write(const std::string& inString)
{
    uint32_t elementCount = static_cast<uint32_t>(inString.size());
    write(elementCount);
    for (const auto& element : inString)
    {
        write(element);
    }
}

void OutputMemoryBitStream::ReallocBuffer(uint32_t inNewBitLength)
{
    if (mBuffer == nullptr)
    {
        //just need to memset on first allocation
        mBuffer = static_cast<char*>(std::malloc(inNewBitLength >> 3));
        memset(mBuffer, 0, inNewBitLength >> 3);
    }
    else
    {
        //need to memset, then copy the buffer
        char* tempBuffer = static_cast<char*>(std::malloc(inNewBitLength >> 3));
        memset(tempBuffer, 0, inNewBitLength >> 3);
        memcpy(tempBuffer, mBuffer, mBitCapacity >> 3);
        std::free(mBuffer);
        mBuffer = tempBuffer;
    }
    
    //handle realloc failure
    //...
    mBitCapacity = inNewBitLength;
}
