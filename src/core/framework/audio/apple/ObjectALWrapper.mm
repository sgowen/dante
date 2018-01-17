//
//  ObjectALWrapper.mm
//  noctisgames-framework
//
//  Created by Stephen Gowen on 7/7/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/audio/apple/ObjectALWrapper.h"

#import "ARCSafe_MemMgmt.h"

ALDevice* gDevice = nil;
ALContext* gContext = nil;
ALChannelSource* gChannel = nil;
OALAudioTrack* gMusicTrack = nil;
NSMutableArray* gSounds = nil;

@interface ObjectALSoundBufferWrapper : NSObject
{
    ALBuffer* mBuffer;
    id<ALSoundSource> mSoundSource;
}

- (id)initWithSoundPath:(NSString *)path;

- (void)play:(float)inVolume isLooping:(bool)isLooping;

- (void)stop;

- (void)pause;

- (void)resume;

- (void)setVolume:(float)volume;

- (bool)isPlaying;

@end

@implementation ObjectALSoundBufferWrapper

- (id)initWithSoundPath:(NSString *)path
{
    self = [super init];
    
    if (self)
    {
        mBuffer = [[OpenALManager sharedInstance] bufferFromFile:path];
        mSoundSource = nil;
    }
    
    return self;
}

- (void)play:(float)inVolume isLooping:(bool)isLooping
{
    mSoundSource = [gChannel play:mBuffer gain:inVolume pitch:1.0f pan:0.0f loop:isLooping];
}

- (void)stop
{
    if (mSoundSource)
    {
        [mSoundSource stop];
    }
}

- (void)pause
{
    if (mSoundSource)
    {
        [mSoundSource setPaused:true];
    }
}

- (void)resume
{
    if (mSoundSource)
    {
        [mSoundSource setPaused:false];
    }
}

- (void)setVolume:(float)volume
{
    if (mSoundSource)
    {
        [mSoundSource setVolume:volume];
    }
}

- (bool)isPlaying
{
    return mSoundSource && [mSoundSource playing];
}

@end

void initObjectAL()
{
    gSounds = nil;
    gMusicTrack = nil;
    gChannel = nil;
    gContext = nil;
    gDevice = nil;
    
    // Create the device and context.
    // Note that it's easier to just let OALSimpleAudio handle
    // these rather than make and manage them yourself.
    gDevice = [ALDevice deviceWithDeviceSpecifier:nil];
    gContext = [ALContext contextOnDevice:gDevice attributes:nil];
    [OpenALManager sharedInstance].currentContext = gContext;
    
    // Deal with interruptions for me!
    [OALAudioSession sharedInstance].handleInterruptions = YES;
    
    // We don't want ipod music to keep playing since
    // we have our own bg music.
    [OALAudioSession sharedInstance].allowIpod = NO;
    
    // Mute all audio if the silent switch is turned on.
    [OALAudioSession sharedInstance].honorSilentSwitch = YES;
    
    // Take all 32 sources for this channel.
    // (we probably won't use that many but what the heck!)
    gChannel = [ALChannelSource channelWithSources:32];
    
    gMusicTrack = [OALAudioTrack track];
    
    gSounds = [[NSMutableArray alloc] init];
}

void deinitObjectAL()
{
    // Stop all music and sound effects.
    [gChannel stop];
    [gChannel clear];
    [gChannel resetToDefault];
    
    [gMusicTrack stop];
    [gSounds removeAllObjects];
    
    // Note: You'll likely only have one device and context open throughout
    // your program, so in a real program you'd be better off making a
    // singleton object that manages the device and context, rather than
    // allocating/deallocating it here.
    // Most of the demos just let OALSimpleAudio manage the device and context
    // for them.
    as_release(gSounds);
    as_release(gMusicTrack);
    as_release(gChannel);
    as_release(gContext);
    as_release(gDevice);
}

void pauseObjectAL()
{
    gMusicTrack.paused = YES;
    gChannel.paused = YES;
}

void resumeObjectAL()
{
    gChannel.paused = NO;
    gMusicTrack.paused = NO;
}

int loadSound(const char* pathCString)
{
    NSString *path = [[NSString alloc] initWithCString:pathCString encoding:NSASCIIStringEncoding];
    
    int ret = (int) [gSounds count];
    
    [gSounds addObject:[[ObjectALSoundBufferWrapper alloc] initWithSoundPath:path]];
    
    return ret;
}

void playSound(int bufferIndex, float inVolume, bool isLooping)
{
    ObjectALSoundBufferWrapper* sound = [gSounds objectAtIndex:bufferIndex];
    
    [sound play:inVolume isLooping:isLooping];
}

void stopSound(int bufferIndex)
{
    ObjectALSoundBufferWrapper* sound = [gSounds objectAtIndex:bufferIndex];
    
    [sound stop];
}

void pauseSound(int bufferIndex)
{
    ObjectALSoundBufferWrapper* sound = [gSounds objectAtIndex:bufferIndex];
    
    [sound pause];
}

void resumeSound(int bufferIndex)
{
    ObjectALSoundBufferWrapper* sound = [gSounds objectAtIndex:bufferIndex];
    
    [sound resume];
}

void setSoundVolume(int bufferIndex, float volume)
{
    ObjectALSoundBufferWrapper* sound = [gSounds objectAtIndex:bufferIndex];
    
    [sound setVolume:volume];
}

bool isSoundPlaying(int bufferIndex)
{
    ObjectALSoundBufferWrapper* sound = [gSounds objectAtIndex:bufferIndex];
    
    return [sound isPlaying];
}

void loadMusic(const char* pathCString)
{
    NSString *path = [[NSString alloc] initWithCString:pathCString encoding:NSASCIIStringEncoding];
    
    [gMusicTrack preloadFile:path];
}

void playMusic(bool isLooping, float inVolume)
{
    [gMusicTrack setNumberOfLoops:isLooping ? -1 : 0];
    [gMusicTrack setVolume:inVolume];
    
    [gMusicTrack play];
}

void setMusicVolume(float inVolume)
{
    [gMusicTrack setVolume:inVolume];
}

void stopMusic()
{
    [gMusicTrack stop];
}

void pauseMusic()
{
    [gMusicTrack setPaused:true];
}

void resumeMusic()
{
    [gMusicTrack setPaused:false];
}

bool isMusicPlaying()
{
    return [gMusicTrack playing];
}

bool isMusicLoaded()
{
    return [gMusicTrack preloaded];
}

void unloadMusic()
{
    [gMusicTrack clear];
}

