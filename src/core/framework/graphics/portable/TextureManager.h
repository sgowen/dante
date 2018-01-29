//
//  TextureManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/27/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__TextureManager__
#define __noctisgames__TextureManager__

#include <tinythread.h>

#include <map>
#include <vector>
#include <string>

class TextureLoader;
class NGTexture;

class TextureManager
{
public:
    TextureManager();
    ~TextureManager();
    
    void createDeviceDependentResources();
    void releaseDeviceDependentResources();
    void loadTextureDataSync(NGTexture* arg);
    void loadTextureSync(NGTexture* texture);
    void loadTextureAsync(NGTexture* texture);
    void unloadTexture(NGTexture* texture);
    bool ensureTextures();
    bool ensureTexture(NGTexture* texture);
    void handleAsyncTextureLoads();
    void cleanUpThreads();
    bool isLoadingData();
    NGTexture* getTextureWithName(std::string name);
    
private:
    std::map<std::string, NGTexture*> _textures;
    std::vector<NGTexture*> _loadingTextures;
    std::vector<tthread::thread*> _textureDataLoadingThreads;

    TextureLoader* _textureLoader;
    
    friend void tthreadLoadTextureDataSync(void* arg);
};

#endif /* defined(__noctisgames__TextureManager__) */
