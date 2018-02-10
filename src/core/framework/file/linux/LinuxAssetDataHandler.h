//
//  LinuxAssetDataHandler.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__LinuxAssetDataHandler__
#define __noctisgames__LinuxAssetDataHandler__

#include <framework/file/portable/AssetDataHandler.h>

class LinuxAssetDataHandler : public AssetDataHandler
{
public:
	static LinuxAssetDataHandler* getInstance();

    virtual FileData getAssetData(const char* relativePath);
    virtual void releaseAssetData(const FileData* fileData);

private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    LinuxAssetDataHandler();
    LinuxAssetDataHandler(const LinuxAssetDataHandler&);
    LinuxAssetDataHandler& operator=(const LinuxAssetDataHandler&);
};

#endif /* defined(__noctisgames__LinuxAssetDataHandler__) */
