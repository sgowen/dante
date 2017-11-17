//
//  OpenGLNGRectBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/15/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "OpenGLNGRectBatcher.h"

#include "macros.h"
#include "NGRect.h"
#include "Vector2.h"
#include "GpuProgramWrapper.h"
#include "OpenGLManager.h"
#include "Color.h"

OpenGLNGRectBatcher::OpenGLNGRectBatcher(bool isFill) : NGRectBatcher(isFill)
{
    // Empty
}

void OpenGLNGRectBatcher::beginBatch()
{
    OGLManager->getColorVertices().clear();
    _numNGRects = 0;
}

void OpenGLNGRectBatcher::endBatch(GpuProgramWrapper &gpuProgramWrapper)
{
    if (_numNGRects > 0)
    {
        gpuProgramWrapper.bind();
        
        if (_isFill)
        {
            glDrawElements(GL_TRIANGLES, _numNGRects * INDICES_PER_RECTANGLE, GL_UNSIGNED_SHORT, &OGLManager->getIndices()[0]);
        }
        else
        {
            glDrawArrays(GL_LINES, 0, VERTICES_PER_LINE * _numNGRects * 4);
        }
        
        gpuProgramWrapper.unbind();
    }
}

void OpenGLNGRectBatcher::renderNGRect(float x1, float y1, float x2, float y2, Color &c)
{
    if (_isFill)
    {
        OGLManager->addVertexCoordinate(x1, y1, 0, c.red, c.green, c.blue, c.alpha);
        OGLManager->addVertexCoordinate(x1, y2, 0, c.red, c.green, c.blue, c.alpha);
        OGLManager->addVertexCoordinate(x2, y2, 0, c.red, c.green, c.blue, c.alpha);
        OGLManager->addVertexCoordinate(x2, y1, 0, c.red, c.green, c.blue, c.alpha);
    }
    else
    {
        OGLManager->addVertexCoordinate(x1, y1, 0, c.red, c.green, c.blue, c.alpha);
        OGLManager->addVertexCoordinate(x2, y1, 0, c.red, c.green, c.blue, c.alpha);
        
        OGLManager->addVertexCoordinate(x2, y1, 0, c.red, c.green, c.blue, c.alpha);
        OGLManager->addVertexCoordinate(x2, y2, 0, c.red, c.green, c.blue, c.alpha);
        
        OGLManager->addVertexCoordinate(x2, y2, 0, c.red, c.green, c.blue, c.alpha);
        OGLManager->addVertexCoordinate(x1, y2, 0, c.red, c.green, c.blue, c.alpha);
        
        OGLManager->addVertexCoordinate(x1, y2, 0, c.red, c.green, c.blue, c.alpha);
        OGLManager->addVertexCoordinate(x1, y1, 0, c.red, c.green, c.blue, c.alpha);
    }

    _numNGRects++;
}
