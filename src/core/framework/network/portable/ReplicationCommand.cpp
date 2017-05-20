//
//  ReplicationCommand.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "ReplicationCommand.h"

ReplicationCommand::ReplicationCommand()
{
    // Empty
}

ReplicationCommand::ReplicationCommand(uint32_t inInitialDirtyState) : mAction(RA_Create), mDirtyState(inInitialDirtyState)
{
    // Empty
}

void ReplicationCommand::HandleCreateAckd()
{
    if (mAction == RA_Create)
    {
        // if the create is ack'd, we can demote to just an update...
        mAction = RA_Update;
    }
}

void ReplicationCommand::AddDirtyState(uint32_t inState)
{
    mDirtyState |= inState;
}

void ReplicationCommand::SetDestroy()
{
    mAction = RA_Destroy;
}

bool ReplicationCommand::HasDirtyState() const
{
    return (mAction == RA_Destroy) || (mDirtyState != 0);
}

ReplicationAction ReplicationCommand::GetAction() const
{
    return mAction;
}

uint32_t ReplicationCommand::GetDirtyState() const
{
    return mDirtyState;
}

void ReplicationCommand::ClearDirtyState(uint32_t inStateToClear)
{
    mDirtyState &= ~inStateToClear;
    
    if (mAction == RA_Destroy)
    {
        mAction = RA_Update;
    }
}
