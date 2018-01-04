//
//  LineBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/15/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/LineBatcher.h"

#include "framework/graphics/portable/RendererHelper.h"
#include "framework/graphics/portable/NGShader.h"
#include "framework/math/Line.h"
#include "framework/graphics/portable/Color.h"

#include <framework/util/FrameworkConstants.h>

LineBatcher::LineBatcher(RendererHelper* inRendererHelper) : _rendererHelper(inRendererHelper), _numLines(0)
{
    // Empty
}

LineBatcher::~LineBatcher()
{
    // Empty
}

void LineBatcher::beginBatch()
{
    _rendererHelper->clearColorVertices();
    _numLines = 0;
}

void LineBatcher::renderLine(Line &line, Color &c)
{
    float oX = line.getOrigin().getX();
    float oY = line.getOrigin().getY();
    float eX = line.getEnd().getX();
    float eY = line.getEnd().getY();
    
    _rendererHelper->addVertexCoordinate(oX, oY, 0, c.red, c.green, c.blue, c.alpha);
    _rendererHelper->addVertexCoordinate(eX, eY, 0, c.red, c.green, c.blue, c.alpha);
    
    _numLines++;
}

void LineBatcher::endBatch(NGShader &shader)
{
    if (_numLines > 0)
    {
        shader.bind();
        
        _rendererHelper->draw(NGPrimitiveType_Lines, 0, VERTICES_PER_LINE * _numLines);
        
        shader.unbind();
    }
}
