//
//  RTTI.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/4/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "RTTI.h"

RTTI::RTTI(const std::string& className) : _className(className), _pBaseRTTI(nullptr)
{
    // Empty
}

RTTI::RTTI(const std::string& className, const RTTI& baseRTTI) : _className(className), _pBaseRTTI(&baseRTTI)
{
    // Empty
}

const std::string& RTTI::getClassName() const
{
    return _className;
}

bool RTTI::isExactly(const RTTI& rtti) const
{
    return (this == &rtti);
}

bool RTTI::derivesFrom(const RTTI& rtti) const
{
    const RTTI * pCompare = this;
    
    while (pCompare)
    {
        if (pCompare == &rtti)
        {
            return true;
        }
        
        pCompare = pCompare->_pBaseRTTI;
    }
    
    return false;
}
