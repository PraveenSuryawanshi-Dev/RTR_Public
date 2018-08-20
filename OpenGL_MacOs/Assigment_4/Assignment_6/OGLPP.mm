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

typedef struct material
{
    GLfloat ambient_material[4];
    GLfloat diffuse_material[4];
    GLfloat specular_material[4];
    GLfloat shininess;
    
}MATERIAL;

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
    [window setTitle:@"Material Demo 24 Sphere"];
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

	GLfloat sphere_angle;


    GLuint La_uniform;
	GLuint Ld_uniform;
	GLuint Ls_uniform;
	GLuint light_position_uniform;
	GLuint light_rotation_uniform;

	GLuint L_KeyPressed_uniform;
		
	GLuint Ka_uniform;
	GLuint Kd_uniform;
	GLuint Ks_uniform;
	GLuint material_shininess_uniform;

    GLboolean bIsLKeyPressed;
	GLboolean gIsXKeyPressed;
	GLboolean gIsYKeyPressed;
	GLboolean gIsZKeyPressed;

    GLfloat lightPosition[4];

	MATERIAL  material[6][4];

	bool gbLight;

	GLfloat width,height;

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
			"in vec3 vNormal;" \
			"uniform mat4 u_model_matrix;" \
			"uniform mat4 u_view_matrix;" \
			"uniform mat4 u_projection_matrix;" \
			"uniform int u_lighting_enabled;" \
			"uniform vec3 u_La;" \
			"uniform vec3 u_Ld;" \
			"uniform vec3 u_Ls;" \
			"uniform vec4 u_light_position;" \
			"uniform mat4 u_light_rotation;"\

			"uniform vec3 u_Ka;" \
			"uniform vec3 u_Kd;" \
			"uniform vec3 u_Ks;" \
			"uniform float u_material_shininess;" \
			"out vec3 phong_ads_color;" \

			"void main(void)" \
			"{" \
				"if(u_lighting_enabled == 1)" \
				"{" \
					"vec4 temp= u_light_position*u_light_rotation;"\

					"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" \
					"vec3 transformed_normals=normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
					"vec3 light_direction = normalize(vec3(temp) - eye_coordinates.xyz);" \
					"float tn_dot_ld = max(dot(transformed_normals, light_direction),0.0);" \
					"vec3 ambient = u_La * u_Ka;" \
					"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" \
					"vec3 reflection_vector = reflect(-light_direction, transformed_normals);" \
					"vec3 viewer_vector = normalize(-eye_coordinates.xyz);" \
					"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess);" \
					"phong_ads_color=ambient + diffuse + specular;" \
				"}" \
				"else" \
				"{" \
					"phong_ads_color = vec3(1.0, 1.0, 1.0);" \
				"}" \

			"gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
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
  "in vec3 phong_ads_color;" \
			"out vec4 FragColor;" \
			"void main(void)" \
			"{" \
				"FragColor = vec4(phong_ads_color, 1.0);" \
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

	light_rotation_uniform = glGetUniformLocation(shaderProgramObject, "u_light_rotation");
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
    
    bIsLKeyPressed = false;

    gbLight = false;

    lightPosition[0] =0.0f;
    lightPosition[1] =0.0f;
    lightPosition[2] =0.0f;
    lightPosition[3] =0.0f;
    
	[self InitializeMaterials];

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
    
     width=rect.size.width;
     height=rect.size.height;

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
	if (sphere_angle >= 720.0f)
		sphere_angle = 360.0f;
	else
		sphere_angle = sphere_angle + 0.6f;

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

	if(gbLight)
		glUniform1i(L_KeyPressed_uniform, 1);
	else
		glUniform1i(L_KeyPressed_uniform, 0);


	//DRAWING SPHERE
	vmath::mat4 modelMatrix = vmath::mat4::identity();
	vmath::mat4 viewMatrix = vmath::mat4::identity();
	vmath::mat4 rotateMatrix = vmath::mat4::identity();


	viewMatrix = vmath::mat4::identity();
	//model view
	
	glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, PerspectiveProjectionMatrix);
	

	static GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
	static GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
	static GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
	

	// setting light's properties
	glUniform3fv(La_uniform, 1, lightAmbient);
	glUniform3fv(Ld_uniform, 1, lightDiffuse);
	glUniform3fv(Ls_uniform, 1, lightSpecular);
	glUniform4fv(light_position_uniform, 1, lightPosition);


	//double row = -4.0f;
	//double col = 4.4f;

	int col = 5;
	for (size_t i = 0; i < 6; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			GLfloat newWidth = (width / 4.0f);
			GLfloat newHeight = (height) / 6.0f;

			glViewport((newWidth*j), newHeight*col, newWidth, newHeight);
			PerspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)newWidth / (GLfloat)newHeight, 0.1f, 100.0f);

			// setting material's properties
			glUniform3fv(Ka_uniform, 1, material[i][j].ambient_material);
			glUniform3fv(Kd_uniform, 1, material[i][j].diffuse_material);
			glUniform3fv(Ks_uniform, 1, material[i][j].specular_material);
			glUniform1f(material_shininess_uniform, material[i][j].shininess);
			//


			rotateMatrix = vmath::mat4::identity();
			modelMatrix = vmath::translate(0.0f, 0.0f, -8.0f);
			//modelMatrix = vmath::translate((GLfloat)row, (GLfloat)col, -13.0f);
			if (gIsXKeyPressed == GL_TRUE)
			{
				rotateMatrix = vmath::rotate(sphere_angle, 1.0f, 0.0f, 0.0f);
				//modelMatrix = modelMatrix * rotateMatrix;

				lightPosition[1] = sphere_angle;
				glUniform4fv(light_position_uniform, 1, lightPosition);
				glUniformMatrix4fv(light_rotation_uniform, 1, GL_FALSE, rotateMatrix);
			}
			else
			if (gIsYKeyPressed == GL_TRUE)
			{
				rotateMatrix = vmath::rotate(sphere_angle, 0.0f, 1.0f, 0.0f);
				//modelMatrix = modelMatrix * rotateMatrix;

				lightPosition[0] = sphere_angle;
				glUniform4fv(light_position_uniform, 1, lightPosition);
				glUniformMatrix4fv(light_rotation_uniform, 1, GL_FALSE, rotateMatrix);
			}
			else
			if (gIsZKeyPressed == GL_TRUE)
			{
				rotateMatrix = vmath::rotate(sphere_angle, 0.0f, 0.0f, 1.0f);
				//modelMatrix = modelMatrix * rotateMatrix;

				lightPosition[0] = sphere_angle;
				glUniform4fv(light_position_uniform, 1, lightPosition);
				glUniformMatrix4fv(light_rotation_uniform, 1, GL_FALSE, rotateMatrix);
			}
			else
			{
				lightPosition[3] = 0.0f;
				lightPosition[1] = 0.0f;
				lightPosition[2] = 1.0f;
				glUniform4fv(light_position_uniform, 1, lightPosition);
			}
			
			glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
			//row = row + 2.8f;
		
			
			// BIND VAO

			//START DRAWING
			drawSphere();
			// UNBING VAO

		}

		col--;

		//row = -4.0f;
		//col = col -1.8f;
	}



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

		case 'X':
		case 'x':
		
			lightPosition[0] = 0.0f;
			lightPosition[1] = 0.0f;
			lightPosition[2] = 0.0f;

			if (gIsXKeyPressed == GL_TRUE)
			{
				gIsXKeyPressed = GL_FALSE;
			}
			else
			{
				gIsXKeyPressed = GL_TRUE;
				gIsYKeyPressed = GL_FALSE;
				gIsZKeyPressed = GL_FALSE;
			}
			break;


		case 'Y':
		case 'y':
		
			lightPosition[0] = 0.0f;
			lightPosition[1] = 0.0f;
			lightPosition[2] = 0.0f;

			if (gIsYKeyPressed == GL_TRUE)
			{
				gIsYKeyPressed = GL_FALSE;
			}
			else
			{
				gIsYKeyPressed = GL_TRUE;
				gIsXKeyPressed = GL_FALSE;
				gIsZKeyPressed = GL_FALSE;
			}
			break;

		case 'Z':
		case 'z':

			lightPosition[0] = 0.0f;
			lightPosition[1] = 0.0f;
			lightPosition[2] = 0.0f;

			if (gIsZKeyPressed == GL_TRUE)
			{
				gIsZKeyPressed = GL_FALSE;
			}
			else
			{
				gIsZKeyPressed = GL_TRUE;
				gIsXKeyPressed = GL_FALSE;
				gIsYKeyPressed = GL_FALSE;
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
    
   //DETACH THE FRAGMENT SHADER OBJECT
	glDetachShader(shaderProgramObject, fragmentShaderObject);

	//DETACH THE VERTEX SHADER OBJECT
	glDetachShader(shaderProgramObject, vertexShaderObject);


	//DELETE VERTEX SHADER OBJECT
	glDeleteShader(vertexShaderObject);
	vertexShaderObject = 0;

	//DELETE FRAGMENT SHADER OBJECT
	glDeleteShader(fragmentShaderObject);
	fragmentShaderObject = 0;

	//DELETE SHADER PROGRAM OBJECT
	glDeleteProgram(shaderProgramObject);
	shaderProgramObject = 0;


	//UNLINK THE PROGRAM
	glUseProgram(0);

    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);

    [super dealloc];
}

