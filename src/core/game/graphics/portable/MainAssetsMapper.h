//
//  MainAssetsMapper.h
//  dante
//
//  Created by Stephen Gowen on 11/9/16.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __dante__MainAssetsMapper__
#define __dante__MainAssetsMapper__

#include "framework/graphics/portable/AssetsMapper.h"

class MainAssetsMapper : public AssetsMapper
{
public:
    MainAssetsMapper();
    
    virtual ~MainAssetsMapper();
    
    virtual void mapAssets();
};

#endif /* defined(__dante__MainAssetsMapper__) */
