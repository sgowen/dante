//
//  NGShaderLoader.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/17/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGShaderLoader__
#define __noctisgames__NGShaderLoader__

#include <vector>

struct ShaderProgramWrapper;
class NGShaderVarInput;
class NGShaderUniformInput;

class NGShaderLoader
{
public:
    NGShaderLoader();
    
    virtual ~NGShaderLoader();
    
    virtual ShaderProgramWrapper* loadNGShader(const char* vertexShaderName, const char* fragmentShaderName, std::vector<NGShaderUniformInput*>& uniforms, std::vector<NGShaderVarInput*>& inputLayout) = 0;
    
    virtual void destroyNGShader(ShaderProgramWrapper* shaderProgramWrapper, std::vector<NGShaderUniformInput*>& uniforms, std::vector<NGShaderVarInput*>& inputLayout) = 0;
};

#endif /* defined(__noctisgames__NGShaderLoader__) */
