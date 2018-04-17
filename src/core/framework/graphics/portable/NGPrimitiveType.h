//
//  NGPrimitiveType.h
//  noctisgames
//
//  Created by Stephen Gowen on 12/16/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGPrimitiveType__
#define __noctisgames__NGPrimitiveType__

#include <framework/graphics/portable/NGGraphics.h>

typedef enum
{
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    NGPrimitiveType_Triangles = GL_TRIANGLES,
    NGPrimitiveType_TriangleStrip = GL_TRIANGLE_STRIP,
    NGPrimitiveType_Lines = GL_LINES,
    NGPrimitiveType_LineStrip = GL_LINE_STRIP
#elif defined _WIN32
    NGPrimitiveType_Triangles = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
    NGPrimitiveType_TriangleStrip = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
    NGPrimitiveType_Lines = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
    NGPrimitiveType_LineStrip = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
#endif
} NGPrimitiveType;

#endif /* defined(__noctisgames__NGPrimitiveType__) */
