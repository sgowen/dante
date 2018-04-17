//
//  NGWaterShader.h
//  noctisgames
//
//  Created by Stephen Gowen on 3/21/18.
//  Copyright (c) 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGWaterShader__
#define __noctisgames__NGWaterShader__

#include <framework/graphics/portable/NGShader.h>

#include <framework/graphics/portable/VertexProgramInput.h>

class NGWaterShader : public NGShader
{
public:
    NGWaterShader(RendererHelper& inRendererHelper);
    
    virtual void bind(void* data1 = NULL, void* data2 = NULL, void* data3 = NULL);
    virtual void unbind();
    
    void update(float deltaTime);
    
private:
    float4 _time;
    float _lastTime;
};

#endif /* defined(__noctisgames__NGWaterShader__) */
