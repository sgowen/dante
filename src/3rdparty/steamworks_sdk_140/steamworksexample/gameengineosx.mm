//========= Copyright 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Main class for the game engine -- osx implementation
//
// $NoKeywords: $
//=============================================================================

#include "stdafx.h"

#include <Cocoa/Cocoa.h>
#include <map>
#include <queue>
#include <sys/time.h>

#include "glstringosx.h"
#include "gameengineosx.h"


CGameEngineGL *g_engine;		// dxabstract will use this.. it is set by the engine constructor

IGameEngine *CreateGameEngineOSX()
{
    static CGameEngineGL* s_pGameEngine = NULL;
    
    if (!s_pGameEngine)
    {
        s_pGameEngine = new CGameEngineGL();
    }
    
    return s_pGameEngine;
}

inline uint64_t GetTickCount()
{
    timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000ULL) + (time.tv_usec / 1000ULL);
}

void OutputDebugString( const char *pchMsg )
{
    fprintf( stderr, "%s", pchMsg );
}

struct Packet_t
{
    uint32 unSize;
    void *pData;
};

class CVoiceContext
{
public:
    CVoiceContext()
    {
        alGenBuffers( ARRAYSIZE(m_buffers), m_buffers );
        alGenSources( 1, &m_nSource );
        
        alSourcei( m_nSource, AL_LOOPING, AL_FALSE );
        
        for (int i = 0; i < ARRAYSIZE(m_buffers); i++ )
            alSourcei( m_nSource, AL_BUFFER, m_buffers[i] );
        
        m_nNextFreeBuffer = 0;
    }
    virtual ~CVoiceContext()
    {
        //
    }
    
    ALuint m_buffers[4];
    ALuint m_nSource;
    size_t m_nNextFreeBuffer;
    std::queue<Packet_t> m_pending;
};

class AutoReleasePool
{
public:
    AutoReleasePool()
    {
        pool = [[NSAutoreleasePool alloc] init];
    }
    ~AutoReleasePool()
    {
        [pool release];
    }
    
    NSAutoreleasePool *pool;
};


@interface GLApplication : NSApplication
{
    
}
- (void)sendEvent:(NSEvent *)anEvent;
- (void)windowWillClose:(NSNotification *)notification;
@end

@implementation GLApplication
- (void)sendEvent:(NSEvent *)anEvent
{
    //fprintf( stderr, "sendEvent: %s\n", [[anEvent description] UTF8String] );
    
    [super sendEvent:anEvent];
}

- (void)windowWillClose:(NSNotification *)notification
{
    CreateGameEngineOSX()->Shutdown();
}

@end


//-----------------------------------------------------------------------------
// Purpose: Constructor for game engine instance
//-----------------------------------------------------------------------------
CGameEngineGL::CGameEngineGL()
{
    g_engine = this;
    
    m_bEngineReadyForUse = false;
    m_bShuttingDown = false;
    m_nWindowWidth = 0;
    m_nWindowHeight = 0;
    m_ulPreviousGameTickCount = 0;
    m_ulGameTickCount = 0;
    m_unVoiceChannelCount = 0;
    
    
    
    
    m_hTextureWhite = 0;
    m_nNextFontHandle = 1;
    m_nNextTextureHandle = 1;
    m_hLastTexture = 0;
    
    m_rgflPointsData = new GLfloat[ 3*POINT_BUFFER_TOTAL_SIZE ];
    m_rgflPointsColorData = new GLubyte[ 4*POINT_BUFFER_TOTAL_SIZE ];
    m_dwPointsToFlush = 0;
    
    m_rgflLinesData = new GLfloat[ 6*LINE_BUFFER_TOTAL_SIZE ];
    m_rgflLinesColorData = new GLubyte[ 8*LINE_BUFFER_TOTAL_SIZE ];
    m_dwLinesToFlush = 0;
    
    m_rgflQuadsData = new GLfloat [ 12*QUAD_BUFFER_TOTAL_SIZE ];
    m_rgflQuadsColorData = new GLubyte[ 16*QUAD_BUFFER_TOTAL_SIZE ];
    m_rgflQuadsTextureData = new GLfloat[ 8*QUAD_BUFFER_TOTAL_SIZE ];
    m_dwQuadsToFlush = 0;
    
    
    if( !BInitializeGraphics() )
    {
        OutputDebugString( "!! Initializing graphics failed\n" );
        return;
    }
    
    if ( !BInitializeAudio() )
    {
        OutputDebugString( "!! Initializing audio failed\n" );
        return;
    }
    
    m_bEngineReadyForUse = true;
}


//-----------------------------------------------------------------------------
// Purpose: Shutdown the game engine
//-----------------------------------------------------------------------------
void CGameEngineGL::Shutdown()
{
    // Flag that we are shutting down so the frame loop will stop running
    m_bShuttingDown = true;
    
    
    if ( m_rgflPointsData )
    {
        delete[] m_rgflPointsData;
        m_rgflPointsData = NULL;
    }
    
    if ( m_rgflPointsColorData )
    {
        delete[] m_rgflPointsColorData;
        m_rgflPointsColorData = NULL;
    }
    
    if ( m_rgflLinesData )
    {
        delete[] m_rgflLinesData;
        m_rgflLinesData = NULL;
    }
    
    if ( m_rgflLinesColorData )
    {
        delete[] m_rgflLinesColorData;
        m_rgflLinesColorData = NULL;
    }
    
    if ( m_rgflQuadsData )
    {
        delete[] m_rgflQuadsData;
        m_rgflQuadsData = NULL;
    }
    
    if ( m_rgflQuadsColorData )
    {
        delete[] m_rgflQuadsColorData;
        m_rgflQuadsColorData = NULL;
    }
    
    if ( m_rgflQuadsTextureData )
    {
        delete[] m_rgflQuadsTextureData;
        m_rgflQuadsTextureData = NULL;
    }
    
    std::map<std::string, GLString *>::const_iterator i;
    for (i = m_MapStrings.begin(); i != m_MapStrings.end(); ++i)
    {
        [i->second release];
    }
    
    m_MapStrings.clear();
    
    m_dwLinesToFlush = 0;
    m_dwPointsToFlush = 0;
    m_dwQuadsToFlush = 0;
}


