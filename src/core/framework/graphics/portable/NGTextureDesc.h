//
//  NGTextureDesc.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/21/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGTextureDesc__
#define __noctisgames__NGTextureDesc__

#include <string>

struct NGTextureDesc
{
    std::string _textureName;
    bool _repeatS;
    bool _isEncrypted;
    bool _hasNormal;
    
    NGTextureDesc(std::string inTextureName, bool repeatS, bool isEncrypted, bool hasNormal) :
    _textureName(inTextureName),
    _repeatS(repeatS),
    _isEncrypted(isEncrypted),
    _hasNormal(hasNormal)
    {
        // I normally don't allow definitions in headers, but sometimes I'm cool with it
    }
};

#endif /* defined(__noctisgames__NGTextureDesc__) */
