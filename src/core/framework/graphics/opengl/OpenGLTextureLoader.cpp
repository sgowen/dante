//
//  OpenGLTextureLoader.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "OpenGLTextureLoader.h"

#include "GpuTextureDataWrapper.h"
#include "GpuTextureWrapper.h"
#include "AssetDataHandler.h"
#include "FileData.h"
#include "PngImageData.h"
#include "StringUtil.h"

extern "C"
{
#include "png.h"
}

#include <string>
#include <assert.h>
#include <stdlib.h>

OpenGLTextureLoader::OpenGLTextureLoader() : ITextureLoader()
{
    // Empty
}

OpenGLTextureLoader::~OpenGLTextureLoader()
{
    // Empty
}

GpuTextureDataWrapper* OpenGLTextureLoader::loadTextureData(const char* textureName)
{
    size_t len = strlen(textureName);
    
    char* textureFileName = new char[len + 5];
    
    strcpy(textureFileName, textureName);
    textureFileName[len] = '.';
    textureFileName[len+1] = 'n';
    textureFileName[len+2] = 'g';
    textureFileName[len+3] = 't';
    textureFileName[len+4] = '\0';
    
    const FileData png_file = AssetDataHandler::getAssetDataHandler()->getAssetData(textureFileName);
    unsigned char* output = (unsigned char*) malloc(png_file.data_length);
    StringUtil::encryptDecrypt((unsigned char*)png_file.data, output, png_file.data_length);
    
    const PngImageData raw_image_data = getPngImageDataFromFileData(output, (int)png_file.data_length);
    
    AssetDataHandler::getAssetDataHandler()->releaseAssetData(&png_file);
    
    GpuTextureDataWrapper* tdw = new GpuTextureDataWrapper(raw_image_data);
    
    delete[] textureFileName;
    
    free((void *)output);
    
    return tdw;
}

GpuTextureWrapper* OpenGLTextureLoader::loadTexture(GpuTextureDataWrapper* textureData, bool repeatS)
{
    return new GpuTextureWrapper(loadPngAssetIntoTexture(textureData->raw_image_data, repeatS));
}

struct DataHandle
{
    const png_byte* data;
    const png_size_t size;
};

struct ReadDataHandle
{
    const DataHandle data;
    png_size_t offset;
};

struct PngInfo
{
    const png_uint_32 width;
    const png_uint_32 height;
    const int color_type;
};

static void readPngDataCallback(png_structp png_ptr, png_byte* png_data, png_size_t read_length);

static PngInfo readAndUpdateInfo(const png_structp png_ptr, const png_infop info_ptr);

static DataHandle readEntirePngImage(const png_structp png_ptr, const png_infop info_ptr, const png_uint_32 height);

static GLenum getGlColorFormat(const int png_color_format);

PngImageData OpenGLTextureLoader::getPngImageDataFromFileData(const void* png_data, const int png_data_size)
{
    assert(png_data != NULL && png_data_size > 8);
    assert(png_check_sig((png_const_bytep)png_data, 8));
    
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    assert(png_ptr != NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    assert(info_ptr != NULL);
    
    ReadDataHandle png_data_handle = (ReadDataHandle)
    {
        {
            (png_byte*) png_data, static_cast<png_size_t>(png_data_size)
        }, 0
    };
    png_set_read_fn(png_ptr, &png_data_handle, readPngDataCallback);
    
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        // Error reading PNG file!
    }
    
    const PngInfo png_info = readAndUpdateInfo(png_ptr, info_ptr);
    const DataHandle raw_image = readEntirePngImage(png_ptr, info_ptr, png_info.height);
    
    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    
    return (PngImageData)
    {
        static_cast<int>(png_info.width),
        static_cast<int>(png_info.height),
        (int) raw_image.size,
        getGlColorFormat(png_info.color_type),
        raw_image.data
    };
}

