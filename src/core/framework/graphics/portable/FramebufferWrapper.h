//
//  FramebufferWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/19/18.
//  Copyright (c) 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__FramebufferWrapper__
#define __noctisgames__FramebufferWrapper__

#include <framework/graphics/portable/NGGraphics.h>
#include <framework/graphics/portable/NGTexture.h>

struct FramebufferWrapper
{
    int renderWidth;
    int renderHeight;
    NGTexture* texture;
    
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    GLuint fboTexture;
    GLuint fbo;

    FramebufferWrapper(int inRenderWidth, int inRenderHeight) : renderWidth(inRenderWidth), renderHeight(inRenderHeight), texture(new NGTexture("framebuffer", NULL, NULL, false)), fboTexture(0), fbo(0) {}
#elif defined _WIN32
    ID3D11Texture2D* renderTarget;
    ID3D11RenderTargetView* renderTargetView;
    ID3D11ShaderResourceView* shaderResourceView;

    FramebufferWrapper(int inRenderWidth, int inRenderHeight) : renderWidth(inRenderWidth), renderHeight(inRenderHeight), texture(new NGTexture("framebuffer", NULL, NULL, false)), renderTarget(NULL), renderTargetView(NULL), shaderResourceView(NULL) {}
#endif
};

#endif /* defined(__noctisgames__FramebufferWrapper__) */
