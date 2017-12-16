//
//  OpenGLCircleBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/15/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/graphics/opengl/OpenGLCircleBatcher.h"

#include "framework/math/Circle.h"
#include "framework/graphics/portable/GpuProgramWrapper.h"
#include "framework/math/Color.h"

#include "framework/graphics/opengl/OpenGLManager.h"

OpenGLCircleBatcher::OpenGLCircleBatcher() : CircleBatcher()
{
    // Empty
}

void OpenGLCircleBatcher::clearVertices()
{
    OGLManager->getColorVertices().clear();
}

void OpenGLCircleBatcher::addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a)
{
    OGLManager->addVertexCoordinate(x, y, z, r, g, b, a);
    
    _numPoints++;
}

void OpenGLCircleBatcher::endBatch(GpuProgramWrapper &gpuProgramWrapper)
{
    gpuProgramWrapper.bind();
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, _numPoints);
    
    gpuProgramWrapper.unbind();
}
