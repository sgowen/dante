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

#include <framework/math/Vector2.h>
#include <framework/math/Vector3.h>
#include <framework/graphics/portable/Color.h>

class NGLightingShader : public NGShader
{
public:
    NGLightingShader(RendererHelper& inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName);
    
    virtual void bind(void* vertices, void* data1 = NULL, void* data2 = NULL);
    virtual void unbind();
    
    void config(float resolutionX, float resolutionY, float lightPosX, float lightPosY);
    void configZ(float lightPosZ);
    
private:
    Vector2 _resolution;
    float _defaultLightZ;
    Vector3 _lightPos;
    Color _lightColor; // Light RGB and intensity (alpha)
    Color _ambientColor; // Ambient RGB and intensity (alpha)
    Vector3 _fallOff; // Attenuation coefficients for light falloff
};

#endif /* defined(__noctisgames__NGLightingShader__) */
