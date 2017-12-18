//
//  NGFramebufferToScreenProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGFramebufferToScreenProgram__
#define __noctisgames__NGFramebufferToScreenProgram__

#include "framework/graphics/portable/ShaderProgram.h"

#include <framework/graphics/portable/NGGraphics.h>

class NGFramebufferToScreenProgram : public ShaderProgram
{
public:
    NGFramebufferToScreenProgram(RendererHelper& inRendererHelper, ShaderProgramWrapper* inShaderProgramWrapper);
    
    virtual ~NGFramebufferToScreenProgram();
    
    virtual void bind(void* data = NULL);
    
    virtual void unbind();
    
private:
    GLint u_texture_unit_location;
    GLint a_position_location;
};

#endif /* defined(__noctisgames__NGFramebufferToScreenProgram__) */
