//
//  apple_audio_engine_helper.mm
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/8/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "apple_audio_engine_helper.h"

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#if TARGET_OS_IPHONE
#import "SuperpoweredIOSAudioIO.h"
#elif TARGET_OS_OSX
#import "SuperpoweredOSXAudioIO.h"
#endif

#if TARGET_OS_IPHONE
@interface MySuperpoweredIOSAudioIODelegate : NSObject <SuperpoweredIOSAudioIODelegate>

- (void)interruptionStarted;
- (void)interruptionEnded;
- (void)recordPermissionRefused;
- (void)mapChannels:(multiOutputChannelMap *)outputMap inputMap:(multiInputChannelMap *)inputMap externalAudioDeviceName:(NSString *)externalAudioDeviceName outputsAndInputs:(NSString *)outputsAndInputs;

@end

@implementation MySuperpoweredIOSAudioIODelegate

- (void)interruptionStarted {}
- (void)interruptionEnded {}
- (void)recordPermissionRefused {}
- (void)mapChannels:(multiOutputChannelMap *)outputMap inputMap:(multiInputChannelMap *)inputMap externalAudioDeviceName:(NSString *)externalAudioDeviceName outputsAndInputs:(NSString *)outputsAndInputs {}

@end

static bool audioProcessingMusic(void *clientData, float **buffers, unsigned int inputChannels, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processMusic(buffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound1(void *clientData, float **buffers, unsigned int inputChannels, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound1(buffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound2(void *clientData, float **buffers, unsigned int inputChannels, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound2(buffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound3(void *clientData, float **buffers, unsigned int inputChannels, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound3(buffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound4(void *clientData, float **buffers, unsigned int inputChannels, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound4(buffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound5(void *clientData, float **buffers, unsigned int inputChannels, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound5(buffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound6(void *clientData, float **buffers, unsigned int inputChannels, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound6(buffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound7(void *clientData, float **buffers, unsigned int inputChannels, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound7(buffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound8(void *clientData, float **buffers, unsigned int inputChannels, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound8(buffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound9(void *clientData, float **buffers, unsigned int inputChannels, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound9(buffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound10(void *clientData, float **buffers, unsigned int inputChannels, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound10(buffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound11(void *clientData, float **buffers, unsigned int inputChannels, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound11(buffers, (unsigned int)numberOfSamples, sampleRate);
}
#elif TARGET_OS_OSX
static bool audioProcessingMusic(void *clientData, float **inputBuffers, unsigned int inputChannels, float **outputBuffers, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processMusic(outputBuffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound1(void *clientData, float **inputBuffers, unsigned int inputChannels, float **outputBuffers, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound1(outputBuffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound2(void *clientData, float **inputBuffers, unsigned int inputChannels, float **outputBuffers, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound2(outputBuffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound3(void *clientData, float **inputBuffers, unsigned int inputChannels, float **outputBuffers, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound3(outputBuffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound4(void *clientData, float **inputBuffers, unsigned int inputChannels, float **outputBuffers, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound4(outputBuffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound5(void *clientData, float **inputBuffers, unsigned int inputChannels, float **outputBuffers, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound5(outputBuffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound6(void *clientData, float **inputBuffers, unsigned int inputChannels, float **outputBuffers, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound6(outputBuffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound7(void *clientData, float **inputBuffers, unsigned int inputChannels, float **outputBuffers, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound7(outputBuffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound8(void *clientData, float **inputBuffers, unsigned int inputChannels, float **outputBuffers, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound8(outputBuffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound9(void *clientData, float **inputBuffers, unsigned int inputChannels, float **outputBuffers, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound9(outputBuffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound10(void *clientData, float **inputBuffers, unsigned int inputChannels, float **outputBuffers, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound10(outputBuffers, (unsigned int)numberOfSamples, sampleRate);
}

static bool audioProcessingSound11(void *clientData, float **inputBuffers, unsigned int inputChannels, float **outputBuffers, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime)
{
    return ((SuperpoweredSoundManager *)clientData)->processSound11(outputBuffers, (unsigned int)numberOfSamples, sampleRate);
}
#endif

NSMutableArray *gAudioSystems = nil;

void initializeWithSuperpoweredSoundManager(SuperpoweredSoundManager* superpoweredSoundManager, int sampleRate)
{
    if (gAudioSystems)
    {
        return;
    }
    
    gAudioSystems = [[NSMutableArray alloc] init];
    
#if TARGET_OS_IPHONE
    MySuperpoweredIOSAudioIODelegate* del = [[MySuperpoweredIOSAudioIODelegate alloc] init];
    
    [gAudioSystems addObject:[[SuperpoweredIOSAudioIO alloc] initWithDelegate:del preferredBufferSize:12 preferredMinimumSamplerate:sampleRate audioSessionCategory:AVAudioSessionCategoryPlayback channels:2 audioProcessingCallback:audioProcessingMusic clientdata:superpoweredSoundManager]];
    
    [gAudioSystems addObject:[[SuperpoweredIOSAudioIO alloc] initWithDelegate:del preferredBufferSize:12 preferredMinimumSamplerate:sampleRate audioSessionCategory:AVAudioSessionCategoryPlayback channels:2 audioProcessingCallback:audioProcessingSound1 clientdata:superpoweredSoundManager]];
    [gAudioSystems addObject:[[SuperpoweredIOSAudioIO alloc] initWithDelegate:del preferredBufferSize:12 preferredMinimumSamplerate:sampleRate audioSessionCategory:AVAudioSessionCategoryPlayback channels:2 audioProcessingCallback:audioProcessingSound2 clientdata:superpoweredSoundManager]];
    [gAudioSystems addObject:[[SuperpoweredIOSAudioIO alloc] initWithDelegate:del preferredBufferSize:12 preferredMinimumSamplerate:sampleRate audioSessionCategory:AVAudioSessionCategoryPlayback channels:2 audioProcessingCallback:audioProcessingSound3 clientdata:superpoweredSoundManager]];
    [gAudioSystems addObject:[[SuperpoweredIOSAudioIO alloc] initWithDelegate:del preferredBufferSize:12 preferredMinimumSamplerate:sampleRate audioSessionCategory:AVAudioSessionCategoryPlayback channels:2 audioProcessingCallback:audioProcessingSound4 clientdata:superpoweredSoundManager]];
    [gAudioSystems addObject:[[SuperpoweredIOSAudioIO alloc] initWithDelegate:del preferredBufferSize:12 preferredMinimumSamplerate:sampleRate audioSessionCategory:AVAudioSessionCategoryPlayback channels:2 audioProcessingCallback:audioProcessingSound5 clientdata:superpoweredSoundManager]];
    [gAudioSystems addObject:[[SuperpoweredIOSAudioIO alloc] initWithDelegate:del preferredBufferSize:12 preferredMinimumSamplerate:sampleRate audioSessionCategory:AVAudioSessionCategoryPlayback channels:2 audioProcessingCallback:audioProcessingSound6 clientdata:superpoweredSoundManager]];
    [gAudioSystems addObject:[[SuperpoweredIOSAudioIO alloc] initWithDelegate:del preferredBufferSize:12 preferredMinimumSamplerate:sampleRate audioSessionCategory:AVAudioSessionCategoryPlayback channels:2 audioProcessingCallback:audioProcessingSound7 clientdata:superpoweredSoundManager]];
    [gAudioSystems addObject:[[SuperpoweredIOSAudioIO alloc] initWithDelegate:del preferredBufferSize:12 preferredMinimumSamplerate:sampleRate audioSessionCategory:AVAudioSessionCategoryPlayback channels:2 audioProcessingCallback:audioProcessingSound8 clientdata:superpoweredSoundManager]];
    [gAudioSystems addObject:[[SuperpoweredIOSAudioIO alloc] initWithDelegate:del preferredBufferSize:12 preferredMinimumSamplerate:sampleRate audioSessionCategory:AVAudioSessionCategoryPlayback channels:2 audioProcessingCallback:audioProcessingSound9 clientdata:superpoweredSoundManager]];
    [gAudioSystems addObject:[[SuperpoweredIOSAudioIO alloc] initWithDelegate:del preferredBufferSize:12 preferredMinimumSamplerate:sampleRate audioSessionCategory:AVAudioSessionCategoryPlayback channels:2 audioProcessingCallback:audioProcessingSound10 clientdata:superpoweredSoundManager]];
    [gAudioSystems addObject:[[SuperpoweredIOSAudioIO alloc] initWithDelegate:del preferredBufferSize:12 preferredMinimumSamplerate:sampleRate audioSessionCategory:AVAudioSessionCategoryPlayback channels:2 audioProcessingCallback:audioProcessingSound11 clientdata:superpoweredSoundManager]];
#elif TARGET_OS_OSX
    [gAudioSystems addObject:[[SuperpoweredOSXAudioIO alloc] initWithDelegate:nil preferredBufferSizeMs:12 numberOfChannels:2 enableInput:false enableOutput:true]];
    
    [gAudioSystems addObject:[[SuperpoweredOSXAudioIO alloc] initWithDelegate:nil preferredBufferSizeMs:12 numberOfChannels:2 enableInput:false enableOutput:true]];
    [gAudioSystems addObject:[[SuperpoweredOSXAudioIO alloc] initWithDelegate:nil preferredBufferSizeMs:12 numberOfChannels:2 enableInput:false enableOutput:true]];
    [gAudioSystems addObject:[[SuperpoweredOSXAudioIO alloc] initWithDelegate:nil preferredBufferSizeMs:12 numberOfChannels:2 enableInput:false enableOutput:true]];
    [gAudioSystems addObject:[[SuperpoweredOSXAudioIO alloc] initWithDelegate:nil preferredBufferSizeMs:12 numberOfChannels:2 enableInput:false enableOutput:true]];
    [gAudioSystems addObject:[[SuperpoweredOSXAudioIO alloc] initWithDelegate:nil preferredBufferSizeMs:12 numberOfChannels:2 enableInput:false enableOutput:true]];
    [gAudioSystems addObject:[[SuperpoweredOSXAudioIO alloc] initWithDelegate:nil preferredBufferSizeMs:12 numberOfChannels:2 enableInput:false enableOutput:true]];
    [gAudioSystems addObject:[[SuperpoweredOSXAudioIO alloc] initWithDelegate:nil preferredBufferSizeMs:12 numberOfChannels:2 enableInput:false enableOutput:true]];
    [gAudioSystems addObject:[[SuperpoweredOSXAudioIO alloc] initWithDelegate:nil preferredBufferSizeMs:12 numberOfChannels:2 enableInput:false enableOutput:true]];
    [gAudioSystems addObject:[[SuperpoweredOSXAudioIO alloc] initWithDelegate:nil preferredBufferSizeMs:12 numberOfChannels:2 enableInput:false enableOutput:true]];
    [gAudioSystems addObject:[[SuperpoweredOSXAudioIO alloc] initWithDelegate:nil preferredBufferSizeMs:12 numberOfChannels:2 enableInput:false enableOutput:true]];
    [gAudioSystems addObject:[[SuperpoweredOSXAudioIO alloc] initWithDelegate:nil preferredBufferSizeMs:12 numberOfChannels:2 enableInput:false enableOutput:true]];
    
    [[gAudioSystems objectAtIndex:0] setProcessingCallback_C:audioProcessingMusic clientdata:superpoweredSoundManager];
    
    [[gAudioSystems objectAtIndex:1] setProcessingCallback_C:audioProcessingSound1 clientdata:superpoweredSoundManager];
    [[gAudioSystems objectAtIndex:2] setProcessingCallback_C:audioProcessingSound2 clientdata:superpoweredSoundManager];
    [[gAudioSystems objectAtIndex:3] setProcessingCallback_C:audioProcessingSound3 clientdata:superpoweredSoundManager];
    [[gAudioSystems objectAtIndex:4] setProcessingCallback_C:audioProcessingSound4 clientdata:superpoweredSoundManager];
    [[gAudioSystems objectAtIndex:5] setProcessingCallback_C:audioProcessingSound5 clientdata:superpoweredSoundManager];
    [[gAudioSystems objectAtIndex:6] setProcessingCallback_C:audioProcessingSound6 clientdata:superpoweredSoundManager];
    [[gAudioSystems objectAtIndex:7] setProcessingCallback_C:audioProcessingSound7 clientdata:superpoweredSoundManager];
    [[gAudioSystems objectAtIndex:8] setProcessingCallback_C:audioProcessingSound8 clientdata:superpoweredSoundManager];
    [[gAudioSystems objectAtIndex:9] setProcessingCallback_C:audioProcessingSound9 clientdata:superpoweredSoundManager];
    [[gAudioSystems objectAtIndex:10] setProcessingCallback_C:audioProcessingSound10 clientdata:superpoweredSoundManager];
    [[gAudioSystems objectAtIndex:11] setProcessingCallback_C:audioProcessingSound11 clientdata:superpoweredSoundManager];
#endif
    
    for (NSObject *audioSystem in gAudioSystems)
    {
        [audioSystem performSelector:@selector(start) withObject:nil];
    }
}

const char * getBundlePathForSoundWithName(const char* soundName)
{
    NSString *path = [[NSString alloc] initWithCString:soundName encoding:NSASCIIStringEncoding];
    const char *bundlePath = [[[NSBundle mainBundle] pathForResource:path ofType:@"wav"] fileSystemRepresentation];
    
    return bundlePath;
}
