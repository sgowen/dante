//
//  AssetDataHandler.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/2/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__AssetDataHandler__
#define __noctisgames__AssetDataHandler__

#include "FileData.h"

class AssetDataHandler
{
public:
    static AssetDataHandler* getAssetDataHandler();
    
    virtual FileData getAssetData(const char* relativePath) = 0;
    
    virtual void releaseAssetData(const FileData* fileData) = 0;
    
protected:
    AssetDataHandler();
    
private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    AssetDataHandler(const AssetDataHandler&);
    AssetDataHandler& operator=(const AssetDataHandler&);
};

#endif /* defined(__noctisgames__AssetDataHandler__) */
