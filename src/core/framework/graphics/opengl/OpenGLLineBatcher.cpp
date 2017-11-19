//
//  OpenGLLineBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/15/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/graphics/opengl/OpenGLLineBatcher.h"

#include "framework/util/macros.h"
#include "framework/math/Line.h"
#include "framework/math/Vector2.h"
#include "framework/graphics/opengl/OpenGLManager.h"
#include "framework/graphics/portable/GpuProgramWrapper.h"
#include "framework/math/Color.h"

OpenGLLineBatcher::OpenGLLineBatcher() : LineBatcher()
{
    _numLines = 0;
}

OpenGLLineBatcher::~OpenGLLineBatcher()
{
    // Empty
}

void OpenGLLineBatcher::beginBatch()
{
    OGLManager->getColorVertices().clear();
    _numLines = 0;
}

void OpenGLLineBatcher::endBatch(GpuProgramWrapper &gpuProgramWrapper)
{
    if (_numLines > 0)
    {
        gpuProgramWrapper.bind();
        
        glDrawArrays(GL_LINES, 0, VERTICES_PER_LINE * _numLines);
        
        gpuProgramWrapper.unbind();
    }
}

void OpenGLLineBatcher::renderLine(float originX, float originY, float endX, float endY, Color &c)
{
    OGLManager->addVertexCoordinate(originX, originY, 0, c.red, c.green, c.blue, c.alpha);
    OGLManager->addVertexCoordinate(endX, endY, 0, c.red, c.green, c.blue, c.alpha);
    
    _numLines++;
}
