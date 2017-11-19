//
//  OpenGLCircleBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/15/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#define DEGREE_SPACING 6

#include "framework/graphics/opengl/OpenGLCircleBatcher.h"
#include "framework/util/macros.h"
#include "framework/math/Circle.h"
#include "framework/math/Vector2.h"
#include "framework/graphics/opengl/OpenGLManager.h"
#include "framework/graphics/portable/DummyGpuProgramWrapper.h"
#include "framework/math/Color.h"

#include <math.h>

OpenGLCircleBatcher::OpenGLCircleBatcher() : CircleBatcher()
{
    // Empty
}

void OpenGLCircleBatcher::renderCircle(Circle &circle, Color &c, GpuProgramWrapper &gpuProgramWrapper)
{
    OGLManager->getColorVertices().clear();
    
    _numPoints = 0;
    
    for (int i = 0; i < 360; i += DEGREE_SPACING)
    {
        float rad = DEGREES_TO_RADIANS(i);
        float cos = cosf(rad);
        float sin = sinf(rad);
        
        OGLManager->addVertexCoordinate(cos * circle.getRadius() + circle.getCenter().getX(), sin * circle.getRadius() + circle.getCenter().getY(), 0, c.red, c.green, c.blue, c.alpha);
        
        _numPoints++;
    }
    
    endBatch(gpuProgramWrapper);
}

void OpenGLCircleBatcher::renderPartialCircle(Circle &circle, int arcDegrees, Color &c, GpuProgramWrapper &gpuProgramWrapper)
{
    OGLManager->getColorVertices().clear();
    
    OGLManager->addVertexCoordinate(circle.getCenter().getX(), circle.getCenter().getY(), 0, c.red, c.green, c.blue, c.alpha);
    
    _numPoints = 1;
    
    for (int i = 90 - arcDegrees; i > -270; i -= DEGREE_SPACING)
    {
        float rad = DEGREES_TO_RADIANS(i);
        float cos = cosf(rad);
        float sin = sinf(rad);
        
        OGLManager->addVertexCoordinate(cos * circle.getRadius() + circle.getCenter().getX(), sin * circle.getRadius() + circle.getCenter().getY(), 0, c.red, c.green, c.blue, c.alpha);
        
        _numPoints++;
    }
    
    float rad = DEGREES_TO_RADIANS(-270);
    float cos = cosf(rad);
    float sin = sinf(rad);
    
    OGLManager->addVertexCoordinate(cos * circle.getRadius() + circle.getCenter().getX(), sin * circle.getRadius() + circle.getCenter().getY(), 0, c.red, c.green, c.blue, c.alpha);
    
    _numPoints++;
    
    endBatch(gpuProgramWrapper);
}

void OpenGLCircleBatcher::endBatch(GpuProgramWrapper &gpuProgramWrapper)
{
    gpuProgramWrapper.bind();
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, _numPoints);
    
    gpuProgramWrapper.unbind();
}
