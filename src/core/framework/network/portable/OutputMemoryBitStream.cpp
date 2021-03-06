//
//  OutputMemoryBitStream.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/portable/OutputMemoryBitStream.h>

#include <Box2D/Box2D.h>
#include <framework/util/MathUtil.h>
#include <framework/util/StringUtil.h>
#include <framework/util/Constants.h>

#include <cstring>	// memcpy()

OutputMemoryBitStream::OutputMemoryBitStream() : _bitHead(0), _buffer(NULL)
{
    reallocBuffer(NW_MAX_PACKET_SIZE * 8);
}

OutputMemoryBitStream::~OutputMemoryBitStream()
{
    std::free(_buffer);
}

const char* OutputMemoryBitStream::getBufferPtr() const
{
    return _buffer;
}

uint32_t OutputMemoryBitStream::getBitLength() const
{
    return _bitHead;
}

uint32_t OutputMemoryBitStream::getByteLength() const
{
    return (_bitHead + 7) >> 3;
}

void OutputMemoryBitStream::writeBits(uint8_t inData, uint32_t inBitCount)
{
    uint32_t nextBitHead = _bitHead + static_cast<uint32_t>(inBitCount);
    
    if (nextBitHead > _bitCapacity)
    {
        reallocBuffer(std::max(_bitCapacity * 2, nextBitHead));
    }
    
    //calculate the byteOffset into our buffer
    //by dividing the head by 8
    //and the bitOffset by taking the last 3 bits
    uint32_t byteOffset = _bitHead >> 3;
    uint32_t bitOffset = _bitHead & 0x7;
    
    uint8_t currentMask = ~(0xff << bitOffset);
    _buffer[byteOffset] = (_buffer[byteOffset] & currentMask) | (inData << bitOffset);
    
    //calculate how many bits were not yet used in
    //our target byte in the buffer
    uint32_t bitsFreeThisByte = 8 - bitOffset;
    
    //if we needed more than that, carry to the next byte
    if (bitsFreeThisByte < inBitCount)
    {
        //we need another byte
        _buffer[byteOffset + 1] = inData >> bitsFreeThisByte;
    }
    
    _bitHead = nextBitHead;
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

void OutputMemoryBitStream::writeBytes(const void* inData, uint32_t inByteCount)
{
    writeBits(inData, inByteCount << 3);
}

void OutputMemoryBitStream::write(bool inData)
{
    writeBits(&inData, 1);
}

void OutputMemoryBitStream::writeLarge(const std::string& inString)
{
    uint16_t elementCount = static_cast<uint16_t>(inString.size());
    assert(elementCount <= 1198);
    write(elementCount);
    for (const char& element : inString)
    {
        write(element);
    }
}

void OutputMemoryBitStream::writeSmall(const std::string& inString)
{
    uint8_t elementCount = static_cast<uint8_t>(inString.size());
    write(elementCount);
    for (const auto& element : inString)
    {
        write(element);
    }
}

void OutputMemoryBitStream::writeSignedBinaryValue(float inValue)
{
    bool isNonZero = (inValue != 0.f);
    write(isNonZero);
    if (isNonZero)
    {
        write(inValue > 0.f);
    }
}

void OutputMemoryBitStream::reallocBuffer(uint32_t inNewBitLength)
{
    if (_buffer == NULL)
    {
        //just need to memset on first allocation
        _buffer = static_cast<char*>(std::malloc(inNewBitLength >> 3));
        memset(_buffer, 0, inNewBitLength >> 3);
    }
    else
    {
        //need to memset, then copy the buffer
        char* tempBuffer = static_cast<char*>(std::malloc(inNewBitLength >> 3));
        memset(tempBuffer, 0, inNewBitLength >> 3);
        memcpy(tempBuffer, _buffer, _bitCapacity >> 3);
        std::free(_buffer);
        _buffer = tempBuffer;
    }
    
    //handle realloc failure
    //...
    _bitCapacity = inNewBitLength;
}
