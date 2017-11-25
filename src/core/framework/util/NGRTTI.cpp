//
//  NGRTTI.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/4/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/util/NGRTTI.h"

NGRTTI::NGRTTI(const std::string& className) : _className(className), _pBaseNGRTTI(NULL)
{
    // Empty
}

NGRTTI::NGRTTI(const std::string& className, const NGRTTI& baseNGRTTI) : _className(className), _pBaseNGRTTI(&baseNGRTTI)
{
    // Empty
}

const std::string& NGRTTI::getClassName() const
{
    return _className;
}

bool NGRTTI::isExactly(const NGRTTI& rtti) const
{
    return (this == &rtti);
}

bool NGRTTI::derivesFrom(const NGRTTI& rtti) const
{
    const NGRTTI * pCompare = this;
    
    while (pCompare)
    {
        if (pCompare == &rtti)
        {
            return true;
        }
        
        pCompare = pCompare->_pBaseNGRTTI;
    }
    
    return false;
}
