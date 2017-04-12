//
//  OpenGLFramebufferToScreenProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/4/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLFramebufferToScreenProgram__
#define __noctisgames__OpenGLFramebufferToScreenProgram__

#include "OpenGLProgram.h"

class OpenGLFramebufferToScreenProgram : public OpenGLProgram
{
public:
    OpenGLFramebufferToScreenProgram(const char* vertexShaderPath, const char* fragmentShaderPath);
    
    virtual void bind();
    
    virtual void unbind();
    
private:
    GLint u_texture_unit_location;
    GLint a_position_location;
};

#endif /* defined(__noctisgames__OpenGLFramebufferToScreenProgram__) */
