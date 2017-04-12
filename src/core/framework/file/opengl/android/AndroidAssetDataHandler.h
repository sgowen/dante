//
//  AndroidAssetDataHandler.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__AndroidAssetDataHandler__
#define __noctisgames__AndroidAssetDataHandler__

#define ANDROID_ASSETS (AndroidAssetDataHandler::getInstance())

#include "AssetDataHandler.h"

#include <android/asset_manager_jni.h>

class AndroidAssetDataHandler : public AssetDataHandler
{
public:
	static AndroidAssetDataHandler* getInstance();
    
    void init(JNIEnv* jni, jobject activity);
    
    virtual FileData getAssetData(const char* relativePath);
    
    virtual void releaseAssetData(const FileData* fileData);

private:
    AAssetManager* mAssetManager;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    AndroidAssetDataHandler();
    AndroidAssetDataHandler(const AndroidAssetDataHandler&);
    AndroidAssetDataHandler& operator=(const AndroidAssetDataHandler&);
};

#endif /* defined(__noctisgames__AndroidAssetDataHandler__) */
