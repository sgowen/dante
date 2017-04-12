//
//  GridLockedPhysicalEntity.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/6/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "GridManager.h"

GridManager* GridManager::getInstance()
{
    static GridManager instance = GridManager();

    return &instance;
}

void GridManager::setGridCellSize(float gridCellSize)
{
    m_fGridCellSize = gridCellSize;
}

float GridManager::getGridCellSize()
{
    return m_fGridCellSize;
}

GridManager::GridManager()
{
    // Hide Constructor for Singleton
}
