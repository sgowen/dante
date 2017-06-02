//
//  InputMemoryBitStream.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "InputMemoryBitStream.h"

#include "Vector2.h"
#include "Color.h"

void InputMemoryBitStream::ReadBits(uint8_t& outData, uint32_t inBitCount)
{
    uint32_t byteOffset = mBitHead >> 3;
    uint32_t bitOffset = mBitHead & 0x7;
    
    outData = static_cast<uint8_t>(mBuffer[byteOffset]) >> bitOffset;
    
    uint32_t bitsFreeThisByte = 8 - bitOffset;
    if (bitsFreeThisByte < inBitCount)
    {
        //we need another byte
        outData |= static_cast<uint8_t>(mBuffer[byteOffset + 1]) << bitsFreeThisByte;
    }
    
    //don't forget a mask so that we only read the bit we wanted...
    outData &= (~(0x00ff << inBitCount));
    
    mBitHead += inBitCount;
}

void InputMemoryBitStream::ReadBits(void* outData, uint32_t inBitCount)
{
    uint8_t* destByte = reinterpret_cast< uint8_t* >(outData);
    // read all the bytes
    while (inBitCount > 8)
    {
        ReadBits(*destByte, 8);
        ++destByte;
        inBitCount -= 8;
    }
    
    // read anything left
    if (inBitCount > 0)
    {
        ReadBits(*destByte, inBitCount);
    }
}

void InputMemoryBitStream::read(Vector2& outVector)
{
    read(outVector.getXRef());
    read(outVector.getYRef());
}

void InputMemoryBitStream::read(Color& outColor)
{
    read(outColor.red);
    read(outColor.green);
    read(outColor.blue);
    read(outColor.alpha);
}
