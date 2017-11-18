//
//  AppleAssetDataHandler.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__AppleAssetDataHandler__
#define __noctisgames__AppleAssetDataHandler__

#include "AssetDataHandler.h"

class AppleAssetDataHandler : public AssetDataHandler
{
public:
	static AppleAssetDataHandler* getInstance();
    
    virtual FileData getAssetData(const char* relativePath);
    
    virtual void releaseAssetData(const FileData* fileData);

private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    AppleAssetDataHandler();
    AppleAssetDataHandler(const AppleAssetDataHandler&);
    AppleAssetDataHandler& operator=(const AppleAssetDataHandler&);
};

#endif /* defined(__noctisgames__AppleAssetDataHandler__) */
