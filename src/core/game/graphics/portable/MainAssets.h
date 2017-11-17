//
//  MainAssets.h
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __dante__MainAssets__
#define __dante__MainAssets__

#define MAIN_ASSETS (MainAssets::getInstance())

class TextureRegion;

class MainAssets
{
public:
	static MainAssets* getInstance();
    
    bool isUsingCompressedTextureSet();
    
    void setUsingCompressedTextureSet(bool isUsingCompressedTextureSet);
    
    bool isUsingDesktopTextureSet();
    
    void setUsingDesktopTextureSet(bool isUsingDesktopTextureSet);
    
    bool isUsingGamePadTextureSet();
    
    void setUsingGamePadTextureSet(bool isUsingGamePadTextureSet);

private:
    int _textureSetFlag;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    MainAssets();
    MainAssets(const MainAssets&);
    MainAssets& operator=(const MainAssets&);
};

#endif /* defined(__dante__MainAssets__) */
