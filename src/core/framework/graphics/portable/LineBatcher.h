//
//  LineBatcher.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/15/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__LineBatcher__
#define __noctisgames__LineBatcher__

class RendererHelper;
class GpuProgramWrapper;
class Line;
class Color;

class LineBatcher
{
public:
    LineBatcher(RendererHelper* inRendererHelper);
    
    ~LineBatcher();
    
    void beginBatch();
    
    void endBatch(GpuProgramWrapper &gpuProgramWrapper);
    
    void renderLine(Line &line, Color &c);
    
private:
    RendererHelper* _rendererHelper;
    int _numLines;
};

#endif /* defined(__noctisgames__LineBatcher__) */
