//
//  NGShader.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGShader__
#define __noctisgames__NGShader__

#include <cstddef>
#include <vector>

class RendererHelper;
class NGShaderLoader;
class NGShaderUniformInput;
class NGShaderVarInput;
struct ShaderProgramWrapper;

class NGShader
{
public:
    NGShader(RendererHelper& inRendererHelper, NGShaderLoader& inNGShaderLoader, const char* vertexShaderName, const char* fragmentShaderName);
    
    virtual ~NGShader();
    
    virtual void bind(void* data = NULL) = 0;
    
    virtual void unbind() = 0;
    
protected:
    RendererHelper& _rendererHelper;
    NGShaderLoader& _shaderProgramLoader;
    const char* _vertexShaderName;
    const char* _fragmentShaderName;
    std::vector<NGShaderUniformInput*> _uniforms;
    std::vector<NGShaderVarInput*> _inputLayout;
    ShaderProgramWrapper* _shaderProgramWrapper;
    
    void load();
};

#endif /* defined(__noctisgames__NGShader__) */
