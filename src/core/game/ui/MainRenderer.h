//
//  MainRenderer.h
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __dante__MainRenderer__
#define __dante__MainRenderer__

#include "Renderer.h"

#include <string>

class TextureWrapper;
class Font;
class Vector3;

class MainRenderer : public Renderer
{
public:
    MainRenderer(int maxBatchSize);
    
    virtual ~MainRenderer();
    
    virtual void createDeviceDependentResources();
    
    virtual void releaseDeviceDependentResources();
    
    void tempDraw(float stateTime);
    
private:
    TextureWrapper* m_demo;
    TextureWrapper* m_misc;
    Font* m_font;
    
    void RenderBandWidth();
    void RenderRoundTripTime();
    void RenderText(const std::string& inStr, const Vector3& origin, const Vector3& inColor);
};

#endif /* defined(__dante__MainRenderer__) */
