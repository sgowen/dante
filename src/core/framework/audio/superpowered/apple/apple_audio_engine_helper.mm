//
//  apple_audio_engine_helper.mm
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/8/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/audio/superpowered/apple/apple_audio_engine_helper.h"

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

#define AUDIO_PROCESSING_CALLBACK(name) \
static bool audioProcessing##name(void *clientData, float **buffers, unsigned int inputChannels, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime) \
{ \
return ((SuperpoweredSoundManager *)clientData)->process##name(buffers, (unsigned int)numberOfSamples, sampleRate); \
}

AUDIO_PROCESSING_CALLBACK(Music);
AUDIO_PROCESSING_CALLBACK(Sound1);
AUDIO_PROCESSING_CALLBACK(Sound2);
AUDIO_PROCESSING_CALLBACK(Sound3);
AUDIO_PROCESSING_CALLBACK(Sound4);
AUDIO_PROCESSING_CALLBACK(Sound5);
AUDIO_PROCESSING_CALLBACK(Sound6);
AUDIO_PROCESSING_CALLBACK(Sound7);
AUDIO_PROCESSING_CALLBACK(Sound8);
AUDIO_PROCESSING_CALLBACK(Sound9);
AUDIO_PROCESSING_CALLBACK(Sound10);
AUDIO_PROCESSING_CALLBACK(Sound11);

#elif TARGET_OS_OSX

#define AUDIO_PROCESSING_CALLBACK(name) \
static bool audioProcessing##name(void *clientData, float **inputBuffers, unsigned int inputChannels, float **outputBuffers, unsigned int outputChannels, unsigned int numberOfSamples, unsigned int sampleRate, uint64_t hostTime) \
{ \
return ((SuperpoweredSoundManager *)clientData)->process##name(outputBuffers, (unsigned int)numberOfSamples, sampleRate); \
}

AUDIO_PROCESSING_CALLBACK(Music);
AUDIO_PROCESSING_CALLBACK(Sound1);
AUDIO_PROCESSING_CALLBACK(Sound2);
AUDIO_PROCESSING_CALLBACK(Sound3);
AUDIO_PROCESSING_CALLBACK(Sound4);
AUDIO_PROCESSING_CALLBACK(Sound5);
AUDIO_PROCESSING_CALLBACK(Sound6);
AUDIO_PROCESSING_CALLBACK(Sound7);
AUDIO_PROCESSING_CALLBACK(Sound8);
AUDIO_PROCESSING_CALLBACK(Sound9);
AUDIO_PROCESSING_CALLBACK(Sound10);
AUDIO_PROCESSING_CALLBACK(Sound11);

#endif

NSMutableArray *gAudioSystems = nil;

void initializeAudioSystemsWithSuperpoweredSoundManager(SuperpoweredSoundManager* superpoweredSoundManager, int sampleRate)
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

void deinitializeAudioSystems()
{
    if (gAudioSystems)
    {
        for (NSObject *audioSystem in gAudioSystems)
        {
            [audioSystem performSelector:@selector(stop) withObject:nil];
        }
        
        [gAudioSystems removeAllObjects];
    }
}

const char * getBundlePathForSoundWithName(const char* soundName)
{
    NSString *path = [[NSString alloc] initWithCString:soundName encoding:NSASCIIStringEncoding];
    const char *bundlePath = [[[NSBundle mainBundle] pathForResource:path ofType:@"wav"] fileSystemRepresentation];
    
    return bundlePath;
}
