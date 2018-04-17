//
//  InputMemoryBitStream.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/portable/InputMemoryBitStream.h>

#include <cstring>	// memcpy()

InputMemoryBitStream::InputMemoryBitStream(char* inBuffer, uint32_t inBitCount) :
_buffer(inBuffer),
_bitCapacity(inBitCount),
_bitHead(0),
_isBufferOwner(false)
{
    // Empty
}

InputMemoryBitStream::InputMemoryBitStream(const InputMemoryBitStream& inOther) :
_bitCapacity(inOther._bitCapacity),
_bitHead(inOther._bitHead),
_isBufferOwner(true)
{
    int byteCount = (_bitCapacity + 7) / 8;
    _buffer = static_cast<char*>(malloc(byteCount));
    memcpy(_buffer, inOther._buffer, byteCount);
}

InputMemoryBitStream::~InputMemoryBitStream()
{
    if (_isBufferOwner)
    {
        free(_buffer);
    }
}

const char*	InputMemoryBitStream::getBufferPtr() const
{
    return _buffer;
}

uint32_t InputMemoryBitStream::getRemainingBitCount() const
{
    return _bitCapacity - _bitHead;
}

void InputMemoryBitStream::readBits(uint8_t& outData, uint32_t inBitCount)
{
    uint32_t byteOffset = _bitHead >> 3;
    uint32_t bitOffset = _bitHead & 0x7;
    
    outData = static_cast<uint8_t>(_buffer[byteOffset]) >> bitOffset;
    
    uint32_t bitsFreeThisByte = 8 - bitOffset;
    if (bitsFreeThisByte < inBitCount)
    {
        //we need another byte
        outData |= static_cast<uint8_t>(_buffer[byteOffset + 1]) << bitsFreeThisByte;
    }
    
    //don't forget a mask so that we only read the bit we wanted...
    outData &= (~(0x00ff << inBitCount));
    
    _bitHead += inBitCount;
}

void InputMemoryBitStream::readBits(void* outData, uint32_t inBitCount)
{
    uint8_t* destByte = reinterpret_cast< uint8_t* >(outData);
    // read all the bytes
    while (inBitCount > 8)
    {
        readBits(*destByte, 8);
        ++destByte;
        inBitCount -= 8;
    }
    
    // read anything left
    if (inBitCount > 0)
    {
        readBits(*destByte, inBitCount);
    }
}

void InputMemoryBitStream::readBytes(void* outData, uint32_t inByteCount)
{
    readBits(outData, inByteCount << 3);
}

void InputMemoryBitStream::readSignedBinaryValue(float& outValue)
{
    bool isNonZero;
    read(isNonZero);
    if (isNonZero)
    {
        bool isPositive;
        read(isPositive);
        outValue = isPositive ? 1.f : -1.f;
    }
    else
    {
        outValue = 0.f;
    }
}

void InputMemoryBitStream::read(bool& outData)
{
    readBits(&outData, 1);
}

void InputMemoryBitStream::resetToCapacity(uint32_t inByteCapacity)
{
    _bitCapacity = inByteCapacity << 3; _bitHead = 0;
}

void InputMemoryBitStream::readLarge(std::string& inString)
{
    uint16_t elementCount;
    read(elementCount);
    
    inString.resize(elementCount);
    for (auto& element : inString)
    {
        read(element);
    }
}

void InputMemoryBitStream::readSmall(std::string& inString)
{
    uint8_t elementCount;
    read(elementCount);
    
    inString.resize(elementCount);
    for (auto& element : inString)
    {
        read(element);
    }
}
