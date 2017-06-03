//
//  AckRange.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "AckRange.h"

#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"

AckRange::AckRange() : m_iStart(0), m_iCount(0)
{
    // Empty
}

AckRange::AckRange(uint16_t inStart) : m_iStart(inStart), m_iCount(1)
{
    // Empty
}

void AckRange::write(OutputMemoryBitStream& inOutputStream) const
{
    inOutputStream.write(m_iStart);
    bool hasCount = m_iCount > 1;
    inOutputStream.write(hasCount);
    if (hasCount)
    {
        //most you can ack is 255...
        uint32_t countMinusOne = m_iCount - 1;
        uint8_t countToAck = countMinusOne > 255 ? 255 : static_cast<uint8_t>(countMinusOne);
        inOutputStream.write(countToAck);
    }
}

void AckRange::read(InputMemoryBitStream& inInputStream)
{
    inInputStream.read(m_iStart);
    bool hasCount;
    inInputStream.read(hasCount);
    if (hasCount)
    {
        uint8_t countMinusOne;
        inInputStream.read(countMinusOne);
        m_iCount = countMinusOne + 1;
    }
    else
    {
        //default!
        m_iCount = 1;
    }
}

bool AckRange::extendIfShould(uint16_t inSequenceNumber)
{
    if (inSequenceNumber == m_iStart + m_iCount)
    {
        ++m_iCount;
        return true;
    }
    else
    {
        return false;
    }
}

uint16_t AckRange::getStart() const
{
    return m_iStart;
}

uint32_t AckRange::getCount() const
{
    return m_iCount;
}
