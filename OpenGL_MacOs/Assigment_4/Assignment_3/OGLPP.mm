// headers
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>
#import "vmath.h"
#import "MySphere.h"

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
    [window setTitle:@"Single Light with per Fragment using Phong Shading model on a Steady sphere"];
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

    GLuint model_matrix_uniform;
	GLuint view_matrix_uniform;
	GLuint projection_matrix_uniform;

    vmath::mat4 PerspectiveProjectionMatrix;

    GLuint La_uniform;
	GLuint Ld_uniform;
	GLuint Ls_uniform;
	GLuint light_position_uniform;
	GLuint L_KeyPressed_uniform;

	GLuint Ka_uniform;
	GLuint Kd_uniform;
	GLuint Ks_uniform;
	GLuint material_shininess_uniform;

	bool gbLight;

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
    "in vec3 vNormal;" \
    "uniform mat4 u_model_matrix;" \
    "uniform mat4 u_view_matrix;" \
    "uniform mat4 u_projection_matrix;" \
    "uniform vec4 u_light_position;" \
    "uniform int u_lighting_enabled;" \
    "out vec3 transformed_normals;" \
    "out vec3 light_direction;" \
    "out vec3 viewer_vector;" \
    "void main(void)" \
    "{" \
        "if(u_lighting_enabled == 1)" \
        "{" \
            "vec4 eyeCoordinates = u_view_matrix * u_model_matrix * vPosition;" \
            "transformed_normals = mat3(u_view_matrix * u_model_matrix) * vNormal;" \
            "light_direction = vec3(u_light_position) - eyeCoordinates.xyz;" \
            "viewer_vector = -eyeCoordinates.xyz;" \
    "}" \
        "gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
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
    "in vec3 transformed_normals;" \
    "in vec3 light_direction;" \
    "in vec3 viewer_vector;" \
    "out vec4 FragColor;" \
    "uniform vec3 u_La;" \
    "uniform vec3 u_Ld;" \
    "uniform vec3 u_Ls;" \
    "uniform vec3 u_Ka;" \
    "uniform vec3 u_Kd;" \
    "uniform vec3 u_Ks;" \
    "uniform float u_material_shininess;" \
    "uniform int u_lighting_enabled;" \
    "void main(void)" \
    "{" \
        "vec3 phong_ads_color;" \
        "if(u_lighting_enabled == 1)" \
        "{" \
            "vec3 normalized_transformed_normals = normalize(transformed_normals);" \
            "vec3 normalized_light_direction = normalize(light_direction);" \
            "vec3 normalized_viewer_vector = normalize(viewer_vector);" \
            "vec3 ambient = u_La * u_Ka;" \
            "float tn_dot_ld = max(dot(normalized_transformed_normals,normalized_light_direction),0.0);" \
            "vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" \
            "vec3 reflection_vector = reflect(-normalized_light_direction,normalized_transformed_normals);" \
            "vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector,normalized_viewer_vector),0.0),u_material_shininess);" \
            "phong_ads_color = ambient + diffuse + specular;" \
    "}"\
    "else" \
    "{" \
            "phong_ads_color = vec3(1.0,1.0,1.0);" \
    "}" \
        "FragColor = vec4(phong_ads_color,1.0);" \
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
    
    // get MVP uniform location
    model_matrix_uniform = glGetUniformLocation(shaderProgramObject, "u_model_matrix");
    view_matrix_uniform = glGetUniformLocation(shaderProgramObject, "u_view_matrix");
	projection_matrix_uniform = glGetUniformLocation(shaderProgramObject, "u_projection_matrix");
   



	// 'L' HANDLE KEY TO TOGGLE LIGHT.
	L_KeyPressed_uniform = glGetUniformLocation(shaderProgramObject, "u_lighting_enabled");

	//AMBIENT COLOR TOKEN
	La_uniform = glGetUniformLocation(shaderProgramObject, "u_La");
	
	// DIFUSE COLOR TOKEN
	Ld_uniform = glGetUniformLocation(shaderProgramObject, "u_Ld");
	
	// SPECULAR COLOR TOKEN
	Ls_uniform = glGetUniformLocation(shaderProgramObject, "u_Ls");
	
	// LIGHT POSITION TOKEN
	light_position_uniform = glGetUniformLocation(shaderProgramObject, "u_light_position");

	// AMBIENT REFLECTIVE TOKEN
	Ka_uniform = glGetUniformLocation(shaderProgramObject, "u_Ka");
	
	// DIFUUSE REFLECTIVE TOKEN
	Kd_uniform = glGetUniformLocation(shaderProgramObject, "u_Kd");
	
	// SPECULAR REFLECTIVE TOKEN
	Ks_uniform = glGetUniformLocation(shaderProgramObject, "u_Ks");
	
	//SHINYNESS COLOR TOKEN
	material_shininess_uniform = glGetUniformLocation(shaderProgramObject, "u_material_shininess");;

	/*VERTICES ,COLOR ,NORMAL  INITIALIZATION*/
	makeSphere(2.0f,30.0f,30.0f);


    
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
    
    //bIsLKeyPressed = false;

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

    
	if (gbLight == true)
	{
		static GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
		static GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
		static GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
		static GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };


		static GLfloat material_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
		static GLfloat material_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
		static GLfloat material_specular[] = { 1.0f,1.0f,1.0f,1.0f };
		static GLfloat material_shininess = 50.0f;

		// set 'u_lighting_enabled' uniform
		glUniform1i(L_KeyPressed_uniform, 1);

		// setting light's properties
		glUniform3fv(La_uniform, 1, lightAmbient);
		glUniform3fv(Ld_uniform, 1, lightDiffuse);
		glUniform3fv(Ls_uniform, 1, lightSpecular);
		glUniform4fv(light_position_uniform, 1, lightPosition);

		// setting material's properties
		glUniform3fv(Ka_uniform, 1, material_ambient);
		glUniform3fv(Kd_uniform, 1, material_diffuse);
		glUniform3fv(Ks_uniform, 1, material_specular);
		glUniform1f(material_shininess_uniform, material_shininess);
	}
	else
	{
		// set 'u_lighting_enabled' uniform
		glUniform1i(L_KeyPressed_uniform, 0);
	}



	/*DRAWING SPHERE*/
	vmath::mat4 modelMatrix = vmath::mat4::identity();
	vmath::mat4 viewMatrix = vmath::mat4::identity();

	modelMatrix = vmath::translate(0.0f, 0.0f, -8.0f);

	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, PerspectiveProjectionMatrix);

	
	//START DRAWING
	drawSphere();
	// UNBING VAO
	

	//STOP USING SHADER
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
    // destroy vao

     cleanupSphereBuffers();
     cleanupSphereMeshData();
    
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
