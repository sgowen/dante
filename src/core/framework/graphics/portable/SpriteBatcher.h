//
//  SpriteBatcher.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/25/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SpriteBatcher__
#define __noctisgames__SpriteBatcher__

class RendererHelper;
class TextureWrapper;
class GpuProgramWrapper;
class TextureRegion;
class Color;

class SpriteBatcher
{
public:
    SpriteBatcher(RendererHelper* inRendererHelper);
    
    ~SpriteBatcher();

    void beginBatch();

    void endBatch(TextureWrapper* textureWrapper, GpuProgramWrapper& gpuProgramWrapper);

    void renderSprite(float x, float y, float width, float height, float angle, TextureRegion& tr, bool flipX = false);
    void renderSprite(float x, float y, float width, float height, float angle, Color &c, TextureRegion& tr, bool flipX = false);

private:
    RendererHelper* _rendererHelper;
    int _numSprites;
};

#endif /* defined(__noctisgames__SpriteBatcher__) */