-(void) InitializeMaterials
{
	/***emerald**************************************************************************************/
	material[0][0].ambient_material[0] = 0.0215f; material[0][0].ambient_material[1] = 0.1745f;
	material[0][0].ambient_material[2] = 0.0215f; material[0][0].ambient_material[3] = 1.0f;

	material[0][0].diffuse_material[0] = 0.07568f; material[0][0].diffuse_material[1] = 0.61424f;
	material[0][0].diffuse_material[2] = 0.07568f; material[0][0].diffuse_material[3] = 1.0f;

	material[0][0].specular_material[0] = 0.633f; material[0][0].specular_material[1] = 0.727811f;
	material[0][0].specular_material[2] = 0.633f; material[0][0].specular_material[3] = 1.0f;
	material[0][0].shininess = 0.6f * 128.0f;


	/***jade**************************************************************************************/
	material[1][0].ambient_material[0] = 0.135f; material[1][0].ambient_material[1] = 0.2225f;
	material[1][0].ambient_material[2] = 0.1575f; material[1][0].ambient_material[3] = 1.0f;

	material[1][0].diffuse_material[0] = 0.54f; material[1][0].diffuse_material[1] = 0.89f;
	material[1][0].diffuse_material[2] = 0.63f; material[1][0].diffuse_material[3] = 1.0f;

	material[1][0].specular_material[0] = 0.316228f; material[1][0].specular_material[1] = 0.7316228f;
	material[1][0].specular_material[2] = 0.316228f; material[1][0].specular_material[3] = 1.0f;
	material[1][0].shininess = 0.1f * 128.0f;


	/***obsidian**************************************************************************************/
	material[2][0].ambient_material[0] = 0.05375f; material[2][0].ambient_material[1] = 0.05f;
	material[2][0].ambient_material[2] = 0.06625f; material[2][0].ambient_material[3] = 1.0f;

	material[2][0].diffuse_material[0] = 0.18275f; material[2][0].diffuse_material[1] = 0.17f;
	material[2][0].diffuse_material[2] = 0.22525f; material[2][0].diffuse_material[3] = 1.0f;

	material[2][0].specular_material[0] = 0.332741f; material[2][0].specular_material[1] = 0.328634f;
	material[2][0].specular_material[2] = 0.346435f; material[2][0].specular_material[3] = 1.0f;
	material[2][0].shininess = 0.3f * 128.0f;


	/***pearl**************************************************************************************/
	material[3][0].ambient_material[0] = 0.25f; material[3][0].ambient_material[1] = 0.2075f;
	material[3][0].ambient_material[2] = 0.20725f; material[3][0].ambient_material[3] = 1.0f;

	material[3][0].diffuse_material[0] = 1.0f; material[3][0].diffuse_material[1] = 0.829f;
	material[3][0].diffuse_material[2] = 0.829f; material[3][0].diffuse_material[3] = 1.0f;

	material[3][0].specular_material[0] = 0.296648f; material[3][0].specular_material[1] = 0.296648f;
	material[3][0].specular_material[2] = 0.296648f; material[3][0].specular_material[3] = 1.0f;
	material[3][0].shininess = 0.088f * 128.0f;


	/***ruby**************************************************************************************/
	material[4][0].ambient_material[0] = 0.1745f; material[4][0].ambient_material[1] = 0.01175f;
	material[4][0].ambient_material[2] = 0.01175f; material[4][0].ambient_material[3] = 1.0f;

	material[4][0].diffuse_material[0] = 0.61424f; material[4][0].diffuse_material[1] = 0.04136f;
	material[4][0].diffuse_material[2] = 0.04136f; material[4][0].diffuse_material[3] = 1.0f;

	material[4][0].specular_material[0] = 0.727811f; material[4][0].specular_material[1] = 0.626959f;
	material[4][0].specular_material[2] = 0.626959f; material[4][0].specular_material[3] = 1.0f;
	material[4][0].shininess = 0.6f * 128.0f;


	/***turquoise**************************************************************************************/
	material[5][0].ambient_material[0] = 0.1f; material[5][0].ambient_material[1] = 0.18725f;
	material[5][0].ambient_material[2] = 0.1745f; material[5][0].ambient_material[3] = 1.0f;

	material[5][0].diffuse_material[0] = 0.396f; material[5][0].diffuse_material[1] = 0.74151f;
	material[5][0].diffuse_material[2] = 0.69102f; material[5][0].diffuse_material[3] = 1.0f;

	material[5][0].specular_material[0] = 0.297254f; material[5][0].specular_material[1] = 0.30829f;
	material[5][0].specular_material[2] = 0.306678f; material[5][0].specular_material[3] = 1.0f;
	material[5][0].shininess = 0.1f * 128.0f;

	/***brass**************************************************************************************/
	material[0][1].ambient_material[0] = 0.329412f; material[0][1].ambient_material[1] = 0.223529f;
	material[0][1].ambient_material[2] = 0.027451f; material[0][1].ambient_material[3] = 1.0f;

	material[0][1].diffuse_material[0] = 0.780392f; material[0][1].diffuse_material[1] = 0.568627f;
	material[0][1].diffuse_material[2] = 0.113725f; material[0][1].diffuse_material[3] = 1.0f;

	material[0][1].specular_material[0] = 0.992157f; material[0][1].specular_material[1] = 0.941176f;
	material[0][1].specular_material[2] = 0.807843f; material[0][1].specular_material[3] = 1.0f;
	material[0][1].shininess = 0.21794872f * 128.0f;


	/***bronze**************************************************************************************/
	material[1][1].ambient_material[0] = 0.2125f; material[1][1].ambient_material[1] = 0.1275f;
	material[1][1].ambient_material[2] = 0.054f; material[1][1].ambient_material[3] = 1.0f;

	material[1][1].diffuse_material[0] = 0.714f; material[1][1].diffuse_material[1] = 0.4284f;
	material[1][1].diffuse_material[2] = 0.4284f; material[1][1].diffuse_material[3] = 1.0f;

	material[1][1].specular_material[0] = 0.393548f; material[1][1].specular_material[1] = 0.271906f;
	material[1][1].specular_material[2] = 0.166721f; material[1][1].specular_material[3] = 1.0f;
	material[1][1].shininess = 0.2f * 128.0f;

	/***chrome**************************************************************************************/
	material[2][1].ambient_material[0] = 0.25f; material[2][1].ambient_material[1] = 0.25f;
	material[2][1].ambient_material[2] = 0.25f; material[2][1].ambient_material[3] = 1.0f;

	material[2][1].diffuse_material[0] = 0.4f; material[2][1].diffuse_material[1] = 0.4f;
	material[2][1].diffuse_material[2] = 0.4f; material[2][1].diffuse_material[3] = 1.0f;

	material[2][1].specular_material[0] = 0.774597f; material[2][1].specular_material[1] = 0.774597f;
	material[2][1].specular_material[2] = 0.774597f; material[2][1].specular_material[3] = 1.0f;
	material[2][1].shininess = 0.6f * 128.0f;

	/****copper*************************************************************************************/
	material[3][1].ambient_material[0] = 0.19125f; material[3][1].ambient_material[1] = 0.0735f;
	material[3][1].ambient_material[2] = 0.0225f; material[3][1].ambient_material[3] = 1.0f;

	material[3][1].diffuse_material[0] = 0.7038f; material[3][1].diffuse_material[1] = 0.27048f;
	material[3][1].diffuse_material[2] = 0.0828f; material[3][1].diffuse_material[3] = 1.0f;

	material[3][1].specular_material[0] = 0.256777f; material[3][1].specular_material[1] = 0.137622f;
	material[3][1].specular_material[2] = 0.086014f; material[3][1].specular_material[3] = 1.0f;
	material[3][1].shininess = 0.1f * 128.0f;

	/***gold**************************************************************************************/
	material[4][1].ambient_material[0] = 0.24725f; material[4][1].ambient_material[1] = 0.1995f;
	material[4][1].ambient_material[2] = 0.0745f; material[4][1].ambient_material[3] = 1.0f;

	material[4][1].diffuse_material[0] = 0.75164f; material[4][1].diffuse_material[1] = 0.60648f;
	material[4][1].diffuse_material[2] = 0.22648f; material[4][1].diffuse_material[3] = 1.0f;

	material[4][1].specular_material[0] = 0.628281f; material[4][1].specular_material[1] = 0.555802f;
	material[4][1].specular_material[2] = 0.366065f; material[4][1].specular_material[3] = 1.0f;
	material[4][1].shininess = 0.4f * 128.0f;


	/***silver**************************************************************************************/
	material[5][1].ambient_material[0] = 0.19225f; material[5][1].ambient_material[1] = 0.19225f;
	material[5][1].ambient_material[2] = 0.19225f; material[5][1].ambient_material[3] = 1.0f;

	material[5][1].diffuse_material[0] = 0.50754f; material[5][1].diffuse_material[1] = 0.50754f;
	material[5][1].diffuse_material[2] = 0.50754f; material[5][1].diffuse_material[3] = 1.0f;

	material[5][1].specular_material[0] = 0.508273f; material[5][1].specular_material[1] = 0.508273f;
	material[5][1].specular_material[2] = 0.508273f; material[5][1].specular_material[3] = 1.0f;
	material[5][1].shininess = 0.2f * 128.0f;


	/***black**************************************************************************************/
	material[0][2].ambient_material[0] = 0.0f; material[0][2].ambient_material[1] = 0.0f;
	material[0][2].ambient_material[2] = 0.0f; material[0][2].ambient_material[3] = 1.0f;

	material[0][2].diffuse_material[0] = 0.01f; material[0][2].diffuse_material[1] = 0.01f;
	material[0][2].diffuse_material[2] = 0.01f; material[0][2].diffuse_material[3] = 1.0f;

	material[0][2].specular_material[0] = 0.50f; material[0][2].specular_material[1] = 0.50f;
	material[0][2].specular_material[2] = 0.50f; material[0][2].specular_material[3] = 1.0f;
	material[0][2].shininess = 0.25f * 128.0f;

	/***cyan**************************************************************************************/
	material[1][2].ambient_material[0] = 0.0f; material[1][2].ambient_material[1] = 0.1f;
	material[1][2].ambient_material[2] = 0.06f; material[1][2].ambient_material[3] = 1.0f;

	material[1][2].diffuse_material[0] = 0.0f; material[1][2].diffuse_material[1] = 0.50980392f;
	material[1][2].diffuse_material[2] = 0.50980392f; material[1][2].diffuse_material[3] = 1.0f;

	material[1][2].specular_material[0] = 0.50196078f; material[1][2].specular_material[1] = 0.50196078f;
	material[1][2].specular_material[2] = 0.50196078f; material[1][2].specular_material[3] = 1.0f;
	material[1][2].shininess = 0.25f * 128.0f;

	/***green**************************************************************************************/
	material[2][2].ambient_material[0] = 0.0f; material[2][2].ambient_material[1] = 0.0f;
	material[2][2].ambient_material[2] = 0.0f; material[2][2].ambient_material[3] = 1.0f;

	material[2][2].diffuse_material[0] = 0.1f; material[2][2].diffuse_material[1] = 0.35f;
	material[2][2].diffuse_material[2] = 0.1f; material[2][2].diffuse_material[3] = 1.0f;

	material[2][2].specular_material[0] = 0.45f; material[2][2].specular_material[1] = 0.55f;
	material[2][2].specular_material[2] = 0.45f; material[2][2].specular_material[3] = 1.0f;
	material[2][2].shininess = 0.25f * 128.0f;


	/***red**************************************************************************************/
	material[3][2].ambient_material[0] = 0.0f; material[3][2].ambient_material[1] = 0.0f;
	material[3][2].ambient_material[2] = 0.0f; material[3][2].ambient_material[3] = 1.0f;

	material[3][2].diffuse_material[0] = 0.5f; material[3][2].diffuse_material[1] = 0.0f;
	material[3][2].diffuse_material[2] = 0.0f; material[3][2].diffuse_material[3] = 1.0f;

	material[3][2].specular_material[0] = 0.7f; material[3][2].specular_material[1] = 0.6f;
	material[3][2].specular_material[2] = 0.6f; material[3][2].specular_material[3] = 1.0f;
	material[3][2].shininess = 0.25f * 128.0f;

	/***white**************************************************************************************/
	material[4][2].ambient_material[0] = 0.0f; material[4][2].ambient_material[1] = 0.0f;
	material[4][2].ambient_material[2] = 0.0f; material[4][2].ambient_material[3] = 1.0f;

	material[4][2].diffuse_material[0] = 0.55f; material[4][2].diffuse_material[1] = 0.55f;
	material[4][2].diffuse_material[2] = 0.55f; material[4][2].diffuse_material[3] = 1.0f;

	material[4][2].specular_material[0] = 0.70f; material[4][2].specular_material[1] = 0.70f;
	material[4][2].specular_material[2] = 0.70f; material[4][2].specular_material[3] = 1.0f;
	material[4][2].shininess = 0.25f * 128.0f;

	/***plastic**************************************************************************************/
	material[5][2].ambient_material[0] = 0.0f; material[5][2].ambient_material[1] = 0.0f;
	material[5][2].ambient_material[2] = 0.0f; material[5][2].ambient_material[3] = 1.0f;

	material[5][2].diffuse_material[0] = 0.5f; material[5][2].diffuse_material[1] = 0.5f;
	material[5][2].diffuse_material[2] = 0.0f; material[5][2].diffuse_material[3] = 1.0f;

	material[5][2].specular_material[0] = 0.60f; material[5][2].specular_material[1] = 0.60f;
	material[5][2].specular_material[2] = 0.50f; material[5][2].specular_material[3] = 1.0f;
	material[5][2].shininess = 0.25f * 128.0f;


	/***black**************************************************************************************/
	material[0][3].ambient_material[0] = 0.02f; material[0][3].ambient_material[1] = 0.02f;
	material[0][3].ambient_material[2] = 0.02f; material[0][3].ambient_material[3] = 1.0f;

	material[0][3].diffuse_material[0] = 0.01f; material[0][3].diffuse_material[1] = 0.01f;
	material[0][3].diffuse_material[2] = 0.01f; material[0][3].diffuse_material[3] = 1.0f;

	material[0][3].specular_material[0] = 0.50f; material[0][3].specular_material[1] = 0.50f;
	material[0][3].specular_material[2] = 0.50f; material[0][3].specular_material[3] = 1.0f;
	material[0][3].shininess = 0.07812f* 128.0f;


	/***cyan**************************************************************************************/
	material[1][3].ambient_material[0] = 0.0f; material[1][3].ambient_material[1] = 0.05f;
	material[1][3].ambient_material[2] = 0.05f; material[1][3].ambient_material[3] = 1.0f;

	material[1][3].diffuse_material[0] = 0.4f; material[1][3].diffuse_material[1] = 0.5f;
	material[1][3].diffuse_material[2] = 0.5f; material[1][3].diffuse_material[3] = 1.0f;

	material[1][3].specular_material[0] = 0.04f; material[1][3].specular_material[1] = 0.7f;
	material[1][3].specular_material[2] = 0.7f; material[1][3].specular_material[3] = 1.0f;
	material[1][3].shininess = 0.078125f * 128.0f;
	/********************************************************************************************/


	/***green**************************************************************************************/
	material[2][3].ambient_material[0] = 0.0f; material[2][3].ambient_material[1] = 0.05f;
	material[2][3].ambient_material[2] = 0.0f; material[2][3].ambient_material[3] = 1.0f;

	material[2][3].diffuse_material[0] = 0.4f; material[2][3].diffuse_material[1] = 0.5f;
	material[2][3].diffuse_material[2] = 0.4f; material[2][3].diffuse_material[3] = 1.0f;

	material[2][3].specular_material[0] = 0.04f; material[2][3].specular_material[1] = 0.7f;
	material[2][3].specular_material[2] = 0.04f; material[2][3].specular_material[3] = 1.0f;
	material[2][3].shininess = 0.078125f * 128.0f;

	/***red**************************************************************************************/
	material[3][3].ambient_material[0] = 0.05f; material[3][3].ambient_material[1] = 0.0f;
	material[3][3].ambient_material[2] = 0.0f; material[3][3].ambient_material[3] = 1.0f;

	material[3][3].diffuse_material[0] = 0.5f; material[3][3].diffuse_material[1] = 0.4f;
	material[3][3].diffuse_material[2] = 0.4f; material[3][3].diffuse_material[3] = 1.0f;

	material[3][3].specular_material[0] = 0.7f; material[3][3].specular_material[1] = 0.04f;
	material[3][3].specular_material[2] = 0.04f; material[3][3].specular_material[3] = 1.0f;
	material[3][3].shininess = 0.078125f * 128.0f;

	/***white**************************************************************************************/
	material[4][3].ambient_material[0] = 0.05f; material[4][3].ambient_material[1] = 0.05f;
	material[4][3].ambient_material[2] = 0.05f; material[4][3].ambient_material[3] = 1.0f;

	material[4][3].diffuse_material[0] = 0.5f; material[4][3].diffuse_material[1] = 0.5f;
	material[4][3].diffuse_material[2] = 0.5f; material[4][3].diffuse_material[3] = 1.0f;

	material[4][3].specular_material[0] = 0.7f; material[4][3].specular_material[1] = 0.7f;
	material[4][3].specular_material[2] = 0.7f; material[4][3].specular_material[3] = 1.0f;
	material[4][3].shininess = 0.078125f * 128.0f;

	/***yellow rubber**************************************************************************************/
	material[5][3].ambient_material[0] = 0.05f; material[5][3].ambient_material[1] = 0.05f;
	material[5][3].ambient_material[2] = 0.0f; material[5][3].ambient_material[3] = 1.0f;

	material[5][3].diffuse_material[0] = 0.5f; material[5][3].diffuse_material[1] = 0.5f;
	material[5][3].diffuse_material[2] = 0.4f; material[5][3].diffuse_material[3] = 1.0f;

	material[5][3].specular_material[0] = 0.7f; material[5][3].specular_material[1] = 0.7f;
	material[5][3].specular_material[2] = 0.04f; material[5][3].specular_material[3] = 1.0f;
	material[5][3].shininess = 0.078125f * 128.0f;
	/*******************************************************************************************/
}

@end

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,const CVTimeStamp *pNow,const CVTimeStamp *pOutputTime,CVOptionFlags flagsIn,
                               CVOptionFlags *pFlagsOut,void *pDisplayLinkContext)
{
    CVReturn result=[(GLView *)pDisplayLinkContext getFrameForTime:pOutputTime];
    return(result);
}
