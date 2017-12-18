//
//  ShaderProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ShaderProgram__
#define __noctisgames__ShaderProgram__

#include <cstddef>
#include <vector>

class RendererHelper;
class ShaderProgramLoader;
class NGShaderUniformInput;
class NGShaderVarInput;
struct ShaderProgramWrapper;

class ShaderProgram
{
public:
    ShaderProgram(RendererHelper& inRendererHelper, ShaderProgramLoader& inShaderProgramLoader, const char* vertexShaderName, const char* fragmentShaderName);
    
    virtual ~ShaderProgram();
    
    virtual void bind(void* data = NULL) = 0;
    
    virtual void unbind() = 0;
    
protected:
    RendererHelper& _rendererHelper;
    ShaderProgramLoader& _shaderProgramLoader;
    const char* _vertexShaderName;
    const char* _fragmentShaderName;
    std::vector<NGShaderUniformInput*> _uniforms;
    std::vector<NGShaderVarInput*> _inputLayout;
    ShaderProgramWrapper* _shaderProgramWrapper;
    
    void load();
};

#endif /* defined(__noctisgames__ShaderProgram__) */
