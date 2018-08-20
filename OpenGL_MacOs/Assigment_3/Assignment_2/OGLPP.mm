// headers
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import <QuartzCore/CVDisplayLink.h>

#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>

#import "vmath.h"

enum
{
    VDG_ATTRIBUTE_VERTEX = 0,
    VDG_ATTRIBUTE_COLOR,
    VDG_ATTRIBUTE_NORMAL,
    VDG_ATTRIBUTE_TEXTURE0,
};

// 'C' style global function declarations
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef,const CVTimeStamp *,const CVTimeStamp *,CVOptionFlags,CVOptionFlags *,void *);

// global variables
FILE *gpFile=NULL;

// interface declarations
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface GLView : NSOpenGLView
@end

// Entry-point function
int main(int argc, const char * argv[])
{
    // code
    NSAutoreleasePool *pPool=[[NSAutoreleasePool alloc]init];
    
    NSApp=[NSApplication sharedApplication];

    [NSApp setDelegate:[[AppDelegate alloc]init]];
    
    [NSApp run];
    
    [pPool release];
    
    return(0);
}

// interface implementations
@implementation AppDelegate
{
@private
    NSWindow *window;
    GLView *glView;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // code
    // log file
    NSBundle *mainBundle=[NSBundle mainBundle];
    NSString *appDirName=[mainBundle bundlePath];
    NSString *parentDirPath=[appDirName stringByDeletingLastPathComponent];
    NSString *logFileNameWithPath=[NSString stringWithFormat:@"%@/Log.txt",parentDirPath];
    const char *pszLogFileNameWithPath=[logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];
    gpFile=fopen(pszLogFileNameWithPath,"w");
    if(gpFile==NULL)
    {
        printf("Can Not Create Log File.\nExitting ...\n");
        [self release];
        [NSApp terminate:self];
    }
    fprintf(gpFile, "Program Is Started Successfully\n");
    
    // window
    NSRect win_rect;
    win_rect=NSMakeRect(0.0,0.0,800.0,600.0);
    
    // create simple window
    window=[[NSWindow alloc] initWithContentRect:win_rect
                                       styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                                         backing:NSBackingStoreBuffered
                                           defer:NO];
    [window setTitle:@"Smiley in 2-D Rectangle  in Perspective."];
    [window center];
    
    glView=[[GLView alloc]initWithFrame:win_rect];
    
    [window setContentView:glView];
    [window setDelegate:self];
    [window makeKeyAndOrderFront:self];
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
    // code
    fprintf(gpFile, "Program Is Terminated Successfully\n");
    
    if(gpFile)
    {
        fclose(gpFile);
        gpFile=NULL;
    }
}

- (void)windowWillClose:(NSNotification *)notification
{
    // code
    [NSApp terminate:self];
}

- (void)dealloc
{
    // code
    [glView release];
    
    [window release];
    
    [super dealloc];
}
@end

@implementation GLView
{
@private
    CVDisplayLinkRef displayLink;
        
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;
    GLuint shaderProgramObject;
    
    GLuint vao_rectangle_position;
   
    GLuint vbo_position;
    GLuint vbo_texture;
    GLuint texture_smiley;

    GLuint mvpUniform;
    
    GLuint pyramid_texture;
    GLuint cube_texture;

    vmath::mat4 projectionMatrix;
    
    GLuint texture_sampler_uniform;
}

