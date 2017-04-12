//
//  MainViewController.m
//  dante
//
//  Created by Stephen Gowen on 3/13/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#import "MainViewController.h"

// C++
#include "MainScreen.h"
#include "ScreenInputManager.h"
#include "MainAssets.h"
#include "GameConstants.h"
#include "OpenGLManager.h"

@interface MainViewController ()
{
    MainScreen *_mainScreen;
}

@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation MainViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context)
    {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.userInteractionEnabled = YES;
    view.multipleTouchEnabled = YES;
    
    [view bindDrawable];
    
    self.preferredFramesPerSecond = 60;
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(onPause)
                                                 name:UIApplicationWillResignActiveNotification
                                               object:nil];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(onResume)
                                                 name:UIApplicationDidBecomeActiveNotification
                                               object:nil];
    
    [self setupGL];
}

- (void)dealloc
{    
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context)
    {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil))
    {
        self.view = nil;
        
        [self tearDownGL];
        
        if ([EAGLContext currentContext] == self.context)
        {
            [EAGLContext setCurrentContext:nil];
        }
        
        self.context = nil;
    }

    // Dispose of any resources that can be recreated.
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint pos = [touch locationInView: [UIApplication sharedApplication].keyWindow];
    SCREEN_INPUT_MANAGER->onTouch(ScreenEventType_DOWN, pos.x, pos.y);
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint pos = [touch locationInView: [UIApplication sharedApplication].keyWindow];
    SCREEN_INPUT_MANAGER->onTouch(ScreenEventType_DRAGGED, pos.x, pos.y);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint pos = [touch locationInView: [UIApplication sharedApplication].keyWindow];
    SCREEN_INPUT_MANAGER->onTouch(ScreenEventType_UP, pos.x, pos.y);
}

- (BOOL)prefersStatusBarHidden
{
    return YES;
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    int requestedAction = _mainScreen->getRequestedAction();
    
    switch (requestedAction)
    {
        case REQUESTED_ACTION_UPDATE:
        default:
            break;
    }
    
    _mainScreen->update(self.timeSinceLastUpdate);
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    _mainScreen->render();
}

#pragma mark Private

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
    
    CGRect screenBounds = [[UIScreen mainScreen] nativeBounds];
    
    CGSize size = CGSizeMake(screenBounds.size.width, screenBounds.size.height);
    size.width = roundf(size.width);
    size.height = roundf(size.height);
    
    unsigned long long ramSize = [NSProcessInfo processInfo].physicalMemory;
    bool isLowMemoryDevice = ramSize < 1610612736; // 1536 MB
    
    MAIN_ASSETS->setUsingCompressedTextureSet(isLowMemoryDevice);
    
    _mainScreen = new MainScreen();
    _mainScreen->createDeviceDependentResources();
    float width = MAX(size.width, size.height);
    float height = MIN(size.width, size.height);
    float touchWidth = [UIScreen mainScreen].bounds.size.width;
    float touchHeight = [UIScreen mainScreen].bounds.size.height;
    
    OGLManager->setScreenSize(width, height);
    
    _mainScreen->createWindowSizeDependentResources(width > 1440 ? 1440 : width, height > 900 ? 900 : height, touchWidth, touchHeight);
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
    
    delete _mainScreen;
}

- (void)onResume
{
    _mainScreen->onResume();
}

- (void)onPause
{
    _mainScreen->onPause();
}

@end
