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
    [window setTitle:@"Rotate 3D Color Triangle and 3D Color Square PERSPECTIVE OGL"];
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
    
    GLuint vao_pyramid;
    GLuint vao_cube;

    GLuint vbo_pyramid_position;
    GLuint vbo_pyramid_color;
    
    GLuint vbo_cube_position;
    GLuint vbo_cube_color;

    GLuint mvpUniform;

    GLfloat anglePyramid;
    GLfloat angleCube;
    
    vmath::mat4 PerspectiveProjectionMatrix;
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
    "in vec4 vColor;"\
    "out vec4 out_color;"\
    "uniform mat4 u_mvp_matrix;" \
    "void main(void)" \
    "{" \
        "gl_Position = u_mvp_matrix * vPosition;" \
        "out_color = vColor;"\
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
   "in vec4 out_color;"\
    "out vec4 FragColor;"\
    "void main(void)"\
    "{"\
        "FragColor = out_color;"\
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
    
    //PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITTH COLOR SHADER ATTRIBUTE
    glBindAttribLocation(shaderProgramObject, VDG_ATTRIBUTE_COLOR, "vColor");

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
    
    // *** vertices, colors, shader attribs, vbo, vao initializations ***
    const GLfloat pyramidvertice[] =
    { 
        // Front face
         0.0f,   1.0f,  0.0f, 
        -1.0f, -1.0f,   1.0f, 
         1.0f, - 1.0f , 1.0f,

        //Back Face
         0.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,

        //Right face
         0.0f, 1.0f, 0.0f,
         1.0f, -1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,

        //Left face
         0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f
    };
    const GLfloat pyramidcolor[]=
    { 
        //FRONT FACE
        1.0f, 0.0f, 0.0f,
        0.0f , 1.0f,0.0f,
        0.0f, 0.0f, 1.0f,

        //Back Face
        1.0f, 0.0f, 0.0f,
        0.0f , 1.0f,0.0f,
        0.0f, 0.0f, 1.0f,

        //Right face
        1.0f, 0.0f, 0.0f,
        0.0f , 0.0f,1.0f,
        0.0f, 1.0f, 0.0f,

        //Left Face
        1.0f, 0.0f, 0.0f,
        0.0f , 0.0f,1.0f,
        0.0f, 1.0f, 0.0f
    };



    const GLfloat cubevertices[] =
    {
      // top surface
        1.0f, 1.0f,-1.0f,  // top-right of top
        -1.0f, 1.0f,-1.0f, // top-left of top
        -1.0f, 1.0f, 1.0f, // bottom-left of top
        1.0f, 1.0f, 1.0f,  // bottom-right of top
        
        // bottom surface
        1.0f,-1.0f, 1.0f,  // top-right of bottom
        -1.0f,-1.0f, 1.0f, // top-left of bottom
        -1.0f,-1.0f,-1.0f, // bottom-left of bottom
        1.0f,-1.0f,-1.0f,  // bottom-right of bottom
        
        // front surface
        1.0f, 1.0f, 1.0f,  // top-right of front
        -1.0f, 1.0f, 1.0f, // top-left of front
        -1.0f,-1.0f, 1.0f, // bottom-left of front
        1.0f,-1.0f, 1.0f,  // bottom-right of front
        
        // back surface
        1.0f,-1.0f,-1.0f,  // top-right of back
        -1.0f,-1.0f,-1.0f, // top-left of back
        -1.0f, 1.0f,-1.0f, // bottom-left of back
        1.0f, 1.0f,-1.0f,  // bottom-right of back
        
        // left surface
        -1.0f, 1.0f, 1.0f, // top-right of left
        -1.0f, 1.0f,-1.0f, // top-left of left
        -1.0f,-1.0f,-1.0f, // bottom-left of left
        -1.0f,-1.0f, 1.0f, // bottom-right of left
        
        // right surface
        1.0f, 1.0f,-1.0f,  // top-right of right
        1.0f, 1.0f, 1.0f,  // top-left of right
        1.0f,-1.0f, 1.0f,  // bottom-left of right
        1.0f,-1.0f,-1.0f,  // bottom-right of right

    };
    const GLfloat cubecolor[] =
    {
        //Front Color
        0.39f, 0.58f, 0.93f,
        0.39f, 0.58f, 0.93f,
        0.39f, 0.58f, 0.93f,
        0.39f, 0.58f, 0.93f,

        //Back Color
        0.39f, 0.58f, 0.93f,
        0.39f, 0.58f, 0.93f,
        0.39f, 0.58f, 0.93f,
        0.39f, 0.58f, 0.93f,

        //Left Color
        0.39f, 0.58f, 0.93f,
        0.39f, 0.58f, 0.93f,
        0.39f, 0.58f, 0.93f,
        0.39f, 0.58f, 0.93f,

        //Right Color
        0.39f, 0.58f, 0.93f,
        0.39f, 0.58f, 0.93f,
        0.39f, 0.58f, 0.93f,
        0.39f, 0.58f, 0.93f,

        //Top Bottom
        0.39f, 0.58f, 0.93f,
        0.39f, 0.58f, 0.93f,
        0.39f, 0.58f, 0.93f,
        0.39f, 0.58f, 0.93f,


        //Bottom Color
        0.39f, 0.58f, 0.93f,
        0.39f, 0.58f, 0.93f,
        0.39f, 0.58f, 0.93f,
        0.39f, 0.58f, 0.93f
    };

    
    /*TRIANGLE*/
    glGenVertexArrays(1, &vao_pyramid);
    glBindVertexArray(vao_pyramid);
    
    /*POSITION*/
    glGenBuffers(1, &vbo_pyramid_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidvertice), pyramidvertice, GL_STATIC_DRAW);
    
    glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*END OF POSITION*/


     /*COLOR*/
    glGenBuffers(1, &vbo_pyramid_color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidcolor), pyramidcolor, GL_STATIC_DRAW);
    
    glVertexAttribPointer(VDG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    glEnableVertexAttribArray(VDG_ATTRIBUTE_COLOR);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*END OF POSITION*/

   
    glBindVertexArray(0);
    /***/


     /*SQUARE*/
    glGenVertexArrays(1, &vao_cube);
    glBindVertexArray(vao_cube);
    
    /*POSITION*/
    glGenBuffers(1, &vbo_cube_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubevertices), cubevertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*END OF SQUARE*/

     /*COLOR*/
    glGenBuffers(1, &vbo_cube_color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubecolor), cubecolor, GL_STATIC_DRAW);
    
    glVertexAttribPointer(VDG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    glEnableVertexAttribArray(VDG_ATTRIBUTE_COLOR);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*END OF COLOR*/

   
    glBindVertexArray(0);
    /***/




    
    glClearDepth(1.0f);
    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    // depth test to do
    glDepthFunc(GL_LEQUAL);
    // We will always cull back faces for better performance
    glEnable(GL_CULL_FACE);
    
    // set background color
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 

    //glShadeModel(GL_SMOOTH);

    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    // set projection matrix to identity matrix
    PerspectiveProjectionMatrix = vmath::mat4::identity();
    
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink,&MyDisplayLinkCallback,self);
    CGLContextObj cglContext=(CGLContextObj)[[self openGLContext]CGLContextObj];
    CGLPixelFormatObj cglPixelFormat=(CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink,cglContext,cglPixelFormat);
    CVDisplayLinkStart(displayLink);
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
    
   
    PerspectiveProjectionMatrix =vmath::perspective(45.05f,width /height, 0.1f, 100.0f);

    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (void)drawRect:(NSRect)dirtyRect
{
    // code
    [self update];
    [self drawView];

}

