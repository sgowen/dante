//
//  NGShaderUniformInput.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGShaderUniformInput__
#define __noctisgames__NGShaderUniformInput__

#include <framework/graphics/portable/NGGraphics.h>

class ShaderProgramWrapper;

class NGShaderUniformInput
{
public:
    const char* _attribName;
    int _index;
    int _byteWidth;
    bool _isFragment;
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    GLint _attribute;
#elif _WIN32
    Microsoft::WRL::ComPtr<ID3D11Buffer> _constantbuffer;
#endif
    
    NGShaderUniformInput(const char* attribName, int index, int byteWidth = 0, bool isFragment = false);
    
    void build(ShaderProgramWrapper* inShaderProgramWrapper);
};

#endif /* defined(__noctisgames__NGShaderUniformInput__) */
