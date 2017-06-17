//
//  DeliveryNotificationManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "DeliveryNotificationManager.h"

#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"
#include "StringUtil.h"
#include "FrameworkConstants.h"

DeliveryNotificationManager::DeliveryNotificationManager(bool inShouldSendAcks, bool inShouldprocessAcks) :
m_iNextOutgoingSequenceNumber(0),
m_iNextExpectedSequenceNumber(0),
m_shouldSendAcks(inShouldSendAcks),
m_shouldprocessAcks(inShouldprocessAcks),
m_iDeliveredPacketCount(0),
m_iDroppedPacketCount(0),
m_iDispatchedPacketCount(0)
{
    // Empty
}

DeliveryNotificationManager::~DeliveryNotificationManager()
{
    if (m_iDispatchedPacketCount > 0
        && m_shouldprocessAcks)
    {
        LOG("DeliveryNotificationManager destructor. Delivery rate %d%%, Drop rate %d%%",
            (100 * m_iDeliveredPacketCount) / m_iDispatchedPacketCount,
            (100 * m_iDroppedPacketCount) / m_iDispatchedPacketCount);
    }
}

InFlightPacket* DeliveryNotificationManager::writeState(OutputMemoryBitStream& inOutputStream)
{
    InFlightPacket* toRet = writeSequenceNumber(inOutputStream);
    if (m_shouldSendAcks)
    {
        writeAckData(inOutputStream);
    }
    
    return toRet;
}

bool DeliveryNotificationManager::readAndProcessState(InputMemoryBitStream& inInputStream)
{
    bool toRet = processSequenceNumber(inInputStream);
    if (m_shouldprocessAcks)
    {
        processAcks(inInputStream);
    }
    
    return toRet;
}

void DeliveryNotificationManager::processTimedOutPackets(float frameStartTime)
{
    float timeoutTime = frameStartTime - NETWORK_ACK_TIMEOUT;
    
    while (!m_inFlightPackets.empty())
    {
        const auto& nextInFlightPacket = m_inFlightPackets.front();
        
        //was this packet dispatched before the current time minus the timeout duration?
        if (nextInFlightPacket.getTimeDispatched() < timeoutTime)
        {
            //it failed! let us know about that
            handlePacketDeliveryFailure(nextInFlightPacket);
            m_inFlightPackets.pop_front();
        }
        else
        {
            //it wasn't, and packets are all in order by time here, so we know we don't have to check farther
            break;
        }
    }
}

uint32_t DeliveryNotificationManager::getDroppedPacketCount() const
{
    return m_iDroppedPacketCount;
}

uint32_t DeliveryNotificationManager::getDeliveredPacketCount() const
{
    return m_iDeliveredPacketCount;
}

uint32_t DeliveryNotificationManager::getDispatchedPacketCount() const
{
    return m_iDispatchedPacketCount;
}

const std::deque<InFlightPacket>& DeliveryNotificationManager::getInFlightPackets() const
{
    return m_inFlightPackets;
}

InFlightPacket* DeliveryNotificationManager::writeSequenceNumber(OutputMemoryBitStream& inOutputStream)
{
    //write the sequence number, but also create an inflight packet for this...
    uint16_t sequenceNumber = m_iNextOutgoingSequenceNumber++;
    inOutputStream.write(sequenceNumber);
    
    ++m_iDispatchedPacketCount;
    
    if (m_shouldprocessAcks)
    {
        m_inFlightPackets.emplace_back(InFlightPacket(sequenceNumber));
        
        return &m_inFlightPackets.back();
    }
    else
    {
        return nullptr;
    }
}

void DeliveryNotificationManager::writeAckData(OutputMemoryBitStream& inOutputStream)
{
    //we usually will only have one packet to ack
    //so we'll follow that with a 0 bit if that's the case
    //however, if we have more than 1, we'll make that 1 bit a 1 and then write 8 bits of how many packets
    //we could do some statistical analysis to determine if this is the best strategy but we'll use it for now
    
    //do we have any pending acks?
    //if so, write a 1 bit and write the first range
    //otherwise, write 0 bit
    bool hasAcks = (m_pendingAcks.size() > 0);
    
    inOutputStream.write(hasAcks);
    if (hasAcks)
    {
        //note, we could write all the acks
        m_pendingAcks.front().write(inOutputStream);
        m_pendingAcks.pop_front();
    }
}

