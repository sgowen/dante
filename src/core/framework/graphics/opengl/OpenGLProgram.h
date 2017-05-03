//
//  OpenGLProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/2/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLProgram__
#define __noctisgames__OpenGLProgram__

#include "pch.h"

#include <vector>

#define BUFFER_OFFSET(i) ((void*)(i))

class OpenGLProgram
{
public:
    OpenGLProgram(const char* vertexShaderName, const char* fragmentShaderName);
    
    virtual ~OpenGLProgram();
    
    virtual void bind();
    
    virtual void unbind();
    
    GLuint getProgramObjectId();
    
protected:
    GLuint m_programObjectId;
    
    void mapBuffer(GLuint& vbo, std::vector<GLfloat>& vertices);
    
    void unmapBuffer(GLuint& vbo);
    
private:
    GLuint buildProgram(const void * vertex_shader_source, const int vertex_shader_source_length, const void * fragment_shader_source, const int fragment_shader_source_length);
    
    GLuint compileShader(const GLenum type, const void* source, const GLint length);
    
    GLuint linkProgram(const GLuint vertex_shader, const GLuint fragment_shader);
};

#endif /* defined(__noctisgames__OpenGLProgram__) */
