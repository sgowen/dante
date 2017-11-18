//
//  DirectXTextureProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXTextureProgram__
#define __noctisgames__DirectXTextureProgram__

#include "DirectXProgram.h"

class DirectXTextureProgram : public DirectXProgram
{
public:
    DirectXTextureProgram(const char* vertexShaderName, const char* pixelShaderName);
    
    virtual void mapVertices();
};

#endif /* defined(__noctisgames__DirectXTextureProgram__) */
