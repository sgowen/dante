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
    
    NGTextureDesc(std::string inTextureName, bool repeatS = false, bool isEncrypted = false) : _textureName(inTextureName), _repeatS(repeatS), _isEncrypted(isEncrypted) {}
};

#endif /* defined(__noctisgames__NGTextureDesc__) */
