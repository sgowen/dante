//
//  NGAnimation.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/3/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/NGAnimation.h"

#include "framework/graphics/portable/TextureRegion.h"

#include <math.h>
#include <assert.h>

NGAnimation::NGAnimation(std::string textureName, int x, int y, int regionWidth, int regionHeight, int animationWidth, int animationHeight, int textureWidth, int textureHeight, bool isLooping, int firstLoopingFrame, int xPadding, int yPadding, std::vector<uint16_t> frameTimes) : _textureName(textureName), _cycleTime(0), _firstLoopingFrame(firstLoopingFrame), _isLooping(isLooping)
{
    int numFrames = static_cast<int>(frameTimes.size());
    
	loadTextureRegions(x, y, regionWidth, regionHeight, animationWidth, animationHeight, textureWidth, textureHeight, numFrames, xPadding, yPadding);
    
    _frameTimes.reserve(numFrames);
    for (int i = 0; i < numFrames; ++i)
    {
        uint16_t f = frameTimes[i];
        _frameTimes.push_back(f);
        _cycleTime += f;
    }
    
    assert(_cycleTime > 0);
}

NGAnimation::NGAnimation(std::string textureName, int x, int y, int regionWidth, int regionHeight, int animationWidth, int animationHeight, int textureWidth, int textureHeight, bool isLooping, int firstLoopingFrame, int xPadding, int yPadding, uint16_t frameTime, int numFrames) : _textureName(textureName), _cycleTime(0), _firstLoopingFrame(firstLoopingFrame), _isLooping(isLooping)
{
	loadTextureRegions(x, y, regionWidth, regionHeight, animationWidth, animationHeight, textureWidth, textureHeight, numFrames, xPadding, yPadding);

    _frameTimes.reserve(numFrames);
	for (int i = 0; i < numFrames; ++i)
	{
		_frameTimes.push_back(frameTime);
		_cycleTime += frameTime;
	}
    
    assert(_cycleTime > 0);
}

NGAnimation::~NGAnimation()
{
    _textureRegions.clear();
    _frameTimes.clear();
}

TextureRegion& NGAnimation::getTextureRegion(uint16_t stateTime)
{
	int keyFrameNumber = getKeyFrameNumber(stateTime);

	return getTextureRegionAtKeyFrame(keyFrameNumber);
}

TextureRegion& NGAnimation::getTextureRegionAtKeyFrame(uint16_t keyFrameNumber)
{
	return _textureRegions.at(keyFrameNumber);
}

uint16_t NGAnimation::getKeyFrameNumber(uint16_t stateTime)
{
    if (stateTime >= _cycleTime)
    {
        if (_isLooping)
        {
            uint16_t cycleTime = _cycleTime;
            if (_firstLoopingFrame > 0)
            {
                for (uint16_t i = 0; i < _firstLoopingFrame; ++i)
                {
                    cycleTime -= _frameTimes.at(i);
                }
            }
            
            stateTime %= cycleTime;
        }
        else
        {
            return static_cast<int>(_frameTimes.size()) - 1;
        }
    }
    
    for (uint16_t i = 0; i < _frameTimes.size(); ++i)
    {
        uint16_t frameTime = _frameTimes.at(i);
        
        if (stateTime < frameTime)
        {
            return i;
        }
        
        stateTime -= frameTime;
    }
    
    return 0;
}

std::string& NGAnimation::getTextureName()
{
    return _textureName;
}

void NGAnimation::loadTextureRegions(int x, int y, int regionWidth, int regionHeight, int animationWidth, int animationHeight, int textureWidth, int textureHeight, int numFrames, int xPadding, int yPadding)
{
	int right = x + animationWidth;
	int bottom = y + animationHeight;
	int numTextureRegionsAdded = 0;
	for (int j = y; j < bottom; j += regionHeight + yPadding)
	{
		for (int i = x; i < right; i += regionWidth + xPadding)
		{
			TextureRegion tr = TextureRegion(_textureName, i, j, regionWidth, regionHeight, textureWidth, textureHeight);
			_textureRegions.push_back(tr);
			numTextureRegionsAdded++;

			if (numTextureRegionsAdded == numFrames)
			{
				return;
			}
		}
	}
}