void OpenGLTextureLoader::releasePngImageData(const PngImageData* data)
{
    assert(data != NULL);
    free((void*)data->data);
}

static void readPngDataCallback(png_structp png_ptr, png_byte* raw_data, png_size_t read_length)
{
    ReadDataHandle* handle = (ReadDataHandle*) png_get_io_ptr(png_ptr);
    const png_byte* png_src = handle->data.data + handle->offset;
    
    memcpy(raw_data, png_src, read_length);
    handle->offset += read_length;
}

static PngInfo readAndUpdateInfo(const png_structp png_ptr, const png_infop info_ptr)
{
    png_uint_32 width, height;
    int bit_depth, color_type;
    
    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
    
    // Convert transparency to full alpha
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    {
        png_set_tRNS_to_alpha(png_ptr);
    }
    
    // Convert grayscale, if needed.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    {
        png_set_expand_gray_1_2_4_to_8(png_ptr);
    }
    
    // Convert paletted images, if needed.
    if (color_type == PNG_COLOR_TYPE_PALETTE)
    {
        png_set_palette_to_rgb(png_ptr);
    }
    
    // Add alpha channel, if there is none (rationale: GL_RGBA is faster than GL_RGB on many GPUs)
    if (color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_RGB)
    {
        png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);
    }
    
    // Ensure 8-bit packing
    if (bit_depth < 8)
    {
        png_set_packing(png_ptr);
    }
    else if (bit_depth == 16)
    {
        png_set_scale_16(png_ptr);
    }
    
    png_read_update_info(png_ptr, info_ptr);
    
    // Read the new color type after updates have been made.
    color_type = png_get_color_type(png_ptr, info_ptr);
    
    return (PngInfo)
    {
        width, height, color_type
    };
}

static DataHandle readEntirePngImage(const png_structp png_ptr, const png_infop info_ptr, const png_uint_32 height)
{
    const png_size_t row_size = png_get_rowbytes(png_ptr, info_ptr);
    const int data_length = (int) row_size * height;
    assert(row_size > 0);
    
    png_byte* raw_image = (png_byte*) malloc(data_length);
    assert(raw_image != NULL);
    
    png_byte *row_ptrs[height];
    
    png_uint_32 i;
    for (i = 0; i < height; ++i)
    {
        row_ptrs[i] = raw_image + i * row_size;
    }
    
    png_read_image(png_ptr, &row_ptrs[0]);
    
    return (DataHandle)
    {
        raw_image, static_cast<png_size_t>(data_length)
    };
}

static GLenum getGlColorFormat(const int png_color_format)
{
    assert(png_color_format == PNG_COLOR_TYPE_GRAY || png_color_format == PNG_COLOR_TYPE_RGB_ALPHA || png_color_format == PNG_COLOR_TYPE_GRAY_ALPHA);
    
    switch (png_color_format)
    {
        case PNG_COLOR_TYPE_GRAY:
            return GL_LUMINANCE;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            return GL_RGBA;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            return GL_LUMINANCE_ALPHA;
    }
    
    return 0;
}

GLuint OpenGLTextureLoader::loadPngAssetIntoTexture(PngImageData PngImageData, bool repeatS)
{
    const GLuint texture_object_id = createTexture(PngImageData.width, PngImageData.height, PngImageData.gl_color_format, PngImageData.data, repeatS, 0);
    
    releasePngImageData(&PngImageData);
    
    return texture_object_id;
}

GLuint OpenGLTextureLoader::createTexture(const GLsizei width, const GLsizei height, const GLenum type, const GLvoid* pixels, bool repeat_s, bool mipmap)
{
    GLuint texture_object_id;
    glGenTextures(1, &texture_object_id);
    assert(texture_object_id != 0);
    
    glBindTexture(GL_TEXTURE_2D, texture_object_id);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Wrap texture at left/right edges
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat_s ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, pixels);
    
    if (mipmap)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return texture_object_id;
}