//-----------------------------------------------------------------------------
// Purpose: Initialize voice/audio interfaces
//-----------------------------------------------------------------------------
bool CGameEngineGL::BInitializeAudio()
{
    m_palDevice = alcOpenDevice(NULL);
    if ( m_palDevice )
    {
        m_palContext = alcCreateContext( m_palDevice, NULL );
        alcMakeContextCurrent( m_palContext );
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// Purpose: Initialize the GL rendering interfaces and default state
//-----------------------------------------------------------------------------
#define D3DADAPTER_DEFAULT 0

bool CGameEngineGL::BInitializeGraphics()
{
    m_nWindowWidth = 1024;
    m_nWindowHeight = 768;
    
    AutoReleasePool pool;
    
    ProcessSerialNumber psn = { 0, kCurrentProcess };
    TransformProcessType( &psn, kProcessTransformToForegroundApplication );
    SetFrontProcess( &psn );
    
    uint32_t mask = NSTitledWindowMask | NSClosableWindowMask| NSResizableWindowMask;
    
    GLuint attribs[] =
    {
        NSOpenGLPFAAccelerated,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFANoRecovery,
        NSOpenGLPFABackingStore,
        NSOpenGLPFAColorSize, 32,
        NSOpenGLPFADepthSize, 16,
        0
    };
    
    NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes: (NSOpenGLPixelFormatAttribute*) attribs];
    
    NSApplication *app = [GLApplication sharedApplication];
    
    NSApplicationLoad();
    
    m_view = [[NSOpenGLView alloc] initWithFrame:NSMakeRect( 0, 0, m_nWindowWidth, m_nWindowHeight )
                                     pixelFormat:pixelFormat];
    
    
    int wherex = 50;
    int wherey = 50;
    m_window = [[NSWindow alloc] initWithContentRect:NSMakeRect( wherex, wherey, m_nWindowWidth, m_nWindowHeight )
                                           styleMask:mask
                                             backing:NSBackingStoreBuffered
                                               defer:NO];
    
    [m_window setAcceptsMouseMovedEvents:YES];
    
    // Register to be notified when the window closes so we can shutdown
    [[NSNotificationCenter defaultCenter] addObserver:app
                                             selector:@selector(windowWillClose:)
                                                 name:NSWindowWillCloseNotification
                                               object:m_window];
    
    GLint swapInt = 1;
    [[m_view openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    [m_window setContentView:m_view];
    [m_view release];
    
    [m_window makeKeyAndOrderFront:nil];
    // [m_view setPostsFrameChangedNotifications:YES];
    
    // Clear any errors
    glGetError();
    
    glClearDepth( 1.0f );
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    glDisable( GL_CULL_FACE );
    glDisable( GL_ALPHA_TEST );
    glDisable( GL_STENCIL_TEST );
    glDisable( GL_SCISSOR_TEST );
    glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_FOG );
    
    glDepthMask( GL_FALSE );
    
    // We always need these two
    glEnableClientState( GL_COLOR_ARRAY );
    glEnableClientState( GL_VERTEX_ARRAY );
    
    // This we'll enable as needed
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef( 0, 0, 0 );
    
    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glTranslatef( 0, 0, 0 );
    
    glDepthRange( 0.0f, 1.0f );
    
    AdjustViewport();
    
    return true;
}

void CGameEngineGL::AdjustViewport()
{
    NSRect viewBounds = { { 0, 0 }, { static_cast<CGFloat>(m_nWindowWidth), static_cast<CGFloat>(m_nWindowHeight) } };
    
    if( !NSEqualRects( [m_view bounds], viewBounds ) )
    {
        m_nWindowWidth = [m_view bounds].size.width;
        m_nWindowHeight = [m_view bounds].size.height;
    }
    
    // Perspective
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho( 0, m_nWindowWidth, m_nWindowHeight, 0, -1.0f, 1.0f );
    glTranslatef( 0, 0, 0 );
    
    // View port has changed as well
    glMatrixMode(GL_MODELVIEW);
    
    glViewport( 0, 0, m_nWindowWidth, m_nWindowHeight );
    glScissor( 0, 0, m_nWindowWidth, m_nWindowHeight );
    
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    glFlush();
}

//-----------------------------------------------------------------------------
// Purpose: Updates current tick count for the game engine
//-----------------------------------------------------------------------------
void CGameEngineGL::UpdateGameTickCount()
{
    m_ulPreviousGameTickCount = m_ulGameTickCount;
    m_ulGameTickCount = GetTickCount();
}


//-----------------------------------------------------------------------------
// Purpose: Tell the game engine to sleep for a bit if needed to limit frame rate.  You must keep
// calling this repeatedly until it returns false.  If it returns true it's slept a little, but more
// time may be needed.
//-----------------------------------------------------------------------------
bool CGameEngineGL::BSleepForFrameRateLimit( uint32 ulMaxFrameRate )
{
    // Frame rate limiting
    float flDesiredFrameMilliseconds = 1000.0f/ulMaxFrameRate;
    
    uint64 ulGameTickCount = GetTickCount();
    
    float flMillisecondsElapsed = (float)(ulGameTickCount - m_ulGameTickCount);
    if ( flMillisecondsElapsed < flDesiredFrameMilliseconds )
    {
        // If enough time is left sleep, otherwise just keep spinning so we don't go over the limit...
        if ( flDesiredFrameMilliseconds - flMillisecondsElapsed > 3.0f )
        {
            usleep( 5000 );
        }
        else
        {
            // Just return right away so we busy loop, don't want to sleep too long and go over
        }
        
        return true;
    }
    else
    {
        return false;
    }
}


//-----------------------------------------------------------------------------
// Purpose: Set the background color to clear to
//-----------------------------------------------------------------------------
void CGameEngineGL::SetBackgroundColor( short a, short r, short g, short b )
{
    glClearColor( (float)r/255.0f, (float)g/255.0f, (float)b/255.0f, (float)a/255.0f );
}

//-----------------------------------------------------------------------------
// Purpose: Start a new frame
//-----------------------------------------------------------------------------
bool CGameEngineGL::StartFrame()
{
    AdjustViewport();
    
    // Pump system callbacks
    MessagePump();
    
    // We may now be shutting down, check and don't start a frame then
    if ( BShuttingDown() )
        return false;
    
#if 0 // for debug
    static unsigned char counter;
    counter++;
    glClearColor( (float)counter/255.0f, (float)counter/255.0f, (float)counter/255.0f, (float)1.0f );
#endif
    
    // Clear the screen for the new frame
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    
    return true;
}


//-----------------------------------------------------------------------------
// Purpose: End the current frame
//-----------------------------------------------------------------------------
void CGameEngineGL::EndFrame()
{
    if ( BShuttingDown() )
        return;
    
    // Flush point buffer
    BFlushPointBuffer();
    
    // Flush line buffer
    BFlushLineBuffer();
    
    // Flush quad buffer
    BFlushQuadBuffer();
    
    // Swap buffers now that everything is flushed
    [[m_view openGLContext] flushBuffer];
    
    RunAudio();
}

//-----------------------------------------------------------------------------
// Purpose: Draw a line, the engine internally manages a vertex buffer for batching these
//-----------------------------------------------------------------------------
bool CGameEngineGL::BDrawLine( float xPos0, float yPos0, DWORD dwColor0, float xPos1, float yPos1, DWORD dwColor1 )
{
    if ( m_bShuttingDown )
        return false;
    
    
    // Check if we are out of room and need to flush the buffer
    if ( m_dwLinesToFlush == LINE_BUFFER_TOTAL_SIZE )
    {
        BFlushLineBuffer();
    }
    
    DWORD dwOffset = m_dwLinesToFlush*6;
    m_rgflLinesData[dwOffset] = xPos0;
    m_rgflLinesData[dwOffset+1] = yPos0;
    m_rgflLinesData[dwOffset+2] = 1.0;
    m_rgflLinesData[dwOffset+3] = xPos1;
    m_rgflLinesData[dwOffset+4] = yPos1;
    m_rgflLinesData[dwOffset+5] = 1.0;
    
    dwOffset = m_dwLinesToFlush*8;
    m_rgflLinesColorData[dwOffset] = COLOR_RED( dwColor0 );
    m_rgflLinesColorData[dwOffset+1] = COLOR_GREEN( dwColor0 );
    m_rgflLinesColorData[dwOffset+2] = COLOR_BLUE( dwColor0 );
    m_rgflLinesColorData[dwOffset+3] = COLOR_ALPHA( dwColor0 );
    m_rgflLinesColorData[dwOffset+4] = COLOR_RED( dwColor1 );
    m_rgflLinesColorData[dwOffset+5] = COLOR_GREEN( dwColor1 );
    m_rgflLinesColorData[dwOffset+6] = COLOR_BLUE( dwColor1 );
    m_rgflLinesColorData[dwOffset+7] = COLOR_ALPHA( dwColor1 );
    
    ++m_dwLinesToFlush;
    
    return true;
}


//-----------------------------------------------------------------------------
// Purpose: Flush batched lines to the screen
//-----------------------------------------------------------------------------
bool CGameEngineGL::BFlushLineBuffer()
{
    
    if ( !m_rgflLinesColorData || !m_rgflLinesData || m_bShuttingDown )
        return false;
    
    if ( m_dwLinesToFlush )
    {
        glColorPointer( 4, GL_UNSIGNED_BYTE, 0, m_rgflLinesColorData );
        glVertexPointer( 3, GL_FLOAT, 0, m_rgflLinesData );
        glDrawArrays( GL_LINES, 0, m_dwLinesToFlush*2 );
        
        m_dwLinesToFlush = 0;
    }
    
    return true;
}


//-----------------------------------------------------------------------------
// Purpose: Draw a point, the engine internally manages a vertex buffer for batching these
//-----------------------------------------------------------------------------
bool CGameEngineGL::BDrawPoint( float xPos, float yPos, DWORD dwColor )
{
    if ( m_bShuttingDown )
        return false;
    
    
    
    // Check if we are out of room and need to flush the buffer
    if ( m_dwPointsToFlush == POINT_BUFFER_TOTAL_SIZE )
    {
        BFlushPointBuffer();
    }
    
    DWORD dwOffset = m_dwPointsToFlush*3;
    m_rgflPointsData[dwOffset] = xPos;
    m_rgflPointsData[dwOffset+1] = yPos;
    m_rgflPointsData[dwOffset+2] = 1.0;
    
    dwOffset = m_dwPointsToFlush*4;
    m_rgflPointsColorData[dwOffset] = COLOR_RED( dwColor );
    m_rgflPointsColorData[dwOffset+1] = COLOR_GREEN( dwColor );
    m_rgflPointsColorData[dwOffset+2] = COLOR_BLUE( dwColor );
    m_rgflPointsColorData[dwOffset+3] = COLOR_ALPHA( dwColor );
    
    ++m_dwPointsToFlush;
    
    return true;
}


//-----------------------------------------------------------------------------
// Purpose: Flush batched points to the screen
//-----------------------------------------------------------------------------
bool CGameEngineGL::BFlushPointBuffer()
{
    
    if ( !m_rgflPointsColorData || !m_rgflPointsData || m_bShuttingDown )
        return false;
    
    if ( m_dwPointsToFlush )
    {
        glColorPointer( 4, GL_UNSIGNED_BYTE, 0, m_rgflPointsColorData );
        glVertexPointer( 3, GL_FLOAT, 0, m_rgflPointsData );
        glDrawArrays( GL_POINTS, 0, m_dwPointsToFlush );
        
        m_dwPointsToFlush = 0;
    }
    
    return true;
}


//-----------------------------------------------------------------------------
// Purpose: Draw a filled quad
//-----------------------------------------------------------------------------
bool CGameEngineGL::BDrawFilledRect( float xPos0, float yPos0, float xPos1, float yPos1, DWORD dwColor )
{
    
    if ( !m_hTextureWhite )
    {
        byte *pRGBAData = new byte[ 1 * 1 * 4 ];
        memset( pRGBAData, 255, 1*1*4 );
        m_hTextureWhite = HCreateTexture( pRGBAData, 1, 1 );
        delete[] pRGBAData;
    }
    
    return BDrawTexturedRect( xPos0, yPos0, xPos1, yPos1, 0.0f, 0.0f, 1.0f, 1.0f, dwColor, m_hTextureWhite );
}


//-----------------------------------------------------------------------------
// Purpose: Draw a textured rect
//-----------------------------------------------------------------------------
bool CGameEngineGL::BDrawTexturedRect( float xPos0, float yPos0, float xPos1, float yPos1, float u0, float v0, float u1, float v1, DWORD dwColor, HGAMETEXTURE hTexture )
{
    if ( m_bShuttingDown )
        return false;
    
    
    // Find the texture
    std::map<HGAMETEXTURE, TextureData_t>::iterator iter;
    iter = m_MapTextures.find( hTexture );
    if ( iter == m_MapTextures.end() )
    {
        OutputDebugString( "BDrawTexturedQuad called with invalid hTexture value\n" );
        return false;
    }
    
    // Check if we are out of room and need to flush the buffer, or if our texture is changing
    // then we also need to flush the buffer.
    if ( m_dwQuadsToFlush == QUAD_BUFFER_TOTAL_SIZE || m_hLastTexture != hTexture )
    {
        BFlushQuadBuffer();
    }
    
    // Bind the new texture
    glBindTexture( GL_TEXTURE_2D, iter->second.m_uTextureID );
    
    DWORD dwOffset = m_dwQuadsToFlush*12;
    m_rgflQuadsData[dwOffset] = xPos0;
    m_rgflQuadsData[dwOffset+1] = yPos0;
    m_rgflQuadsData[dwOffset+2] = 1.0;
    m_rgflQuadsData[dwOffset+3] = xPos1;
    m_rgflQuadsData[dwOffset+4] = yPos0;
    m_rgflQuadsData[dwOffset+5] = 1.0;
    m_rgflQuadsData[dwOffset+6] = xPos1;
    m_rgflQuadsData[dwOffset+7] = yPos1;
    m_rgflQuadsData[dwOffset+8] = 1.0;
    m_rgflQuadsData[dwOffset+9] = xPos0;
    m_rgflQuadsData[dwOffset+10] = yPos1;
    m_rgflQuadsData[dwOffset+11] = 1.0;
    
    dwOffset = m_dwQuadsToFlush*16;
    m_rgflQuadsColorData[dwOffset] = COLOR_RED( dwColor );
    m_rgflQuadsColorData[dwOffset+1] = COLOR_GREEN( dwColor );
    m_rgflQuadsColorData[dwOffset+2] = COLOR_BLUE( dwColor );
    m_rgflQuadsColorData[dwOffset+3] = COLOR_ALPHA( dwColor );
    m_rgflQuadsColorData[dwOffset+4] = COLOR_RED( dwColor );
    m_rgflQuadsColorData[dwOffset+5] = COLOR_GREEN( dwColor );
    m_rgflQuadsColorData[dwOffset+6] = COLOR_BLUE( dwColor );
    m_rgflQuadsColorData[dwOffset+7] = COLOR_ALPHA( dwColor );
    m_rgflQuadsColorData[dwOffset+8] = COLOR_RED( dwColor );
    m_rgflQuadsColorData[dwOffset+9] = COLOR_GREEN( dwColor );
    m_rgflQuadsColorData[dwOffset+10] = COLOR_BLUE( dwColor );
    m_rgflQuadsColorData[dwOffset+11] = COLOR_ALPHA( dwColor );
    m_rgflQuadsColorData[dwOffset+12] = COLOR_RED( dwColor );
    m_rgflQuadsColorData[dwOffset+13] = COLOR_GREEN( dwColor );
    m_rgflQuadsColorData[dwOffset+14] = COLOR_BLUE( dwColor );
    m_rgflQuadsColorData[dwOffset+15] = COLOR_ALPHA( dwColor );
    
    dwOffset = m_dwQuadsToFlush*8;
    m_rgflQuadsTextureData[dwOffset] = u0;
    m_rgflQuadsTextureData[dwOffset+1] = v0;
    m_rgflQuadsTextureData[dwOffset+2] = u1;
    m_rgflQuadsTextureData[dwOffset+3] = v0;
    m_rgflQuadsTextureData[dwOffset+4] = u1;
    m_rgflQuadsTextureData[dwOffset+5] = v1;
    m_rgflQuadsTextureData[dwOffset+6] = u0;
    m_rgflQuadsTextureData[dwOffset+7] = v1;
    
    
    ++m_dwQuadsToFlush;
    
    return true;
}

//-----------------------------------------------------------------------------
// Purpose: Draw a textured rect
//-----------------------------------------------------------------------------
bool CGameEngineGL::BDrawTexturedQuad( float xPos0, float yPos0, float xPos1, float yPos1, float xPos2, float yPos2, float xPos3, float yPos3,
                                      float u0, float v0, float u1, float v1, DWORD dwColor, HGAMETEXTURE hTexture )
{
    if ( m_bShuttingDown )
        return false;
    
    
    // Find the texture
    std::map<HGAMETEXTURE, TextureData_t>::iterator iter;
    iter = m_MapTextures.find( hTexture );
    if ( iter == m_MapTextures.end() )
    {
        OutputDebugString( "BDrawTexturedQuad called with invalid hTexture value\n" );
        return false;
    }
    
    // Check if we are out of room and need to flush the buffer, or if our texture is changing
    // then we also need to flush the buffer.
    if ( m_dwQuadsToFlush == QUAD_BUFFER_TOTAL_SIZE || m_hLastTexture != hTexture )
    {
        BFlushQuadBuffer();
    }
    
    // Bind the new texture
    glBindTexture( GL_TEXTURE_2D, iter->second.m_uTextureID );
    
    DWORD dwOffset = m_dwQuadsToFlush*12;
    m_rgflQuadsData[dwOffset] = xPos0;
    m_rgflQuadsData[dwOffset+1] = yPos0;
    m_rgflQuadsData[dwOffset+2] = 1.0;
    m_rgflQuadsData[dwOffset+3] = xPos1;
    m_rgflQuadsData[dwOffset+4] = yPos1;
    m_rgflQuadsData[dwOffset+5] = 1.0;
    m_rgflQuadsData[dwOffset+6] = xPos2;
    m_rgflQuadsData[dwOffset+7] = yPos2;
    m_rgflQuadsData[dwOffset+8] = 1.0;
    m_rgflQuadsData[dwOffset+9] = xPos3;
    m_rgflQuadsData[dwOffset+10] = yPos3;
    m_rgflQuadsData[dwOffset+11] = 1.0;
    
    dwOffset = m_dwQuadsToFlush*16;
    m_rgflQuadsColorData[dwOffset] = COLOR_RED( dwColor );
    m_rgflQuadsColorData[dwOffset+1] = COLOR_GREEN( dwColor );
    m_rgflQuadsColorData[dwOffset+2] = COLOR_BLUE( dwColor );
    m_rgflQuadsColorData[dwOffset+3] = COLOR_ALPHA( dwColor );
    m_rgflQuadsColorData[dwOffset+4] = COLOR_RED( dwColor );
    m_rgflQuadsColorData[dwOffset+5] = COLOR_GREEN( dwColor );
    m_rgflQuadsColorData[dwOffset+6] = COLOR_BLUE( dwColor );
    m_rgflQuadsColorData[dwOffset+7] = COLOR_ALPHA( dwColor );
    m_rgflQuadsColorData[dwOffset+8] = COLOR_RED( dwColor );
    m_rgflQuadsColorData[dwOffset+9] = COLOR_GREEN( dwColor );
    m_rgflQuadsColorData[dwOffset+10] = COLOR_BLUE( dwColor );
    m_rgflQuadsColorData[dwOffset+11] = COLOR_ALPHA( dwColor );
    m_rgflQuadsColorData[dwOffset+12] = COLOR_RED( dwColor );
    m_rgflQuadsColorData[dwOffset+13] = COLOR_GREEN( dwColor );
    m_rgflQuadsColorData[dwOffset+14] = COLOR_BLUE( dwColor );
    m_rgflQuadsColorData[dwOffset+15] = COLOR_ALPHA( dwColor );
    
    dwOffset = m_dwQuadsToFlush*8;
    m_rgflQuadsTextureData[dwOffset] = u0;
    m_rgflQuadsTextureData[dwOffset+1] = v0;
    m_rgflQuadsTextureData[dwOffset+2] = u1;
    m_rgflQuadsTextureData[dwOffset+3] = v0;
    m_rgflQuadsTextureData[dwOffset+4] = u1;
    m_rgflQuadsTextureData[dwOffset+5] = v1;
    m_rgflQuadsTextureData[dwOffset+6] = u0;
    m_rgflQuadsTextureData[dwOffset+7] = v1;
    
    
    ++m_dwQuadsToFlush;
    
    return true;
}


//-----------------------------------------------------------------------------
// Purpose: Flush buffered quads
//-----------------------------------------------------------------------------
bool CGameEngineGL::BFlushQuadBuffer()
{
    
    if ( !m_rgflPointsColorData || !m_rgflPointsData || m_bShuttingDown )
        return false;
    
    if ( m_dwQuadsToFlush )
    {
        glEnable( GL_TEXTURE_2D );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        
        glColorPointer( 4, GL_UNSIGNED_BYTE, 0, m_rgflQuadsColorData );
        glVertexPointer( 3, GL_FLOAT, 0, m_rgflQuadsData );
        glTexCoordPointer( 2, GL_FLOAT, 0, m_rgflQuadsTextureData );
        glDrawArrays( GL_QUADS, 0, m_dwQuadsToFlush*4 );
        
        glDisable( GL_TEXTURE_2D );
        glDisableClientState( GL_TEXTURE_COORD_ARRAY );
        
        m_dwQuadsToFlush = 0;
    }
    
    return true;
}


//-----------------------------------------------------------------------------
// Purpose: Creates a new texture
//-----------------------------------------------------------------------------
HGAMETEXTURE CGameEngineGL::HCreateTexture( byte *pRGBAData, uint32 uWidth, uint32 uHeight, ETEXTUREFORMAT eTextureFormat )
{
    
    if ( m_bShuttingDown )
        return 0;
    
    TextureData_t TexData;
    TexData.m_uWidth = uWidth;
    TexData.m_uHeight = uHeight;
    TexData.m_uTextureID = 0;
    
    glEnable( GL_TEXTURE_2D );
    glGenTextures( 1, &TexData.m_uTextureID );
    glBindTexture( GL_TEXTURE_2D, TexData.m_uTextureID );
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0 );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    
    // build our texture mipmaps
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, uWidth, uHeight, 0, eTextureFormat == eTextureFormat_RGBA ? GL_RGBA : GL_BGRA, GL_UNSIGNED_BYTE, (void *)pRGBAData );
    glDisable( GL_TEXTURE_2D );
    
    int nHandle = m_nNextTextureHandle;
    ++m_nNextTextureHandle;
    m_MapTextures[nHandle] = TexData;
    
    return nHandle;
}


