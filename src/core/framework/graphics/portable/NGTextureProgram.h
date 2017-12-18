//
//  NGTextureProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGTextureProgram__
#define __noctisgames__NGTextureProgram__

#include "framework/graphics/portable/ShaderProgram.h"

#include <framework/graphics/portable/NGGraphics.h>

class NGTextureProgram : public ShaderProgram
{
public:
    NGTextureProgram(RendererHelper& inRendererHelper, ShaderProgramWrapper* inShaderProgramWrapper);
    
    virtual ~NGTextureProgram();
    
    virtual void bind(void* data = NULL);
    
    virtual void unbind();
    
private:
    GLint u_mvp_matrix_location;
    GLint u_texture_unit_location;
    GLint a_position_location;
    GLint a_color_location;
    GLint a_texture_coordinates_location;
};

#endif /* defined(__noctisgames__NGTextureProgram__) */
