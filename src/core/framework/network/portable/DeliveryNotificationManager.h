//
//  DeliveryNotificationManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DeliveryNotificationManager__
#define __noctisgames__DeliveryNotificationManager__

#include "InFlightPacket.h"
#include "AckRange.h"

#include <deque>
#include <stdint.h>

class OutputMemoryBitStream;
class InputMemoryBitStream;

class DeliveryNotificationManager
{
public:
    DeliveryNotificationManager(bool inShouldSendAcks, bool inShouldprocessAcks);
    
    ~DeliveryNotificationManager();
    
    InFlightPacket* writeState(OutputMemoryBitStream& inOutputStream);
    
    bool readAndProcessState(InputMemoryBitStream& inInputStream);
    
    void processTimedOutPackets(float frameStartTime);
    
    uint32_t getDroppedPacketCount() const;
    
    uint32_t getDeliveredPacketCount() const;
    
    uint32_t getDispatchedPacketCount() const;
    
    const std::deque<InFlightPacket>& getInFlightPackets() const;
    
private:
    std::deque<InFlightPacket> m_inFlightPackets;
    std::deque<AckRange> m_pendingAcks;
    
    uint16_t m_iNextOutgoingSequenceNumber;
    uint16_t m_iNextExpectedSequenceNumber;
    
    uint32_t m_iDeliveredPacketCount;
    uint32_t m_iDroppedPacketCount;
    uint32_t m_iDispatchedPacketCount;
    
    bool m_shouldSendAcks;
    bool m_shouldprocessAcks;
    
    InFlightPacket* writeSequenceNumber(OutputMemoryBitStream& inOutputStream);
    
    void writeAckData(OutputMemoryBitStream& inOutputStream);
    
    //returns wether to drop the packet- if sequence number is too low!
    bool processSequenceNumber(InputMemoryBitStream& inInputStream);
    
    void processAcks(InputMemoryBitStream& inInputStream);
    
    void addPendingAck(uint16_t inSequenceNumber);
    
    void handlePacketDeliveryFailure(const InFlightPacket& inFlightPacket);
    
    void handlePacketDeliverySuccess(const InFlightPacket& inFlightPacket);
};

#endif /* defined(__noctisgames__DeliveryNotificationManager__) */
