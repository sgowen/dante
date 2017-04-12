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

class IAudioEngineHelper;

class AudioEngineHelperFactory
{
public:
	static AudioEngineHelperFactory* getInstance();
    
    IAudioEngineHelper* createAudioEngineHelper();

private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    AudioEngineHelperFactory();
    AudioEngineHelperFactory(const AudioEngineHelperFactory&);
    AudioEngineHelperFactory& operator=(const AudioEngineHelperFactory&);
};

#endif /* defined(__noctisgames__AudioEngineHelperFactory__) */
