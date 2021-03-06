//
//  IOSMain.mm
//  noctisgames
//
//  Created by Stephen Gowen on 11/7/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#import "IOSMain.h"

// C++
#include <framework/main/portable/Engine.h>
#include <framework/input/CursorInputManager.h>
#include <framework/util/Constants.h>

@interface IOSMain ()
{
    Engine* _engine;
}

@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation IOSMain

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
    CURSOR_INPUT_MANAGER->onInput(CursorEventType_DOWN, pos.x, pos.y);
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint pos = [touch locationInView: [UIApplication sharedApplication].keyWindow];
    CURSOR_INPUT_MANAGER->onInput(CursorEventType_DRAGGED, pos.x, pos.y);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint pos = [touch locationInView: [UIApplication sharedApplication].keyWindow];
    CURSOR_INPUT_MANAGER->onInput(CursorEventType_UP, pos.x, pos.y);
}

- (BOOL)prefersStatusBarHidden
{
    return YES;
}

#pragma mark must override

- (EngineController *)createEngineController
{
    return NULL;
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    int requestedAction = _engine->getRequestedAction();
    
    switch (requestedAction)
    {
        case REQUESTED_ACTION_EXIT:
            [self performHomePressAndExit];
            break;
        case REQUESTED_ACTION_UPDATE:
            break;
        default:
            _engine->clearRequestedAction();
            break;
    }
    
    _engine->update(self.timeSinceLastUpdate);
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    _engine->render();
}

#pragma mark Private

- (void)setupGL
{
    EngineController* engineController = [self createEngineController];
    _engine = new Engine(engineController);
    
    [EAGLContext setCurrentContext:self.context];
    
    CGRect screenBounds = [[UIScreen mainScreen] nativeBounds];
    
    CGSize size = CGSizeMake(screenBounds.size.width, screenBounds.size.height);
    size.width = roundf(size.width);
    size.height = roundf(size.height);
    
    _engine->createDeviceDependentResources();
    float width = MAX(size.width, size.height);
    float height = MIN(size.width, size.height);
    float touchWidth = [UIScreen mainScreen].bounds.size.width;
    float touchHeight = [UIScreen mainScreen].bounds.size.height;
    
    _engine->createWindowSizeDependentResources(width, height, touchWidth, touchHeight);
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
    
    _engine->releaseDeviceDependentResources();
    delete _engine;
}

- (void)onResume
{
    _engine->onResume();
}

- (void)onPause
{
    _engine->onPause();
}

- (void)performHomePressAndExit
{
    //home button press programmatically
    UIApplication *app = [UIApplication sharedApplication];
    [app performSelector:@selector(suspend)];
    
    //wait 2 seconds while app is going background
    [NSThread sleepForTimeInterval:2.0];
    
    //exit app when app is in background
    exit(0);
}

@end
