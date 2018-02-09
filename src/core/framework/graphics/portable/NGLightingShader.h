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

#define NUM_LIGHTS 12

class NGLightingShader : public NGShader
{
public:
    NGLightingShader(RendererHelper& inRendererHelper);
    
    virtual void bind(void* data1 = NULL, void* data2 = NULL, void* data3 = NULL);
    virtual void unbind();
    
    void resetLights();
    void configAmbientLight(float r, float g, float b, float a);
    void configureFallOff(float x, float y, float z);
    void addLight(float lightPosX, float lightPosY, float lightPosZ, float lightColorR, float lightColorG, float lightColorB, float lightColorA);
    
private:
    float4 _lightPositions[NUM_LIGHTS];
    float4 _lightColors[NUM_LIGHTS]; // Light RGB and intensity (alpha)
    float4 _ambientColor; // Ambient RGB and intensity (alpha)
    float4 _fallOff; // Attenuation coefficients for light falloff
    int4 _numLights;
    int _lightIndex;
};

#endif /* defined(__noctisgames__NGLightingShader__) */
