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

NGAnimation::NGAnimation(std::string textureName, int x, int y, int regionWidth, int regionHeight, int animationWidth, int animationHeight, int textureWidth, int textureHeight, bool isLooping, int firstLoopingFrame, int xPadding, int yPadding, std::vector<float> frameTimes) : _textureName(textureName), _cycleTime(0), _firstLoopingFrame(firstLoopingFrame), _isLooping(isLooping)
{
    int numFrames = static_cast<int>(frameTimes.size());
    
	loadTextureRegions(x, y, regionWidth, regionHeight, animationWidth, animationHeight, textureWidth, textureHeight, numFrames, xPadding, yPadding);
    
    _frameTimes.reserve(numFrames);
    for (int i = 0; i < numFrames; ++i)
    {
        float f = frameTimes[i];
        _frameTimes.push_back(f);
        _cycleTime += f;
    }
}

NGAnimation::NGAnimation(std::string textureName, int x, int y, int regionWidth, int regionHeight, int animationWidth, int animationHeight, int textureWidth, int textureHeight, bool isLooping, int firstLoopingFrame, int xPadding, int yPadding, float frameTime, int numFrames) : _textureName(textureName), _cycleTime(0), _firstLoopingFrame(firstLoopingFrame), _isLooping(isLooping)
{
	loadTextureRegions(x, y, regionWidth, regionHeight, animationWidth, animationHeight, textureWidth, textureHeight, numFrames, xPadding, yPadding);

    _frameTimes.reserve(numFrames);
	for (int i = 0; i < numFrames; ++i)
	{
		_frameTimes.push_back(frameTime);
		_cycleTime += frameTime;
	}
}

NGAnimation::~NGAnimation()
{
    _textureRegions.clear();
    _frameTimes.clear();
}

TextureRegion& NGAnimation::getTextureRegion(float stateTime)
{
	int keyFrameNumber = getKeyFrameNumber(stateTime);

	return getTextureRegion(keyFrameNumber);
}

TextureRegion& NGAnimation::getTextureRegion(int keyFrameNumber)
{
	return _textureRegions.at(keyFrameNumber);
}

int NGAnimation::getKeyFrameNumber(float stateTime)
{
    unsigned int i = 0;
    
    if (stateTime > _cycleTime && _cycleTime > 0)
    {
        if (_isLooping)
        {
            float cycleTime = _cycleTime;
            for ( ; i < _firstLoopingFrame; ++i)
            {
                cycleTime -= _frameTimes.at(i);
            }
            
            while (stateTime > cycleTime)
            {
                stateTime -= cycleTime;
            }
        }
        else
        {
            return ((int) _frameTimes.size()) - 1;
        }
    }
    
    for ( ; i < _frameTimes.size(); ++i)
    {
        float frameTime = _frameTimes.at(i);
        
        if (stateTime < frameTime)
        {
            return i;
        }
        
        stateTime -= frameTime;
    }
    
    return 0;
}

bool NGAnimation::hasFrameTimes()
{
    return _frameTimes.size() > 0;
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
