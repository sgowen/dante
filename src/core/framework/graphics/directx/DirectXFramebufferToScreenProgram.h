//
//  DirectXFramebufferToScreenProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXFramebufferToScreenProgram__
#define __noctisgames__DirectXFramebufferToScreenProgram__

#include "framework/graphics/directx/DirectXProgram.h"

class DirectXFramebufferToScreenProgram : public DirectXProgram
{
public:
    DirectXFramebufferToScreenProgram(DirectXRendererHelper* inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName);
    
    virtual void bind();
};

#endif /* defined(__noctisgames__DirectXFramebufferToScreenProgram__) */
