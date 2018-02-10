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

#include <framework/graphics/portable/NGGraphics.h>

class OpenGLProgramLoader : public ShaderProgramLoader
{
public:
    OpenGLProgramLoader();
    virtual ~OpenGLProgramLoader();
    
    virtual ShaderProgramWrapper* loadNGShader(std::string& vertexShaderFilePath, std::string& fragmentShaderFilePath, std::vector<NGShaderUniformInput*>& uniforms, std::vector<NGShaderVarInput*>& inputLayout);
    virtual void destroyNGShader(ShaderProgramWrapper* shaderProgramWrapper, std::vector<NGShaderUniformInput*>& uniforms, std::vector<NGShaderVarInput*>& inputLayout);
    
private:
    GLuint buildProgram(const void * vertex_shader_source, const int vertex_shader_source_length, const void * fragment_shader_source, const int fragment_shader_source_length);
    GLuint compileShader(const GLenum type, const void* source, const GLint length);
    GLuint linkProgram(const GLuint vertex_shader, const GLuint fragment_shader);
};

#endif /* defined(__noctisgames__OpenGLProgramLoader__) */
