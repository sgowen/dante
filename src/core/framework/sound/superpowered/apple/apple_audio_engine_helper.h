//
//  apple_audio_engine_helper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/8/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__apple_audio_engine_helper__
#define __noctisgames__apple_audio_engine_helper__

#include "SuperpoweredSoundManager.h"

void initializeWithSuperpoweredSoundManager(SuperpoweredSoundManager* superpoweredSoundManager, int sampleRate = 44100);

const char * getBundlePathForSoundWithName(const char* soundName);

#endif /* defined(__noctisgames__apple_audio_engine_helper__) */
