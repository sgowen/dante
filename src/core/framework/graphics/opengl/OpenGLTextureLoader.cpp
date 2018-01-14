//
//  OpenGLTextureLoader.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/graphics/opengl/OpenGLTextureLoader.h"

#include "framework/graphics/portable/TextureDataWrapper.h"
#include <framework/graphics/portable/NGTexture.h>
#include "framework/graphics/portable/TextureWrapper.h"

#include "framework/file/portable/AssetDataHandler.h"
#include "framework/file/portable/FileData.h"
#include "framework/util/StringUtil.h"

extern "C"
{
#include "png.h"
}

#include <string>
#include <assert.h>
#include <stdlib.h>
#include <cstring>

OpenGLPngImageData* getOpenGLPngImageDataFromFileData(const void* png_data, const int png_data_size);
void releaseOpenGLPngImageData(const OpenGLPngImageData* data);
GLuint loadPngAssetIntoTexture(OpenGLPngImageData* OpenGLPngImageData, bool repeatS = false);
GLuint createTexture(const GLsizei width, const GLsizei height, const GLenum type, const GLvoid* pixels, bool repeatS = false, bool mipmap = false);

OpenGLTextureLoader::OpenGLTextureLoader() : TextureLoader()
{
    // Empty
}

TextureDataWrapper* OpenGLTextureLoader::loadTextureData(NGTexture* texture)
{
    const char* textureName = texture->name.c_str();

    const char* finalTextureFileName;
#if defined __linux__ && !defined(__ANDROID__)
    std::string s1("assets/textures/");
    s1 += std::string(textureName);
    finalTextureFileName = s1.c_str();
#else
    finalTextureFileName = textureName;
#endif

    const FileData fileData = AssetDataHandler::getAssetDataHandler()->getAssetData(finalTextureFileName);
    void* output = NULL;
    if (texture->_isEncrypted)
    {
        output = malloc(fileData.data_length);
        StringUtil::encryptDecrypt((unsigned char*)fileData.data, (unsigned char*) output, fileData.data_length);
    }
    else
    {
        output = (void*) fileData.data;
    }

    OpenGLPngImageData* rawImageData = getOpenGLPngImageDataFromFileData(output, (int)fileData.data_length);

    AssetDataHandler::getAssetDataHandler()->releaseAssetData(&fileData);

    TextureDataWrapper* tdw = new TextureDataWrapper(rawImageData);

    if (texture->_isEncrypted)
    {
        free(output);
    }

    return tdw;
}

TextureWrapper* OpenGLTextureLoader::loadTexture(TextureDataWrapper* textureData, bool repeatS)
{
    TextureWrapper* ret = new TextureWrapper(loadPngAssetIntoTexture(textureData->_rawImageData, repeatS));
    
    delete textureData->_rawImageData;
    textureData->_rawImageData = NULL;
    
    return ret;
}

void OpenGLTextureLoader::destroyTexture(TextureWrapper& textureWrapper)
{
    glDeleteTextures(1, &textureWrapper.texture);
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

OpenGLPngImageData* getOpenGLPngImageDataFromFileData(const void* png_data, const int png_data_size)
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

    return new OpenGLPngImageData(static_cast<int>(png_info.width), static_cast<int>(png_info.height), (int) raw_image.size, getGlColorFormat(png_info.color_type), raw_image.data);
}

void releaseOpenGLPngImageData(const OpenGLPngImageData* data)
{
    assert(data != NULL);
    
    free((void*)data->_data);
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

GLuint loadPngAssetIntoTexture(OpenGLPngImageData* inOpenGLPngImageData, bool repeatS)
{
    const GLuint texture_object_id = createTexture(inOpenGLPngImageData->_width, inOpenGLPngImageData->_height, inOpenGLPngImageData->_glColorFormat, inOpenGLPngImageData->_data, repeatS, true);

    releaseOpenGLPngImageData(inOpenGLPngImageData);

    return texture_object_id;
}

GLuint createTexture(const GLsizei width, const GLsizei height, const GLenum type, const GLvoid* pixels, bool repeat_s, bool mipmap)
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
