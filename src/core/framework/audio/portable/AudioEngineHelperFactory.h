//
//  AudioEngineHelperFactory.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/8/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__AudioEngineHelperFactory__
#define __noctisgames__AudioEngineHelperFactory__

#define NG_AUDIO_ENGINE_HELPER_FACTORY (AudioEngineHelperFactory::getInstance())

class AudioEngineHelper;

class AudioEngineHelperFactory
{
    friend class NGAudioEngine;
    
public:
	static AudioEngineHelperFactory* getInstance();

private:
    AudioEngineHelper* createAudioEngineHelper();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    AudioEngineHelperFactory();
    AudioEngineHelperFactory(const AudioEngineHelperFactory&);
    AudioEngineHelperFactory& operator=(const AudioEngineHelperFactory&);
};

#endif /* defined(__noctisgames__AudioEngineHelperFactory__) */
