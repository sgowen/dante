//
//  TextureManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/27/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/graphics/portable/TextureManager.h>

#include <framework/graphics/portable/TextureLoader.h>
#include <framework/graphics/portable/NGTexture.h>
#include <framework/graphics/portable/FramebufferWrapper.h>

#include <framework/graphics/portable/TextureWrapper.h>
#include <framework/graphics/portable/TextureDataWrapper.h>
#include <framework/graphics/portable/TextureLoaderFactory.h>
#include <framework/util/NGSTDUtil.h>
#include <framework/graphics/portable/NGTextureDesc.h>
#include <framework/file/portable/Assets.h>

#include <string>
#include <assert.h>

TextureManager::TextureManager() :
_textureLoader(TEXTURE_LOADER_FACTORY->createTextureLoader())
{
    // Empty
}

TextureManager::~TextureManager()
{
    releaseDeviceDependentResources();
    
    delete _textureLoader;
    
    NGSTDUtil::cleanUpMapOfPointers(_textures);
    _framebuffers.clear();
}

void TextureManager::createDeviceDependentResources()
{
    if (_textures.size() == 0)
    {
        std::vector<NGTextureDesc*>& textureDescs = ASSETS->getTextureDescriptors();
        for (NGTextureDesc* td : textureDescs)
        {
            _textures[td->_textureName] = new NGTexture(td->_textureName, td, this);
            
            if (td->_hasNormal)
            {
                std::string normalMapName = td->_textureName;
                std::string prefix = "n_";
                normalMapName.insert(0, prefix);
                
                _textures[normalMapName] = new NGTexture(normalMapName, td, this);
            }
        }
    }
    
    for (std::map<std::string, NGTexture*>::iterator i = _textures.begin(); i != _textures.end(); ++i)
    {
        loadTextureSync(i->second);
    }
}

void TextureManager::releaseDeviceDependentResources()
{
    _loadingTextures.clear();

	cleanUpThreads();
    
    for (std::map<std::string, NGTexture*>::iterator i = _textures.begin(); i != _textures.end(); ++i)
    {
        unloadTexture(i->second);
    }
    
    _textures.clear();
}

void TextureManager::loadTextureDataSync(NGTexture* arg)
{
    assert(arg != NULL);
    
    NGTexture* texture = static_cast<NGTexture*>(arg);
    
    assert(texture->_textureManager != NULL);
    assert(texture->_name.length() > 0);
    
    if (texture->_textureWrapper
        || texture->_isLoadingData)
    {
        return;
    }
    
    texture->_isLoadingData = true;
    texture->_textureDataWrapper = texture->_textureManager->_textureLoader->loadTextureData(texture);
}

void TextureManager::loadTextureSync(NGTexture* texture)
{
    loadTextureDataSync(texture);
    
    texture->_textureWrapper = _textureLoader->loadTexture(texture->_textureDataWrapper, texture->_desc);
    
    delete texture->_textureDataWrapper;
    texture->_textureDataWrapper = NULL;
    
    texture->_isLoadingData = false;
}

void tthreadLoadTextureDataSync(void* arg)
{
    assert(arg != NULL);
    
    NGTexture* texture = static_cast<NGTexture*>(arg);
    
    texture->_textureManager->loadTextureDataSync(texture);
}

void TextureManager::loadTextureAsync(NGTexture* texture)
{
    assert(texture != NULL);
    assert(texture->_name.length() > 0);
    
    if (texture->_textureWrapper
        || texture->_isLoadingData)
    {
        return;
    }
    
    _loadingTextures.push_back(texture);
    
    _textureDataLoadingThreads.push_back(new tthread::thread(tthreadLoadTextureDataSync, texture));
}

void TextureManager::unloadTexture(NGTexture* texture)
{
    if (texture == NULL)
    {
        return;
    }
    
    for (std::vector<NGTexture *>::iterator i = _loadingTextures.begin(); i != _loadingTextures.end(); )
    {
        if ((*i) == texture)
        {
            i = _loadingTextures.erase(i);
        }
        else
        {
            ++i;
        }
    }
    
    if (texture->_textureWrapper)
    {
        _textureLoader->destroyTexture(*texture->_textureWrapper);
        
        delete texture->_textureWrapper;
        texture->_textureWrapper = NULL;
    }
    
    if (texture->_textureDataWrapper)
    {
        delete texture->_textureDataWrapper;
        texture->_textureDataWrapper = NULL;
    }
    
    texture->_isLoadingData = false;
}

bool TextureManager::ensureTextures()
{
    for (std::map<std::string, NGTexture*>::iterator i = _textures.begin(); i != _textures.end(); ++i)
    {
        if (!ensureTexture(i->second))
        {
            return false;
        }
    }
    
    return true;
}

bool TextureManager::ensureTexture(NGTexture* texture)
{
    if (texture->_textureWrapper == NULL)
    {
        if (!texture->_isLoadingData)
        {
            loadTextureAsync(texture);
        }
        
        return false;
    }
    
    return true;
}

void TextureManager::handleAsyncTextureLoads()
{
    for (std::vector<NGTexture *>::iterator i = _loadingTextures.begin(); i != _loadingTextures.end(); )
    {
        if ((*i)->_textureDataWrapper)
        {
            (*i)->_textureWrapper = _textureLoader->loadTexture((*i)->_textureDataWrapper, (*i)->_desc);
            
            delete (*i)->_textureDataWrapper;
            (*i)->_textureDataWrapper = NULL;
            
            (*i)->_isLoadingData = false;
            
            i = _loadingTextures.erase(i);
        }
        else
        {
            ++i;
        }
    }
    
    if (_loadingTextures.size() == 0)
    {
        cleanUpThreads();
    }
}

void TextureManager::cleanUpThreads()
{
    for (std::vector<tthread::thread *>::iterator i = _textureDataLoadingThreads.begin(); i != _textureDataLoadingThreads.end(); ++i)
    {
        (*i)->join();
    }
    
    NGSTDUtil::cleanUpVectorOfPointers(_textureDataLoadingThreads);
}

bool TextureManager::isLoadingData()
{
    return _loadingTextures.size() > 0;
}

NGTexture* TextureManager::getTextureWithName(std::string name)
{
    NGTexture* ret = NULL;
    
    {
        auto q = _textures.find(name);
        
        if (q != _textures.end())
        {
            ret = q->second;
        }
    }
    
    if (ret == NULL)
    {
        auto q = _framebuffers.find(name);
        
        if (q != _framebuffers.end())
        {
            ret = q->second->texture;
        }
    }
    
    return ret;
}

void TextureManager::registerFramebuffer(std::string name, FramebufferWrapper* fb)
{
    _framebuffers[name] = fb;
}
