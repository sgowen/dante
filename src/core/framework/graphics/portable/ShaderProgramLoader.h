//
//  ShaderProgramLoader.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/17/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ShaderProgramLoader__
#define __noctisgames__ShaderProgramLoader__

#include <vector>

struct ShaderProgramWrapper;
class NGShaderVarInput;
class NGShaderUniformInput;

class ShaderProgramLoader
{
public:
    ShaderProgramLoader();
    
    virtual ~ShaderProgramLoader();
    
    virtual ShaderProgramWrapper* loadShaderProgram(const char* vertexShaderName, const char* fragmentShaderName, std::vector<NGShaderUniformInput*>& uniforms, std::vector<NGShaderVarInput*>& inputLayout) = 0;
};

#endif /* defined(__noctisgames__ShaderProgramLoader__) */