//returns wether to drop the packet- if sequence number is too low!
bool DeliveryNotificationManager::processSequenceNumber(InputMemoryBitStream& inInputStream)
{
    uint16_t sequenceNumber;
    
    inInputStream.read(sequenceNumber);
    if (sequenceNumber == m_iNextExpectedSequenceNumber)
    {
        m_iNextExpectedSequenceNumber = sequenceNumber + 1;
        //is this what we expect? great, let's add an ack to our pending list
        if (m_shouldSendAcks)
        {
            addPendingAck(sequenceNumber);
        }
        //and let's continue processing this packet...
        return true;
    }
    //is the sequence number less than our current expected sequence? silently drop it.
    //if this is due to wrapping around, we might fail to ack some packets that we should ack, but they'll get resent, so it's not a big deal
    //note that we don't have to re-ack it because our system doesn't reuse sequence numbers
    else if (sequenceNumber < m_iNextExpectedSequenceNumber)
    {
        return false;
    }
    else if (sequenceNumber > m_iNextExpectedSequenceNumber)
    {
        //we missed a lot of packets!
        //so our next expected packet comes after this one...
        m_iNextExpectedSequenceNumber = sequenceNumber + 1;
        //we should nack the missing packets..this will happen automatically inside addPendingAck because
        //we're adding an unconsequitive ack
        //and then we can ack this and process it
        if (m_shouldSendAcks)
        {
            addPendingAck(sequenceNumber);
        }
        return true;
    }
    
    //drop packet if we couldn't even read sequence number!
    return false;
}

//in each packet we can ack a range
//anything in flight before the range will be considered nackd by the other side immediately
void DeliveryNotificationManager::processAcks(InputMemoryBitStream& inInputStream)
{
    bool hasAcks;
    inInputStream.read(hasAcks);
    if (hasAcks)
    {
        AckRange ackRange;
        ackRange.read(inInputStream);
        
        //for each InfilghtPacket with a sequence number less than the start, handle delivery failure...
        uint16_t nextAckdSequenceNumber = ackRange.getStart();
        uint32_t onePastAckdSequenceNumber = nextAckdSequenceNumber + ackRange.getCount();
        while (nextAckdSequenceNumber < onePastAckdSequenceNumber && !m_inFlightPackets.empty())
        {
            const auto& nextInFlightPacket = m_inFlightPackets.front();
            //if the packet has a lower sequence number, we didn't get an ack for it, so it probably wasn't delivered
            uint16_t nextInFlightuint16_t = nextInFlightPacket.getSequenceNumber();
            if (nextInFlightuint16_t < nextAckdSequenceNumber)
            {
                //copy this so we can remove it before handling the failure- we don't want to find it when checking for state
                auto copyOfInFlightPacket = nextInFlightPacket;
                m_inFlightPackets.pop_front();
                handlePacketDeliveryFailure(copyOfInFlightPacket);
            }
            else if (nextInFlightuint16_t == nextAckdSequenceNumber)
            {
                handlePacketDeliverySuccess(nextInFlightPacket);
                //received!
                m_inFlightPackets.pop_front();
                //decrement count, advance nextAckdSequenceNumber
                ++nextAckdSequenceNumber;
            }
            else if (nextInFlightuint16_t > nextAckdSequenceNumber)
            {
                //we've already ackd some packets in here.
                //keep this packet in flight, but keep going through the ack...
                ++nextAckdSequenceNumber;
            }
        }
    }
}

void DeliveryNotificationManager::addPendingAck(uint16_t inSequenceNumber)
{
    //if you don't have a range yet, or you can't correctly extend the final range with the sequence number,
    //start a new range
    if (m_pendingAcks.size() == 0 || !m_pendingAcks.back().extendIfShould(inSequenceNumber))
    {
        m_pendingAcks.emplace_back(inSequenceNumber);
    }
}

void DeliveryNotificationManager::handlePacketDeliveryFailure(const InFlightPacket& inFlightPacket)
{
    ++m_iDroppedPacketCount;
    inFlightPacket.handleDeliveryFailure(this);
}

void DeliveryNotificationManager::handlePacketDeliverySuccess(const InFlightPacket& inFlightPacket)
{
    ++m_iDeliveredPacketCount;
    inFlightPacket.handleDeliverySuccess(this);
}
