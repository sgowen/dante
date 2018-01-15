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

#include <framework/math/Vector3.h>
#include <framework/graphics/portable/Color.h>

class NGLightingShader : public NGShader
{
public:
    NGLightingShader(RendererHelper& inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName);
    
    virtual void bind(void* vertices, void* data1 = NULL, void* data2 = NULL);
    virtual void unbind();
    
    void config(float lightPosX, float lightPosY);
    
private:
    static float DEFAULT_LIGHT_Z;
    
    static Vector3 LIGHT_POS;
    
    //Light RGB and intensity (alpha)
    static Color LIGHT_COLOR;
    
    //Ambient RGB and intensity (alpha)
    static Color AMBIENT_COLOR;
    
    //Attenuation coefficients for light falloff
    static Vector3 FALLOFF;
    
    Vector3 _lightPos;
};

#endif /* defined(__noctisgames__NGLightingShader__) */
