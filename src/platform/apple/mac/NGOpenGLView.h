//
//  NGOpenGLView.h
//  dante
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>

@interface NGOpenGLView : NSOpenGLView
{
	CVDisplayLinkRef displayLink;
}

@end
