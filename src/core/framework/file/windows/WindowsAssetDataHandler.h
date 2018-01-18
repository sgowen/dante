//
//  WindowsAssetDataHandler.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__WindowsAssetDataHandler__
#define __noctisgames__WindowsAssetDataHandler__

#include "framework/file/portable/AssetDataHandler.h"

class WindowsAssetDataHandler : public AssetDataHandler
{
public:
	static WindowsAssetDataHandler* getInstance();
    
    virtual FileData getAssetData(const char* relativePath);
    virtual void releaseAssetData(const FileData* fileData);

private:    
    // ctor, copy ctor, and assignment should be private in a Singleton
    WindowsAssetDataHandler();
    WindowsAssetDataHandler(const WindowsAssetDataHandler&);
    WindowsAssetDataHandler& operator=(const WindowsAssetDataHandler&);
};

#endif /* defined(__noctisgames__WindowsAssetDataHandler__) */