//-----------------------------------------------------------------------------
// Purpose: update an exiting textue
//-----------------------------------------------------------------------------
bool CGameEngineGL::UpdateTexture( HGAMETEXTURE texture, byte *pRGBAData, uint32 uWidth, uint32 uHeight, ETEXTUREFORMAT eTextureFormat )
{
    if ( m_bShuttingDown )
        return false;
    
    std::map<HGAMETEXTURE, TextureData_t>::iterator iter;
    iter = m_MapTextures.find( texture );
    if ( iter == m_MapTextures.end() )
    {
        OutputDebugString( "BDrawTexturedQuad called with invalid hTexture value\n" );
        return false;
    }
    
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, iter->second.m_uTextureID );
    
    // build our texture mipmaps
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, uWidth, uHeight, 0, eTextureFormat == eTextureFormat_RGBA ? GL_RGBA : GL_BGRA, GL_UNSIGNED_BYTE, (void *)pRGBAData );
    glDisable( GL_TEXTURE_2D );
    
    return true;
}


//-----------------------------------------------------------------------------
// Purpose: Creates a new font
//-----------------------------------------------------------------------------
HGAMEFONT CGameEngineGL::HCreateFont( int nHeight, int nFontWeight, bool bItalic, const char * pchFont )
{
    
    AutoReleasePool pool;
    
    HGAMEFONT hFont = m_nNextFontHandle;
    ++m_nNextFontHandle;
    
    NSString *fontName = [NSString stringWithUTF8String:pchFont];
    
    NSFont *font = [NSFont fontWithName: fontName size: nHeight];
    [font retain];
    
    m_MapGameFonts[ hFont ] = font;
    
    return hFont;
}


