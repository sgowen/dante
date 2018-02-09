//
//  OpenGLProgramLoader.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/2/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/graphics/opengl/OpenGLProgramLoader.h"

#include "framework/graphics/portable/ShaderProgramWrapper.h"
#include "framework/graphics/portable/NGShaderVarInput.h"
#include "framework/graphics/portable/NGShaderUniformInput.h"

#include "framework/file/portable/AssetDataHandler.h"
#include "framework/file/portable/FileData.h"
#include "framework/util/StringUtil.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>

OpenGLProgramLoader::OpenGLProgramLoader() : ShaderProgramLoader()
{
    // Empty
}

OpenGLProgramLoader::~OpenGLProgramLoader()
{
    // Empty
}

ShaderProgramWrapper* OpenGLProgramLoader::loadNGShader(std::string& vertexShaderFilePath, std::string& fragmentShaderFilePath, std::vector<NGShaderUniformInput*>& uniforms, std::vector<NGShaderVarInput*>& inputLayout)
{
    const FileData vertex_shader_source = AssetDataHandler::getAssetDataHandler()->getAssetData(vertexShaderFilePath.c_str());
    const FileData fragment_shader_source = AssetDataHandler::getAssetDataHandler()->getAssetData(fragmentShaderFilePath.c_str());

    unsigned char* vertex_shader_source_output = (unsigned char*) malloc(vertex_shader_source.data_length);
    StringUtil::encryptDecrypt((unsigned char*)vertex_shader_source.data, vertex_shader_source_output, vertex_shader_source.data_length);

    unsigned char* fragment_shader_source_output = (unsigned char*) malloc(fragment_shader_source.data_length);
    StringUtil::encryptDecrypt((unsigned char*)fragment_shader_source.data, fragment_shader_source_output, fragment_shader_source.data_length);

    GLuint programObjectId = buildProgram(vertex_shader_source_output, (GLint)vertex_shader_source.data_length, fragment_shader_source_output, (GLint)fragment_shader_source.data_length);

    AssetDataHandler::getAssetDataHandler()->releaseAssetData(&vertex_shader_source);
    AssetDataHandler::getAssetDataHandler()->releaseAssetData(&fragment_shader_source);

    free((void *)vertex_shader_source_output);
    free((void *)fragment_shader_source_output);
    
    ShaderProgramWrapper* ret = new ShaderProgramWrapper(programObjectId, uniforms, inputLayout);
    
    int totalSize = 0;
    for (std::vector<NGShaderVarInput*>::iterator i = inputLayout.begin(); i != inputLayout.end(); ++i)
    {
        totalSize += (*i)->_size;
    }
    
    for (std::vector<NGShaderVarInput*>::iterator i = inputLayout.begin(); i != inputLayout.end(); ++i)
    {
        (*i)->build(ret, totalSize);
    }
    
    for (std::vector<NGShaderUniformInput*>::iterator i = uniforms.begin(); i != uniforms.end(); ++i)
    {
        (*i)->build(ret);
    }
    
    return ret;
}

void OpenGLProgramLoader::destroyNGShader(ShaderProgramWrapper* shaderProgramWrapper, std::vector<NGShaderUniformInput*>& uniforms, std::vector<NGShaderVarInput*>& inputLayout)
{
    glDeleteProgram(shaderProgramWrapper->_programObjectId);
}

GLuint OpenGLProgramLoader::buildProgram(const void * vertex_shader_source, const int vertex_shader_source_length, const void * fragment_shader_source, const int fragment_shader_source_length)
{
    assert(vertex_shader_source != NULL);
    assert(fragment_shader_source != NULL);

    GLuint vertex_shader = compileShader(GL_VERTEX_SHADER, vertex_shader_source, vertex_shader_source_length);
    GLuint fragment_shader = compileShader(GL_FRAGMENT_SHADER, fragment_shader_source, fragment_shader_source_length);

    return linkProgram(vertex_shader, fragment_shader);
}

GLuint OpenGLProgramLoader::compileShader(const GLenum type, const void* source, const GLint length)
{
    assert(source != NULL);

    GLuint shader_object_id = glCreateShader(type);
    GLint compile_status;

    assert(shader_object_id != GL_FALSE);

    glShaderSource(shader_object_id, 1, (const GLchar **)&source, &length);
    glCompileShader(shader_object_id);
    glGetShaderiv(shader_object_id, GL_COMPILE_STATUS, &compile_status);

    if (compile_status == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader_object_id, GL_INFO_LOG_LENGTH, &maxLength);

        GLchar* errorLog = (GLchar*) malloc(maxLength);
        glGetShaderInfoLog(shader_object_id, maxLength, &maxLength, errorLog);
        LOG("%s", errorLog);

        glDeleteShader(shader_object_id);

        free(errorLog);
    }

    assert(compile_status != GL_FALSE);

    return shader_object_id;
}

GLuint OpenGLProgramLoader::linkProgram(const GLuint vertex_shader, const GLuint fragment_shader)
{
    GLuint program_object_id = glCreateProgram();
    GLint link_status;

    assert(program_object_id != GL_FALSE);

    glAttachShader(program_object_id, vertex_shader);
    glAttachShader(program_object_id, fragment_shader);
    glLinkProgram(program_object_id);
    glGetProgramiv(program_object_id, GL_LINK_STATUS, &link_status);

    assert(link_status != GL_FALSE);

    // Release vertex and fragment shaders.
    glDetachShader(program_object_id, vertex_shader);
    glDeleteShader(vertex_shader);
    glDetachShader(program_object_id, fragment_shader);
    glDeleteShader(fragment_shader);

    return program_object_id;
}
