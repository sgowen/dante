//
//  IOSMain.h
//  noctisgames
//
//  Created by Stephen Gowen on 11/7/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#include <framework/main/portable/EngineController.h>

@interface IOSMain : GLKViewController

- (EngineController *)createEngineController;

@end
