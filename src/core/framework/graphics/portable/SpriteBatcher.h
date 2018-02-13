//
//  SpriteBatcher.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/25/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SpriteBatcher__
#define __noctisgames__SpriteBatcher__

#include <framework/graphics/portable/VertexProgramInput.h>
#include <framework/graphics/portable/Color.h>

#include <vector>
#include <cstddef>

enum SpriteBatcherState
{
    SpriteBatcherState_Static =                1 << 0,
    SpriteBatcherState_StaticBatchRenderered = 1 << 1
};

class RendererHelper;
class NGTexture;
class NGShader;
class TextureRegion;

class SpriteBatcher
{
public:
    SpriteBatcher(RendererHelper* inRendererHelper);
    ~SpriteBatcher();

    void beginBatch(int index);
    void renderSprite(float x, float y, float width, float height, float angle, TextureRegion& tr, bool flipX = false);
    void endBatch(NGShader* shader, NGTexture* texture, NGTexture* normalMap = NULL, Color& c = Color::WHITE);

    void useDynamicConfig();
    void useStaticConfig();
    bool isDynamic();
    bool isStaticBatchRendered();
    
private:
    RendererHelper* _rendererHelper;
    std::vector<VERTEX_2D_TEXTURE> _vertices;
    int _numSprites;
    int _bufferIndex;
    int _state;
};

#endif /* defined(__noctisgames__SpriteBatcher__) */
