//
//  TextureManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/27/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/TextureManager.h"

#include "framework/graphics/portable/TextureLoader.h"
#include "framework/graphics/portable/NGTexture.h"

#include "framework/graphics/portable/TextureWrapper.h"
#include "framework/graphics/portable/TextureDataWrapper.h"
#include "framework/graphics/portable/TextureLoaderFactory.h"
#include "framework/util/NGSTDUtil.h"
#include "framework/graphics/portable/NGTextureDesc.h"
#include "framework/graphics/portable/Assets.h"

#include <string>
#include <assert.h>

TextureManager::TextureManager(const char* assetsCfgPath) :
_assetsCfgPath(assetsCfgPath),
_textureLoader(TEXTURE_LOADER_FACTORY->createTextureLoader())
{
    // Empty
}

TextureManager::~TextureManager()
{
    releaseDeviceDependentResources();
    
    delete _textureLoader;
    
    NGSTDUtil::cleanUpMapOfPointers(_textures);
}

void TextureManager::createDeviceDependentResources()
{
    ASSETS->initWithJsonFile(_assetsCfgPath);
    
    if (_textures.size() == 0)
    {
        std::vector<NGTextureDesc*>& textureDescs = ASSETS->getTextureDescriptors();
        for (NGTextureDesc* td : textureDescs)
        {
            _textures[td->_textureName] = new NGTexture(td->_textureName, this, td->_repeatS, td->_isEncrypted);
            
            if (td->_hasNormal)
            {
                std::string normalMapName = td->_textureName;
                std::string prefix = "n_";
                normalMapName.insert(0, prefix);
                
                _textures[normalMapName] = new NGTexture(normalMapName, this, td->_repeatS, td->_isEncrypted);
            }
        }
    }
    
    for (std::map<std::string, NGTexture*>::iterator i = _textures.begin(); i != _textures.end(); ++i)
    {
        loadTextureAsync(i->second);
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
    assert(texture->name.length() > 0);
    
    if (texture->textureWrapper
        || texture->_isLoadingData)
    {
        return;
    }
    
    texture->_isLoadingData = true;
    texture->textureDataWrapper = texture->_textureManager->_textureLoader->loadTextureData(texture);
}

void TextureManager::loadTextureSync(NGTexture* texture)
{
    loadTextureDataSync(texture);
    
    texture->textureWrapper = _textureLoader->loadTexture(texture->textureDataWrapper, texture->_repeatS);
    
    delete texture->textureDataWrapper;
    texture->textureDataWrapper = NULL;
    
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
    assert(texture->name.length() > 0);
    
    if (texture->textureWrapper
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
    
    if (texture->textureWrapper)
    {
        _textureLoader->destroyTexture(*texture->textureWrapper);
        
        delete texture->textureWrapper;
        texture->textureWrapper = NULL;
    }
    
    if (texture->textureDataWrapper)
    {
        delete texture->textureDataWrapper;
        texture->textureDataWrapper = NULL;
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
    if (texture->textureWrapper == NULL)
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
        if ((*i)->textureDataWrapper)
        {
            (*i)->textureWrapper = _textureLoader->loadTexture((*i)->textureDataWrapper, (*i)->_repeatS);
            
            delete (*i)->textureDataWrapper;
            (*i)->textureDataWrapper = NULL;
            
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
    auto q = _textures.find(name);
    
    assert(q != _textures.end());
    
    NGTexture* ret = q->second;
    
    return ret;
}