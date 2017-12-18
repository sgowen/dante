//
//  NGRectBatcher.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/25/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGRectBatcher__
#define __noctisgames__NGRectBatcher__

class RendererHelper;
class ShaderProgram;
class NGRect;
class Color;

class NGRectBatcher
{
public:
    NGRectBatcher(RendererHelper* inRendererHelper, bool isFill);
    
    ~NGRectBatcher();
    
    void beginBatch();
    
    void endBatch(ShaderProgram &gpuProgramWrapper);
    
    void renderNGRect(NGRect &r, Color &c);
    
private:
    RendererHelper* _rendererHelper;
    bool _isFill;
    int _numNGRects;
};

#endif /* defined(__noctisgames__NGRectBatcher__) */
