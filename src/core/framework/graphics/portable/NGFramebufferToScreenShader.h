//
//  NGFramebufferToScreenShader.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGFramebufferToScreenShader__
#define __noctisgames__NGFramebufferToScreenShader__

#include "framework/graphics/portable/NGShader.h"

class NGFramebufferToScreenShader : public NGShader
{
public:
    NGFramebufferToScreenShader(RendererHelper& inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName);
    
    virtual void bind(void* vertices, void* data1 = NULL, void* data2 = NULL);
    
    virtual void unbind();
};

#endif /* defined(__noctisgames__NGFramebufferToScreenShader__) */