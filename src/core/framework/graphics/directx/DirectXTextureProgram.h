//
//  DirectXTextureProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXTextureProgram__
#define __noctisgames__DirectXTextureProgram__

#include "framework/graphics/directx/DirectXProgram.h"

class DirectXTextureProgram : public DirectXProgram
{
public:
    DirectXTextureProgram(DirectXRendererHelper* inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName);
    
    virtual void bind();
};

#endif /* defined(__noctisgames__DirectXTextureProgram__) */
