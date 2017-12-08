//
//  OpenGLTextureLoader.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLTextureLoader__
#define __noctisgames__OpenGLTextureLoader__

#include "pch.h"

#include "framework/graphics/portable/TextureLoader.h"

#include "framework/graphics/opengl/OpenGLPngImageData.h"

class OpenGLTextureLoader : public TextureLoader
{
public:
    OpenGLTextureLoader();
    
    virtual ~OpenGLTextureLoader();
    
    virtual GpuTextureDataWrapper* loadTextureData(TextureWrapper* textureWrapper);
    
    virtual GpuTextureWrapper* loadTexture(GpuTextureDataWrapper* textureData, bool repeatS = false);
    
private:
    OpenGLPngImageData getOpenGLPngImageDataFromFileData(const void* png_data, const int png_data_size);
    
    void releaseOpenGLPngImageData(const OpenGLPngImageData* data);
    
    GLuint loadPngAssetIntoTexture(OpenGLPngImageData OpenGLPngImageData, bool repeatS = false);
    GLuint createTexture(const GLsizei width, const GLsizei height, const GLenum type, const GLvoid* pixels, bool repeatS = false, bool mipmap = false);
};

#endif /* defined(__noctisgames__OpenGLTextureLoader__) */
