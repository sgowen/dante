//
//  OpenGLProgramLoader.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/2/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLProgramLoader__
#define __noctisgames__OpenGLProgramLoader__

#include <framework/graphics/portable/ShaderProgramLoader.h>

#include "framework/graphics/portable/NGGraphics.h"

class OpenGLProgramLoader : public ShaderProgramLoader
{
public:
    OpenGLProgramLoader();
    
    virtual ~OpenGLProgramLoader();
    
    virtual ShaderProgramWrapper* loadShaderProgram(const char* vertexShaderName, const char* fragmentShaderName);
    
private:
    GLuint buildProgram(const void * vertex_shader_source, const int vertex_shader_source_length, const void * fragment_shader_source, const int fragment_shader_source_length);
    
    GLuint compileShader(const GLenum type, const void* source, const GLint length);
    
    GLuint linkProgram(const GLuint vertex_shader, const GLuint fragment_shader);
};

#endif /* defined(__noctisgames__OpenGLProgramLoader__) */
