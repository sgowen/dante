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
    _vertices.reserve(MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE);
}

LineBatcher::~LineBatcher()
{
    // Empty
}

void LineBatcher::beginBatch()
{
    _vertices.clear();
    
    _numLines = 0;
}

void LineBatcher::renderLine(Line &line, Color &c)
{
    float oX = line.getOrigin().getX();
    float oY = line.getOrigin().getY();
    float eX = line.getEnd().getX();
    float eY = line.getEnd().getY();
    
    _vertices.push_back(COLOR_VERTEX(oX, oY, c.red, c.green, c.blue, c.alpha));
    _vertices.push_back(COLOR_VERTEX(eX, eY, c.red, c.green, c.blue, c.alpha));
    
    _numLines++;
}

void LineBatcher::endBatch(NGShader &shader)
{
    if (_numLines > 0)
    {
        shader.bind(&_vertices);
        
        _rendererHelper->draw(NGPrimitiveType_Lines, 0, VERTICES_PER_LINE * _numLines);
        
        shader.unbind();
    }
}
