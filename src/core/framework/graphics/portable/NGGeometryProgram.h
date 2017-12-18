//
//  NGGeometryProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGGeometryProgram__
#define __noctisgames__NGGeometryProgram__

#include "framework/graphics/portable/ShaderProgram.h"

#include <framework/graphics/portable/NGGraphics.h>

class NGGeometryProgram : public ShaderProgram
{
public:
    NGGeometryProgram(RendererHelper& inRendererHelper, ShaderProgramWrapper* inShaderProgramWrapper);
    
    virtual ~NGGeometryProgram();
    
    virtual void bind(void* data = NULL);
    
    virtual void unbind();
    
private:
    GLint u_mvp_matrix_location;
    GLint a_position_location;
    GLint a_color_location;
};

#endif /* defined(__noctisgames__NGGeometryProgram__) */
