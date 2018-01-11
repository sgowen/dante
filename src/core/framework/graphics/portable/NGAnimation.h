//
//  NGAnimation.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/3/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGAnimation__
#define __noctisgames__NGAnimation__

#include <vector>
#include <string>

class TextureRegion;

class NGAnimation
{
public:
    NGAnimation(std::string textureName, int x, int y, int regionWidth, int regionHeight, int animationWidth, int animationHeight, int textureWidth, int textureHeight, bool looping, int firstLoopingFrame, int xPadding, int yPadding, std::vector<uint16_t> frameTimes);
    NGAnimation(std::string textureName, int x, int y, int regionWidth, int regionHeight, int animationWidth, int animationHeight, int textureWidth, int textureHeight, bool looping, int firstLoopingFrame, int xPadding, int yPadding, uint16_t frameTime, int numFrames);
    ~NGAnimation();
    
    TextureRegion& getTextureRegion(uint16_t stateTime);
    TextureRegion& getTextureRegionAtKeyFrame(uint16_t keyFrameNumber);
    uint16_t getKeyFrameNumber(uint16_t stateTime);
    std::string& getTextureName();
    
private:
    std::vector<TextureRegion> _textureRegions;
    std::vector<uint16_t> _frameTimes;
    std::string _textureName;
    uint16_t _cycleTime;
    uint16_t _firstLoopingFrame;
    bool _isLooping;
    
    void loadTextureRegions(int x, int y, int regionWidth, int regionHeight, int animationWidth, int animationHeight, int textureWidth, int textureHeight, int numFrames, int xPadding = 0, int yPadding = 0);
};

#endif /* defined(__noctisgames__NGAnimation__) */
