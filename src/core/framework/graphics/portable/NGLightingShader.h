//
//  NGLightingShader.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/14/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGLightingShader__
#define __noctisgames__NGLightingShader__

#include "framework/graphics/portable/NGShader.h"

#include <framework/graphics/portable/VertexProgramInput.h>

class NGLightingShader : public NGShader
{
public:
    NGLightingShader(RendererHelper& inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName);
    
    virtual void bind(void* vertices, void* data1 = NULL, void* data2 = NULL, void* data3 = NULL);
    virtual void unbind();
    virtual void onLoaded();
    
    void onConfigLoaded();
    void resetLights();
    void configLight(int index, float lightPosX, float lightPosY);
    void configZ(float lightPosZ);
    
private:
    mat4x4 _lights;
    float4 _lightColor; // Light RGB and intensity (alpha)
    float4 _ambientColor; // Ambient RGB and intensity (alpha)
    float4 _fallOff; // Attenuation coefficients for light falloff
    int4 _numLights;
};

#endif /* defined(__noctisgames__NGLightingShader__) */
