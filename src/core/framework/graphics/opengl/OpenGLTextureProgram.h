//
//  OpenGLTextureProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/4/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLTextureProgram__
#define __noctisgames__OpenGLTextureProgram__

#include "OpenGLProgram.h"

class OpenGLTextureProgram : public OpenGLProgram
{
public:
    OpenGLTextureProgram(const char* vertexShaderName, const char* fragmentShaderName);
    
    virtual void bind();
    
    virtual void unbind();
    
private:
    GLint u_mvp_matrix_location;
    GLint u_texture_unit_location;
    GLint a_position_location;
    GLint a_color_location;
    GLint a_texture_coordinates_location;
};

#endif /* defined(__noctisgames__OpenGLTextureProgram__) */
