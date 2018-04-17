//
//  DirectXProgramLoader.h
//  noctisgames
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXProgramLoader__
#define __noctisgames__DirectXProgramLoader__

#include <framework/graphics/portable/ShaderProgramLoader.h>

#include <framework/graphics/portable/NGGraphics.h>

class DirectXProgramLoader : public ShaderProgramLoader
{
public:
    DirectXProgramLoader();
    virtual ~DirectXProgramLoader();
    
    virtual ShaderProgramWrapper* loadShader(std::string& vertexShaderFilePath, std::string& fragmentShaderFilePath, std::vector<NGShaderUniformInput*>& uniforms, std::vector<NGShaderVarInput*>& inputLayout);
    virtual void destroyShader(ShaderProgramWrapper* shaderProgramWrapper, std::vector<NGShaderUniformInput*>& uniforms, std::vector<NGShaderVarInput*>& inputLayout);
};

#endif /* defined(__noctisgames__DirectXProgramLoader__) */