-(void) update
{
    if (anglePyramid >= 360.0f) anglePyramid = 0.0f;
    anglePyramid = anglePyramid + 0.5f;

    if (angleCube >= 360) angleCube = 0.0f;
    angleCube = angleCube + 0.5f;

}

- (void)drawView
{
    // code
    [[self openGLContext]makeCurrentContext];
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // start using OpenGL program object
    glUseProgram(shaderProgramObject);
    

    /********** DRAWING TRIAGNLE ***********************/
    // OpenGL Drawing
    // set modelview & modelviewprojection matrices to identity
    vmath::mat4 modelViewMatrix = vmath::mat4::identity();
    vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();
    vmath::mat4 rotateMatrix = vmath::mat4::identity();

    modelViewMatrix = vmath::translate(-1.5f, 0.0f, -6.0f);
    
    // multiply the modelview and orthographic matrix to get modelviewprojection matrix
    modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix; // ORDER IS IMPORTANT
    

    rotateMatrix = vmath::rotate(anglePyramid, 0.0f, 1.0f, 0.0f); // rotating around Y - axis
    modelViewProjectionMatrix = modelViewProjectionMatrix * rotateMatrix;

    // pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variable
    // whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    
    // *** bind vao_pyramid ***
    glBindVertexArray(vao_pyramid);
    
    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_TRIANGLES, 0, 12); // 3 (each with its x,y,z ) vertices in triangleVertices array
    
    // *** unbind vao_triangle ***
    glBindVertexArray(0);

    /***** END OF DRAWING TRIANGLE ****************/


     /********** DRAWING SQUARE ***********************/
    // OpenGL Drawing
    // set modelview & modelviewprojection matrices to identity
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();
    rotateMatrix = vmath::mat4::identity();


    modelViewMatrix = vmath::translate(1.5f, 0.0f, -8.0f);
    
    // multiply the modelview and orthographic matrix to get modelviewprojection matrix
    modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix; // ORDER IS IMPORTANT
    

    rotateMatrix = vmath::rotate(angleCube,angleCube,angleCube); // rotating around X - axis
    modelViewProjectionMatrix = modelViewProjectionMatrix * rotateMatrix;


    // pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variable
    // whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    
    // *** bind vao_triangle ***
    glBindVertexArray(vao_cube);
    
    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 16,4);
    glDrawArrays(GL_TRIANGLE_FAN, 20,4);
    // *** unbind vao_triangle ***
    glBindVertexArray(0);

    /***** END OF DRAWING SQUARE ****************/


    
    // stop using OpenGL program object
    glUseProgram(0);

    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    [self update];
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
    // destroy vao
    if (vao_pyramid)
    {
        glDeleteVertexArrays(1, &vao_pyramid);
        vao_pyramid = 0;
    }
    // destroy vao
    if (vao_cube)
    {
        glDeleteVertexArrays(1, &vao_cube);
        vao_cube = 0;
    }
    
    // destroy vbo_position
    if (vbo_pyramid_position)
    {
        glDeleteBuffers(1, &vbo_pyramid_position);
        vbo_pyramid_position = 0;
    }
     if (vbo_pyramid_color)
    {
        glDeleteBuffers(1, &vbo_pyramid_color);
        vbo_pyramid_color = 0;
    }
    if (vbo_cube_position)
    {
        glDeleteBuffers(1, &vbo_cube_position);
        vbo_cube_position = 0;
    }
    if (vbo_cube_color)
    {
        glDeleteBuffers(1, &vbo_cube_color);
        vbo_cube_color = 0;
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
