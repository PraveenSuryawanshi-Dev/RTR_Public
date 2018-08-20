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
    [window setTitle:@"Light and animation on Cube"];
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
    
    
    GLuint gVao_cube;

	GLuint gVbo_cube_position;

	GLuint gVbo_cube_normal;

	GLuint gModelViewMatrixUniform, gProjectionMatrixUniform;

	GLuint gLdUniform, gKdUniform, gLightPositionUniform;

	GLuint gLKeyPressedUniform;
	
	GLfloat gAngle;

	bool gbAnimate;

	bool gbLight;
    
   

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
        "#version 330 core" \
        "\n" \
        "in vec4 vPosition;" \
		"in vec3 vNormal;"\
		"uniform mat4 u_model_view_matrix;"\
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_LKeyPressed;" \
		"uniform vec3 u_Ld;" \
		"uniform vec3 u_Kd;" \
		"uniform vec4 u_light_position;" \
		"out vec3 diffuse_light;" \
		"void main(void)" \
		"{" \
            "if(u_LKeyPressed == 1)"\
            "{"\
                "vec4 eyeCoordinates  = u_model_view_matrix * vPosition;"\
                "vec3 tnorm = normalize(mat3(u_model_view_matrix)*vNormal);"\
                "vec3 s = normalize(vec3(u_light_position - eyeCoordinates)); "\
                "diffuse_light =u_Ld * u_Kd * max(dot(s,tnorm),0.0);"\
            "}"\
                "gl_Position = u_projection_matrix *u_model_view_matrix* vPosition;" \
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
        "#version 330 core" \
        "\n" \
        "in vec3 diffuse_light;"\
		"out vec4 FragColor;"\
		"uniform int u_LKeyPressed;"\
		"void main(void)"\
		"{"\
            "vec4 color;"\
            "if(u_LKeyPressed == 1)"\
            "{"\
                "color = vec4(diffuse_light,1.0);"\
            "}"\
            "else"\
            "{"\
                "color = vec4(1.0,1.0,1.0,1.0);"\
            "}"\
            "FragColor = color;"\
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
    glBindAttribLocation(shaderProgramObject, VDG_ATTRIBUTE_NORMAL, "vNormal");

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
    
   //GET MVP UNIFORM LOCATION
	gModelViewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_model_view_matrix");

	gProjectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projection_matrix");

	gLKeyPressedUniform = glGetUniformLocation(shaderProgramObject, "u_LKeyPressed");

	gLdUniform = glGetUniformLocation(shaderProgramObject, "u_Ld");

	gKdUniform = glGetUniformLocation(shaderProgramObject, "u_Kd");

	gLightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_light_position");
	//

    
    // *** vertices, colors, shader attribs, vbo, vao initializations ***
 


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
    const GLfloat cubeNormals[] =
    {
        0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f
    };




     /*SQUARE*/
    glGenVertexArrays(1, &gVao_cube);
    glBindVertexArray(gVao_cube);
    
    /*POSITION*/
    glGenBuffers(1, &gVbo_cube_position);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_cube_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubevertices), cubevertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*END OF SQUARE*/

     /*normals*/
    glGenBuffers(1, &gVbo_cube_normal);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_cube_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);
    
    glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*END OF normals*/

   
    glBindVertexArray(0);
    /***/




    
	gbAnimate = false;
	gbLight = false;


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
   
   gAngle = gAngle + 0.3f;

	if (gAngle >= 360.0f)
		gAngle = gAngle - 360.0f;

}

- (void)drawView
{
    // code
    [[self openGLContext]makeCurrentContext];
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
	/*RULE TO ANIMATE OBJECTS
	1. Translate.
	2. Rotate.
	3. Scale.
	*/

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//START USING SHADER OBJECT	
	glUseProgram(shaderProgramObject);
	//DRAWING
	//


	if (gbLight == true)
	{
		glUniform1i(gLKeyPressedUniform, 1);

		glUniform3f(gLdUniform, 1.0f, 1.0f, 1.0f);
		glUniform3f(gKdUniform, 0.5f, 0.5f, 0.5f);

		static float lightPosition[] = { 0.0f ,0.0f , 2.0f, 1.0f };
		glUniform4fv(gLightPositionUniform, 1, lightPosition);
	}
	else
	{
		glUniform1i(gLKeyPressedUniform, 0);
	}


	vmath::mat4  modelmatrix = vmath::mat4::identity();
	vmath::mat4 modelviewMatrix = vmath::mat4::identity();
	vmath::mat4 rotateMatrix = vmath::mat4::identity();

	/*DRAWING RECTANGLE*/

	modelmatrix = vmath::translate(0.0f, 0.0f, -5.0f);
	rotateMatrix = vmath::rotate<GLfloat>(gAngle, gAngle, gAngle);

	modelviewMatrix = modelmatrix * rotateMatrix;


	glUniformMatrix4fv(gModelViewMatrixUniform, 1, GL_FALSE, modelviewMatrix);

	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, PerspectiveProjectionMatrix);


	//*BINDE With Rectangle Cordinates*//
	glBindVertexArray(gVao_cube);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);


	glBindVertexArray(0);

	/*END OF DRAWING TRIANGLE*/



	//
	//STOP USING SHADER OBJECT
	glUseProgram(0);


    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    if(gbAnimate)
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
    static bool bIsAKeyPressed = false;
    
    static bool bIsLKeyPressed = false;
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

		case 'A'://A or a
		case 'a':
			if (bIsAKeyPressed == false)
			{
				gbAnimate = true;
				bIsAKeyPressed = true;
			}
			else
			{
				gbAnimate = false;
				bIsAKeyPressed = false;
			}
			break;

		case 'L': //for L or l
		case 'l':
			if (bIsLKeyPressed == false)
			{
				gbLight = true;
				bIsLKeyPressed = true;
			}
			else
			{
				gbLight = false;
				bIsLKeyPressed = false;
			}
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
    //DESTROY VAO
	if (gVao_cube)
	{
		glDeleteVertexArrays(1, &gVao_cube);
		gVao_cube = 0;
	}


	//DESTROY VBO
	if (gVbo_cube_position)
	{
		glDeleteVertexArrays(1, &gVbo_cube_position);
		gVbo_cube_position = 0;
	}
	if (gVbo_cube_normal)
	{
		glDeleteVertexArrays(1, &gVbo_cube_normal);
		gVbo_cube_normal = 0;
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