//-----------------------------------------------------------------------------
// Purpose: Draws text to the screen inside the given rectangular region, using the given font
//-----------------------------------------------------------------------------
bool CGameEngineGL::BDrawString( HGAMEFONT hFont, RECT rect, DWORD dwColor, DWORD dwFormat, const char *pchText )
{
    
    if ( !hFont )
    {
        OutputDebugString( "Someone is calling BDrawString with a null font handle\n" );
        return false;
    }
    
    AutoReleasePool pool;
    
    NSFont *pFont = (NSFont*) m_MapGameFonts[ hFont ];
    NSRect box = { { static_cast<CGFloat>(rect.left), static_cast<CGFloat>(rect.top) }, { static_cast<CGFloat>(rect.right-rect.left), static_cast<CGFloat>(rect.bottom-rect.top) } };
    NSColor *pColor = [NSColor colorWithCalibratedRed:COLOR_RED(dwColor)/255.0
                                                green:COLOR_GREEN(dwColor)/255.0
                                                 blue:COLOR_BLUE(dwColor)/255.0
                                                alpha:COLOR_ALPHA(dwColor)/255.0 ];
    
    GLString *&string = m_MapStrings[ std::string(pchText) ];
    
    NSString *nsString = [NSString stringWithUTF8String:pchText];
    
    if ( string == NULL)
    {
        string = [[GLString alloc] initWithString:nsString
                                         withFont:pFont
                                    withTextColor:pColor
                                            inBox:&box
                                        withFlags:dwFormat];
    }
    else
    {
        [string setFont: pFont];
        [string setColor: pColor];
        [string setBox: &box];
    }
    
    [string drawWithBounds: box];
    
    return true;
}