-(id)initWithFrame:(NSRect)frame;
{
    // code
    self=[super initWithFrame:frame];
    
    if(self)
    {
        [[self window]setContentView:self];
        
        NSOpenGLPixelFormatAttribute attrs[]=
        {
            // Must specify the 4.1 Core Profile to use OpenGL 4.1
            NSOpenGLPFAOpenGLProfile,
            NSOpenGLProfileVersion3_2Core,
            // Specify the display ID to associate the GL context with (main display for now)
            NSOpenGLPFAScreenMask,CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
            NSOpenGLPFANoRecovery,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFAColorSize,24,
            NSOpenGLPFADepthSize,24,
            NSOpenGLPFAAlphaSize,8,
            NSOpenGLPFADoubleBuffer,
            0}; // last 0 is must
        
        NSOpenGLPixelFormat *pixelFormat=[[[NSOpenGLPixelFormat alloc]initWithAttributes:attrs] autorelease];
        
        if(pixelFormat==nil)
        {
            fprintf(gpFile, "No Valid OpenGL Pixel Format Is Available. Exitting ...");
            [self release];
            [NSApp terminate:self];
        }
        
        NSOpenGLContext *glContext=[[[NSOpenGLContext alloc]initWithFormat:pixelFormat shareContext:nil]autorelease];
        
        [self setPixelFormat:pixelFormat];
        
        [self setOpenGLContext:glContext]; // it automatically releases the older context, if present, and sets the newer one
    }
    return(self);
}

-(CVReturn)getFrameForTime:(const CVTimeStamp *)pOutputTime
{
    // code
    NSAutoreleasePool *pool=[[NSAutoreleasePool alloc]init];
    
    [self drawView];
    
    [pool release];
    return(kCVReturnSuccess);
}

