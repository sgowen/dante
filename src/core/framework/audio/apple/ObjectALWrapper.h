//
//  ObjectALWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ObjectALWrapper__
#define __noctisgames__ObjectALWrapper__

#ifdef __OBJC__
#import "ObjectAL.h"
#endif

void initObjectAL();

void deinitObjectAL();

void pauseObjectAL();

void resumeObjectAL();

int loadSound(const char* pathCString);

void playSound(int bufferIndex, float inVolume, bool isLooping);

void stopSound(int bufferIndex);

void pauseSound(int bufferIndex);

void resumeSound(int bufferIndex);

void setSoundVolume(int bufferIndex, float volume);

bool isSoundPlaying(int bufferIndex);

void loadMusic(const char *pathCString);

void playMusic(bool isLooping, float inVolume);

void setMusicVolume(float inVolume);

void stopMusic();

void pauseMusic();

void resumeMusic();

bool isMusicPlaying();

bool isMusicLoaded();

void unloadMusic();

#endif /* defined(__noctisgames__ObjectALWrapper__) */