void CGameEngineGL::UpdateKey( uint32_t vkKey, int nDown )
{
    if ( nDown )
        m_SetKeysDown.insert( vkKey );
    else
        m_SetKeysDown.erase( vkKey );
}

//-----------------------------------------------------------------------------
// Purpose: Message pump for OS messages
//-----------------------------------------------------------------------------
void CGameEngineGL::MessagePump()
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSApplication *pApp = [NSApplication sharedApplication];
    do
    {
        NSEvent *event = [pApp nextEventMatchingMask:NSAnyEventMask untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES];
        if ( event == nil )
            break;
        
        // fprintf( stderr, ": %s\n", [[event description] UTF8String] );
        
        uint32_t c = 0;
        switch ( [event type] )
        {
            case NSKeyDown:
            case NSKeyUp:
                c = [[event charactersIgnoringModifiers] characterAtIndex:0];
                
                switch ( c )
            {
                case NSUpArrowFunctionKey:
                    c = VK_UP;
                    break;
                case NSDownArrowFunctionKey:
                    c = VK_DOWN;
                    break;
                case NSLeftArrowFunctionKey:
                    c = VK_LEFT;
                    break;
                case NSRightArrowFunctionKey:
                    c = VK_RIGHT;
                    break;
                case 127: // on mac 'del' is backspace
                    c = VK_BACK;
                    break;
            }
                
                c = toupper(c);
                
                if ( [event type] == NSKeyDown )
                    m_SetKeysDown.insert( c );
                else
                    m_SetKeysDown.erase( c );
                continue;
                
            case NSFlagsChanged:
                c = [event modifierFlags];
                UpdateKey( VK_SHIFT, c & NSShiftKeyMask );
                UpdateKey( VK_CONTROL, c & NSControlKeyMask );
                UpdateKey( VK_SELECT, c & NSAlternateKeyMask );
                continue;
                
        }
        
        [pApp sendEvent:event];
        [pApp updateWindows];
    } while ( !BShuttingDown() );
    
    [pool release];
}

