//
//  ViewController.mm
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/7/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#import "ViewController.h"

#include <game/config/MainEngineController.h>

@implementation ViewController

- (EngineController *)createEngineController
{
    static MainEngineController controller;
    return &controller;
}

@end
