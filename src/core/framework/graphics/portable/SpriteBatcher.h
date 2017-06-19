//
//  SpriteBatcher.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/25/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SpriteBatcher__
#define __noctisgames__SpriteBatcher__

struct GpuTextureWrapper;
class GpuProgramWrapper;
class TextureRegion;
class Color;

class SpriteBatcher
{
public:
    SpriteBatcher();
    
    virtual ~SpriteBatcher();

    virtual void beginBatch() = 0;

    virtual void endBatch(GpuTextureWrapper& textureWrapper, GpuProgramWrapper &gpuProgramWrapper) = 0;

    void drawSprite(float x, float y, float width, float height, float angle, TextureRegion& tr);

    void drawSprite(float x, float y, float width, float height, float angle, Color &c, TextureRegion& tr);
    
    void renderSprite(float x, float y, float width, float height, float angle, bool flipX, TextureRegion& tr);
    
    void renderSprite(float x, float y, float width, float height, float angle, bool flipX, Color &c, TextureRegion& tr);

protected:
    int m_iNumSprites;
    
    virtual void addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a, float u, float v) = 0;
    
private:
    void renderSprite(float x, float y, float width, float height, bool flipX, TextureRegion& tr);
    
    void renderSprite(float x, float y, float width, float height, bool flipX, Color &c, TextureRegion& tr);
};

#endif /* defined(__noctisgames__SpriteBatcher__) */