//-----------------------------------------------------------------------------
// Purpose: Find out if a key is currently down
//-----------------------------------------------------------------------------
bool CGameEngineGL::BIsKeyDown( DWORD dwVK )
{
    std::set<DWORD>::iterator iter;
    iter = m_SetKeysDown.find( dwVK );
    if ( iter != m_SetKeysDown.end() )
        return true;
    
    return false;
}

//-----------------------------------------------------------------------------
// Purpose: Get a down key value
//-----------------------------------------------------------------------------
bool CGameEngineGL::BGetFirstKeyDown( DWORD *pdwVK )
{
    std::set<DWORD>::iterator iter;
    iter = m_SetKeysDown.begin();
    if ( iter != m_SetKeysDown.end() )
    {
        *pdwVK = *iter;
        m_SetKeysDown.erase( iter );
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
HGAMEVOICECHANNEL CGameEngineGL::HCreateVoiceChannel()
{
    m_unVoiceChannelCount++;
    CVoiceContext* pVoiceContext = new CVoiceContext;
    
    m_MapVoiceChannel[m_unVoiceChannelCount] = pVoiceContext;
    
    return m_unVoiceChannelCount;
}

void CGameEngineGL::RunAudio()
{
    std::map<HGAMEVOICECHANNEL, CVoiceContext* >::iterator iter;
    
    for( iter = m_MapVoiceChannel.begin(); iter!=m_MapVoiceChannel.end(); ++iter)
    {
        CVoiceContext* pVoice = iter->second;
        
        const int nBufferCount =  ARRAYSIZE( pVoice->m_buffers );
        ALint nQueued, nProcessed;
        alGetSourcei( pVoice->m_nSource, AL_BUFFERS_QUEUED, &nQueued );
        alGetSourcei( pVoice->m_nSource, AL_BUFFERS_PROCESSED, &nProcessed );
        
        if ( ( nQueued == nBufferCount ) && ( nProcessed == 0 ) )
        {	// No room at the inn
            continue;
        }
        
        ALuint nBufferID;
        for ( int i = 0; i < nProcessed; i++ )
            alSourceUnqueueBuffers( pVoice->m_nSource, 1, &nBufferID );
        
        int nMaxToQueue = nBufferCount - nQueued + nProcessed;
        bool bQueued = false;
        
        while ( nMaxToQueue && !pVoice->m_pending.empty() )
        {
            Packet_t &packet = pVoice->m_pending.front();
            
            nBufferID = pVoice->m_buffers[ pVoice->m_nNextFreeBuffer ];
            alBufferData( nBufferID, AL_FORMAT_MONO16, packet.pData, packet.unSize, VOICE_OUTPUT_SAMPLE_RATE_IDEAL );
            pVoice->m_nNextFreeBuffer = (pVoice->m_nNextFreeBuffer + 1 ) % nBufferCount;
            
            alSourceQueueBuffers( pVoice->m_nSource, 1, &nBufferID);
            
            nMaxToQueue--;
            free( packet.pData );
            pVoice->m_pending.pop();
            bQueued = true;
        }
        
        if ( bQueued && ( (nQueued-nProcessed) == 0 ) )
        {
            alSourcePlay( pVoice->m_nSource );
        }
    }
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CGameEngineGL::DestroyVoiceChannel( HGAMEVOICECHANNEL hChannel )
{
    std::map<HGAMEVOICECHANNEL, CVoiceContext* >::iterator iter;
    iter = m_MapVoiceChannel.find( hChannel );
    if ( iter != m_MapVoiceChannel.end() )
    {
        CVoiceContext* pVoiceContext = iter->second;
        
        // free outstanding voice packets
        
        while( !pVoiceContext->m_pending.empty() )
        {
            free( pVoiceContext->m_pending.front().pData );
            pVoiceContext->m_pending.pop();
        }
        
        delete pVoiceContext;
        m_MapVoiceChannel.erase( iter );
    }
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CGameEngineGL::AddVoiceData( HGAMEVOICECHANNEL hChannel, const uint8 *pVoiceData, uint32 uLength )
{
    std::map<HGAMEVOICECHANNEL, CVoiceContext* >::iterator iter;
    iter = m_MapVoiceChannel.find( hChannel );
    if ( iter == m_MapVoiceChannel.end() )
        return false; // channel not found
    
    CVoiceContext* pVoiceContext = iter->second;
    
    Packet_t packet;
    
    packet.pData = malloc ( uLength );
    memcpy( packet.pData, pVoiceData, uLength );
    packet.unSize = uLength;
    
    pVoiceContext->m_pending.push( packet );
    
    return true;
}



//-----------------------------------------------------------------------------
// Purpose: Return true if there is an active Steam Controller
//-----------------------------------------------------------------------------
bool CGameEngineGL::BIsSteamControllerActive( )
{
    if ( m_ActiveControllerHandle )
    {
        return true;
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Purpose: Initialize the steam controller actions
//-----------------------------------------------------------------------------
void CGameEngineGL::InitSteamController( )
{
    // Digital game actions
    m_ControllerDigitalActionHandles[eControllerDigitalAction_TurnLeft] = SteamController( )->GetDigitalActionHandle( "turn_left" );
    m_ControllerDigitalActionHandles[eControllerDigitalAction_TurnRight] = SteamController( )->GetDigitalActionHandle( "turn_right" );
    m_ControllerDigitalActionHandles[eControllerDigitalAction_ForwardThrust] = SteamController( )->GetDigitalActionHandle( "forward_thrust" );
    m_ControllerDigitalActionHandles[eControllerDigitalAction_ReverseThrust] = SteamController( )->GetDigitalActionHandle( "backward_thrust" );
    m_ControllerDigitalActionHandles[eControllerDigitalAction_FireLasers] = SteamController( )->GetDigitalActionHandle( "fire_lasers" );
    m_ControllerDigitalActionHandles[eControllerDigitalAction_PauseMenu] = SteamController( )->GetDigitalActionHandle( "pause_menu" );
    
    m_ControllerDigitalActionHandles[eControllerDigitalAction_MenuUp] = SteamController( )->GetDigitalActionHandle( "menu_up" );
    m_ControllerDigitalActionHandles[eControllerDigitalAction_MenuDown] = SteamController( )->GetDigitalActionHandle( "menu_down" );
    m_ControllerDigitalActionHandles[eControllerDigitalAction_MenuLeft] = SteamController( )->GetDigitalActionHandle( "menu_left" );
    m_ControllerDigitalActionHandles[eControllerDigitalAction_MenuRight] = SteamController( )->GetDigitalActionHandle( "menu_right" );
    m_ControllerDigitalActionHandles[eControllerDigitalAction_MenuSelect] = SteamController( )->GetDigitalActionHandle( "menu_select" );
    m_ControllerDigitalActionHandles[eControllerDigitalAction_MenuCancel] = SteamController( )->GetDigitalActionHandle( "menu_cancel" );
    
    // Analog game actions
    m_ControllerAnalogActionHandles[eControllerAnalogAction_AnalogControls] = SteamController( )->GetAnalogActionHandle( "analog_controls" );
    
    // Action set handles
    m_ControllerActionSetHandles[eControllerActionSet_ShipControls] = SteamController( )->GetActionSetHandle( "ship_controls" );
    m_ControllerActionSetHandles[eControllerActionSet_MenuControls] = SteamController( )->GetActionSetHandle( "menu_controls" );
    
}

//-----------------------------------------------------------------------------
// Purpose: Find an active Steam controller
//-----------------------------------------------------------------------------
void CGameEngineGL::FindActiveSteamController( )
{
    // Use the first available steam controller for all interaction. We can call this each frame to handle
    // a controller disconnecting and a different one reconnecting. Handles are guaranteed to be unique for
    // a given controller, even across power cycles.
    
    // See how many Steam Controllers are active.
    ControllerHandle_t pHandles[STEAM_CONTROLLER_MAX_COUNT];
    int nNumActive = SteamController( )->GetConnectedControllers( pHandles );
    
    // If there's an active controller, and if we're not already using it, select the first one.
    if ( nNumActive && (m_ActiveControllerHandle != pHandles[0]) )
    {
        m_ActiveControllerHandle = pHandles[0];
    }
}


//--------------------------------------------------------------------------------------------------------------
// Purpose: For a given in-game action in a given action set, return a human-reaadable string to use as a prompt.
//--------------------------------------------------------------------------------------------------------------
const char *CGameEngineGL::GetTextStringForControllerOriginDigital( ECONTROLLERACTIONSET dwActionSet, ECONTROLLERDIGITALACTION dwDigitalAction )
{
    EControllerActionOrigin origins[STEAM_CONTROLLER_MAX_ORIGINS];
    int nNumOrigins = SteamController( )->GetDigitalActionOrigins( m_ActiveControllerHandle, m_ControllerActionSetHandles[dwActionSet], m_ControllerDigitalActionHandles[dwDigitalAction], origins );
    
    if ( nNumOrigins )
    {
        // We should handle the case where this action is bound to multiple buttons, but
        // here we just grab the first.
        return SteamController()->GetStringForActionOrigin( origins[0] );
    }
    
    return SteamController()->GetStringForActionOrigin( k_EControllerActionOrigin_None ); // Return "None"
}

//--------------------------------------------------------------------------------------------------------------
// Purpose: For a given in-game action in a given action set, return a human-reaadable string to use as a prompt.
//--------------------------------------------------------------------------------------------------------------
const char *CGameEngineGL::GetTextStringForControllerOriginAnalog( ECONTROLLERACTIONSET dwActionSet, ECONTROLLERANALOGACTION dwDigitalAction )
{
    EControllerActionOrigin origins[STEAM_CONTROLLER_MAX_ORIGINS];
    int nNumOrigins = SteamController( )->GetAnalogActionOrigins( m_ActiveControllerHandle, m_ControllerActionSetHandles[dwActionSet], m_ControllerDigitalActionHandles[dwDigitalAction], origins );
    
    if ( nNumOrigins )
    {
        // We should handle the case where this action is bound to multiple buttons, but
        // here we just grab the first.
        return SteamController()->GetStringForActionOrigin( origins[0] );
    }
    
    return SteamController()->GetStringForActionOrigin( k_EControllerActionOrigin_None ); // Return "None"
}

//-----------------------------------------------------------------------------
// Purpose: Called each frame
//-----------------------------------------------------------------------------
void CGameEngineGL::PollSteamController( )
{
    // There's a bug where the action handles aren't non-zero until a config is done loading. Soon config
    // information will be available immediately. Until then try to init as long as the handles are invalid.
    if ( m_ControllerDigitalActionHandles[eControllerDigitalAction_TurnLeft] == 0 )
    {
        InitSteamController( );
        return;
    }
    
    // Each frame check our active controller handle
    FindActiveSteamController( );
    
}

//-----------------------------------------------------------------------------
// Purpose: Set the LED color on the controller, if supported by controller
//-----------------------------------------------------------------------------
void CGameEngineGL::SetControllerColor( uint8 nColorR, uint8 nColorG, uint8 nColorB, unsigned int nFlags )
{
    SteamController()->SetLEDColor( m_ActiveControllerHandle, nColorR, nColorG, nColorB, nFlags );
}

//-----------------------------------------------------------------------------
// Purpose: Trigger vibration on the controller, if supported by controller
//-----------------------------------------------------------------------------
void CGameEngineGL::TriggerControllerVibration( unsigned short nLeftSpeed, unsigned short nRightSpeed )
{
    SteamController()->TriggerVibration( m_ActiveControllerHandle, nLeftSpeed, nRightSpeed );
}

//-----------------------------------------------------------------------------
// Purpose: Trigger haptics on the controller, if supported by controller
//-----------------------------------------------------------------------------
void CGameEngineGL::TriggerControllerHaptics( ESteamControllerPad ePad, unsigned short usOnMicroSec, unsigned short usOffMicroSec, unsigned short usRepeat )
{
    SteamController()->TriggerRepeatedHapticPulse( m_ActiveControllerHandle, ePad, usOnMicroSec, usOffMicroSec, usRepeat, 0 );
}

//-----------------------------------------------------------------------------
// Purpose: Find out if a controller event is currently active
//-----------------------------------------------------------------------------
bool CGameEngineGL::BIsControllerActionActive( ECONTROLLERDIGITALACTION dwAction )
{
    ControllerDigitalActionData_t digitalData = SteamController( )->GetDigitalActionData( m_ActiveControllerHandle, m_ControllerDigitalActionHandles[dwAction] );
    
    // Actions are only 'active' when they're assigned to a control in an action set, and that action set is active.
    if ( digitalData.bActive )
        return digitalData.bState;
    
    return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Purpose: Get the current x,y state of the analog action. Examples of an analog action are a virtual joystick on the trackpad or the real joystick.
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CGameEngineGL::GetControllerAnalogAction( ECONTROLLERANALOGACTION dwAction, float *x, float *y )
{
    ControllerAnalogActionData_t analogData = SteamController( )->GetAnalogActionData( m_ActiveControllerHandle, m_ControllerAnalogActionHandles[dwAction] );
    
    // Actions are only 'active' when they're assigned to a control in an action set, and that action set is active.
    if ( analogData.bActive )
    {
        *x = analogData.x;
        *y = analogData.y;
    }
    else
    {
        *x = 0.0f;
        *y = 0.0f;
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// Purpose: Put the controller into a specific action set. Action sets are collections of game-context actions ie "walking", "flying" or "menu"
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CGameEngineGL::SetSteamControllerActionSet( ECONTROLLERACTIONSET dwActionSet )
{
    if ( m_ActiveControllerHandle == 0 )
        return;
    
    // This call is low-overhead and can be called repeatedly from game code that is active in a specific mode.
    SteamController( )->ActivateActionSet( m_ActiveControllerHandle, m_ControllerActionSetHandles[dwActionSet] );
}

