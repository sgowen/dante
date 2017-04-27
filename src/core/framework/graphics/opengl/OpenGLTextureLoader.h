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

#include "ITextureLoader.h"

#include "PngImageData.h"

class OpenGLTextureLoader : public ITextureLoader
{
public:
    OpenGLTextureLoader();
    
    virtual ~OpenGLTextureLoader();
    
    virtual GpuTextureDataWrapper* loadTextureData(const char* textureName);
    
    virtual GpuTextureWrapper* loadTexture(GpuTextureDataWrapper* textureData, bool repeatS = false);
    
private:
    PngImageData getPngImageDataFromFileData(const void* png_data, const int png_data_size);
    
    void releasePngImageData(const PngImageData* data);
    
    GLuint loadPngAssetIntoTexture(PngImageData PngImageData, bool repeatS = false);
    GLuint createTexture(const GLsizei width, const GLsizei height, const GLenum type, const GLvoid* pixels, bool repeatS = false, bool mipmap = false);
};

#endif /* defined(__noctisgames__OpenGLTextureLoader__) */
