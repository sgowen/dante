//
//  GridManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/6/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "GridManager.h"

GridManager* GridManager::getInstance()
{
    static GridManager instance = GridManager();

    return &instance;
}

void GridManager::setGridCellSize(float gridCellSize)
{
    _gridCellSize = gridCellSize;
}

float GridManager::getGridCellSize()
{
    return _gridCellSize;
}

GridManager::GridManager()
{
    // Hide Constructor for Singleton
}
