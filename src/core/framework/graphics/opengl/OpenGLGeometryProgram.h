//
//  OpenGLGeometryProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/4/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLGeometryProgram__
#define __noctisgames__OpenGLGeometryProgram__

#include "OpenGLProgram.h"

class OpenGLGeometryProgram : public OpenGLProgram
{
public:
    OpenGLGeometryProgram(const char* vertexShaderName, const char* fragmentShaderName);
    
    virtual void bind();
    
    virtual void unbind();
    
private:
    GLint u_mvp_matrix_location;
    GLint a_position_location;
    GLint a_color_location;
};

#endif /* defined(__noctisgames__OpenGLGeometryProgram__) */