-(void)prepareOpenGL
{
    // code
    // OpenGL Info
    fprintf(gpFile, "OpenGL Version  : %s\n",glGetString(GL_VERSION));
    fprintf(gpFile, "GLSL Version    : %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    [[self openGLContext]makeCurrentContext];
    
    GLint swapInt=1;
    [[self openGLContext]setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    // *** VERTEX SHADER ***
    // create shader
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    
    // provide source code to shader
    const GLchar *vertexShaderSourceCode =
    "#version 330" \
    "\n" \
    "in vec4 vPosition;" \
    "in vec2 vTexture0_Coord;" \
    "out vec2 out_texture0_coord;" \
    "uniform mat4 u_mvp_matrix;" \
    "void main(void)" \
    "{" \
        "gl_Position = u_mvp_matrix * vPosition;" \
        "out_texture0_coord=vTexture0_Coord;" \
    "}";
    glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);
    
    // compile shader
    glCompileShader(vertexShaderObject);
    GLint iInfoLogLength = 0;
    GLint iShaderCompiledStatus = 0;
    char *szInfoLog = NULL;
    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(vertexShaderObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    // *** FRAGMENT SHADER ***
    // re-initialize
    iInfoLogLength = 0;
    iShaderCompiledStatus = 0;
    szInfoLog = NULL;
    
    // create shader
    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    
    // provide source code to shader
    const GLchar *fragmentShaderSourceCode =
    "#version 330" \
    "\n" \
    "in vec2 out_texture0_coord;" \
    "uniform sampler2D u_texture0_sampler;" \
    "out vec4 FragColor;" \
    "void main(void)" \
    "{" \
         "vec3 tex=vec3(texture(u_texture0_sampler, out_texture0_coord));" \
         "FragColor=vec4(tex, 1.0f);" \
    "}";
    glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);
    
    // compile shader
    glCompileShader(fragmentShaderObject);
    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(fragmentShaderObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    // *** SHADER PROGRAM ***
    // create
    shaderProgramObject = glCreateProgram();
    
    // attach vertex shader to shader program
    glAttachShader(shaderProgramObject, vertexShaderObject);
    
    // attach fragment shader to shader program
    glAttachShader(shaderProgramObject, fragmentShaderObject);
    
    // pre-link binding of shader program object with vertex shader position attribute
    glBindAttribLocation(shaderProgramObject, VDG_ATTRIBUTE_VERTEX, "vPosition");
    glBindAttribLocation(shaderProgramObject, VDG_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");

    // link shader
    glLinkProgram(shaderProgramObject);
    GLint iShaderProgramLinkStatus = 0;
    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
    if (iShaderProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength>0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetProgramInfoLog(shaderProgramObject, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    // get MVP uniform location
    mvpUniform = glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");
    // get texture sampler uniform location
    texture_sampler_uniform=glGetUniformLocation(shaderProgramObject,"u_texture0_sampler");

    // load textures
    texture_smiley=[self loadTextureFromBMPFile:"Smiley.bmp"];
    

    // *** vertices, colors, shader attribs, vbo, vao initializations ***
    // vertices, colors, vao, vbo, tbo etc.
    const GLfloat rectangleVertices[]=
    {
        0.8f, 0.8f, 0.0f,  
        
        -0.8f, 0.8f, 0.0f,
        
        -0.8f, -0.8f, 0.0f,
        
        0.8f, -0.8f, 0.0f
    };
    
    const GLfloat rectangleCTexcocords[]=
    {
       
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    };

    
    glGenVertexArrays(1,&vao_rectangle_position);
    glBindVertexArray(vao_rectangle_position);
    
    // RECTANGLE CODE
    // vbo for position
    glGenBuffers(1,&vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_position);
    glBufferData(GL_ARRAY_BUFFER,sizeof(rectangleVertices),rectangleVertices,GL_STATIC_DRAW);
    
    glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX,3,GL_FLOAT,GL_FALSE,0,NULL); // 3 is for x,y,z in triangleVertices array
    
    glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    
    // vbo for texture
    glGenBuffers(1,&vbo_texture);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_texture);
    glBufferData(GL_ARRAY_BUFFER,sizeof(rectangleCTexcocords),rectangleCTexcocords,GL_STATIC_DRAW);
    
    glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0,2,GL_FLOAT,GL_FALSE,0,NULL); // 2 is for 's' And 't'
    
    glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    
    glBindVertexArray(0);
    // ==================
    
   
    
    glClearDepth(1.0f);
    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    // depth test to do
    glDepthFunc(GL_LEQUAL);
    // We will always cull back faces for better performance
    glEnable(GL_CULL_FACE);
    
    // enable texture
    glEnable(GL_TEXTURE_2D);
    
    // set background color
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // black
    
    // set projection matrix to identity matrix
    projectionMatrix = vmath::mat4::identity();
    
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink,&MyDisplayLinkCallback,self);
    CGLContextObj cglContext=(CGLContextObj)[[self openGLContext]CGLContextObj];
    CGLPixelFormatObj cglPixelFormat=(CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink,cglContext,cglPixelFormat);
    CVDisplayLinkStart(displayLink);
}

-(GLuint)loadTextureFromBMPFile:(const char *)texFileName
{
    NSBundle *mainBundle=[NSBundle mainBundle];
    NSString *appDirName=[mainBundle bundlePath];
    NSString *parentDirPath=[appDirName stringByDeletingLastPathComponent];
    NSString *textureFileNameWithPath=[NSString stringWithFormat:@"%@/%s",parentDirPath,texFileName];
    
    NSImage *bmpImage=[[NSImage alloc]initWithContentsOfFile:textureFileNameWithPath];
    if (!bmpImage)
    {
        NSLog(@"can't find %@", textureFileNameWithPath);
        return(0);
    }
    
    CGImageRef cgImage = [bmpImage CGImageForProposedRect:nil context:nil hints:nil];
    
    int w = (int)CGImageGetWidth(cgImage);
    int h = (int)CGImageGetHeight(cgImage);
    CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
    void* pixels = (void *)CFDataGetBytePtr(imageData);

    GLuint bmpTexture;
    glGenTextures(1, &bmpTexture);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // set 1 rather than default 4, for better performance
    glBindTexture(GL_TEXTURE_2D, bmpTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 w,
                 h,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 pixels);
    
    // Create mipmaps for this texture for better image quality
    glGenerateMipmap(GL_TEXTURE_2D);
    
    CFRelease(imageData);
    return(bmpTexture);
}

-(void)reshape
{
    // code
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    NSRect rect=[self bounds];
    
    GLfloat width=rect.size.width;
    GLfloat height=rect.size.height;

    if(height==0)
        height=1;
    
    glViewport(0,0,(GLsizei)width,(GLsizei)height);

    // perspective projection
    projectionMatrix=vmath::perspective(45.0f, width/height, 0.1f, 100.0f);

    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (void)drawRect:(NSRect)dirtyRect
{
    // code
    [self drawView];
}

- (void)drawView
{
    
    [[self openGLContext]makeCurrentContext];
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // start using OpenGL program object
    glUseProgram(shaderProgramObject);
    
    vmath::mat4 modelViewMatrix;
    vmath::mat4 modelViewProjectionMatrix;
    
    // PYRAMID CODE
    // set modelview matrix to identity matrix
    modelViewMatrix=vmath::mat4::identity();
    
    // apply z axis translation to go deep into the screen by -5.0,
    // so that triangle with same fullscreen co-ordinates, but due to above translation will look small
    modelViewMatrix=vmath::translate(0.0f, 0.0f, -3.0f);
    // multiply the modelview and projection matrix to get modelviewprojection matrix
    modelViewProjectionMatrix=projectionMatrix * modelViewMatrix; // ORDER IS IMPORTANT
    
    // pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variable
    // whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
    glUniformMatrix4fv(mvpUniform,1,GL_FALSE,modelViewProjectionMatrix);
    


    // *** bind vao ***
    glBindVertexArray(vao_rectangle_position);

    glActiveTexture(GL_TEXTURE0); // corresponds to VDG_ATTRIBUTE_TEXTURE0
    // bind with texture
    glBindTexture(GL_TEXTURE_2D,texture_smiley);

    glUniform1i(texture_sampler_uniform, 0);
    
    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_TRIANGLE_FAN,0,4); // 3 (each with its x,y,z ) vertices for 4 faces of pyramid
    
    // *** unbind vao ***
    glBindVertexArray(0);
    
    
    // stop using OpenGL program object
    glUseProgram(0);

    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
}

-(BOOL)acceptsFirstResponder
{
    // code
    [[self window]makeFirstResponder:self];
    return(YES);
}

-(void)keyDown:(NSEvent *)theEvent
{
    // code
    int key=(int)[[theEvent characters]characterAtIndex:0];
    switch(key)
    {
        case 27: // Esc key
            [ self release];
            [NSApp terminate:self];
            break;
        case 'F':
        case 'f':
            [[self window]toggleFullScreen:self];
            break;
        default:
            break;
    }
}

-(void)mouseDown:(NSEvent *)theEvent
{
    // code
}

-(void)mouseDragged:(NSEvent *)theEvent
{
    // code
}

-(void)rightMouseDown:(NSEvent *)theEvent
{
    // code
}

- (void) dealloc
{
    // code
    if(vao_rectangle_position)
    {
        glDeleteVertexArrays(1, &vao_rectangle_position);
        vao_rectangle_position=0;
    }
    
    // *** destroy vbos ***
    if(vbo_position)
    {
        glDeleteBuffers(1, &vbo_position);
        vbo_position=0;
    }
    
    if(vbo_texture)
    {
        glDeleteBuffers(1, &vbo_texture);
        vbo_texture=0;
    }
    
    // destroy texture
    if(texture_smiley)
    {
        glDeleteTextures(1,&texture_smiley);
        texture_smiley=0;
    }
    
   

    // detach vertex shader from shader program object
    glDetachShader(shaderProgramObject, vertexShaderObject);
    // detach fragment  shader from shader program object
    glDetachShader(shaderProgramObject, fragmentShaderObject);
    
    // delete vertex shader object
    glDeleteShader(vertexShaderObject);
    vertexShaderObject = 0;
    // delete fragment shader object
    glDeleteShader(fragmentShaderObject);
    fragmentShaderObject = 0;
    
    // delete shader program object
    glDeleteProgram(shaderProgramObject);
    shaderProgramObject = 0;
    
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);

    [super dealloc];
}

@end

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,const CVTimeStamp *pNow,const CVTimeStamp *pOutputTime,CVOptionFlags flagsIn,
                               CVOptionFlags *pFlagsOut,void *pDisplayLinkContext)
{
    CVReturn result=[(GLView *)pDisplayLinkContext getFrameForTime:pOutputTime];
    return(result);
}
