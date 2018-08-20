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
    [window setTitle:@"Three Lights on Sphere using Fragment and Vertex Shader."];
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
        
GLuint gVertexShaderObject_vertexShader;
GLuint gFragmentShaderObject_vertexShader;
GLuint gShaderProgramObject_vertexShader;

GLuint gVertexShaderObject_fragmentShader;
GLuint gFragmentShaderObject_fragmentShader;
GLuint gShaderProgramObject_fragmentShader;

GLuint gVertexShaderObject_SimpleShader;
GLuint gFragmentShaderObject_SimpleShader;
GLuint gShaderProgramObject_SimpleShader;

GLuint model_matrix_uniform;
GLuint view_matrix_uniform;
GLuint projection_matrix_uniform;
vmath::mat4 gPerspectiveProjectionMatrix;



GLuint La1_uniform;
GLuint Ld1_uniform;
GLuint Ls1_uniform;
GLuint light_position1_uniform;
GLuint light_rotation1_uniform;

GLuint La2_uniform;
GLuint Ld2_uniform;
GLuint Ls2_uniform;
GLuint light_position2_uniform;
GLuint light_rotation2_uniform;

GLuint La3_uniform;
GLuint Ld3_uniform;
GLuint Ls3_uniform;
GLuint light_position3_uniform;
GLuint light_rotation3_uniform;
GLuint gbUseFragment_uniform;
GLuint gbUseVertex_uniform;


GLuint Ka_uniform;
GLuint Kd_uniform;
GLuint Ks_uniform;
GLuint material_shininess_uniform;

GLuint L_KeyPressed_uniform;

bool bIsLKeyPressed;
bool bIsFkeyPressed;
bool bIsVkeyPressed;

bool gbLight;
GLfloat angleRed;
GLfloat angleBlue;
GLfloat angleGreen;


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
    gVertexShaderObject_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    
    // provide source code to shader
     const GLchar *vertexShaderSourceCode_vertexShader =
    "#version 330" \
    "\n" \
  		 "in vec4 vPosition;" \
		"in vec3 vNormal;" \

		"uniform int u_lighting_enabled;" \

		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \

		"uniform vec3 u_La1;" \
		"uniform vec3 u_Ld1;" \
		"uniform vec3 u_Ls1;" \
		"uniform vec4 u_light_position1;" \
		"uniform mat4 u_light_rotation1;"\

		"uniform vec3 u_La2;" \
		"uniform vec3 u_Ld2;" \
		"uniform vec3 u_Ls2;" \
		"uniform vec4 u_light_position2;" \
		"uniform mat4 u_light_rotation2;"\

		"uniform vec3 u_La3;" \
		"uniform vec3 u_Ld3;" \
		"uniform vec3 u_Ls3;" \
		"uniform vec4 u_light_position3;" \
		"uniform mat4 u_light_rotation3;"\

		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \

		"out vec3 phong_ads_color;" \
		"void main(void)" \
		"{" \

		"if(u_lighting_enabled == 1)"\
		"{"\
			"vec4 temp;"\
			"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" \
			"vec3 transformed_normals=normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
			
			"temp =u_light_position1 * u_light_rotation1;"\
			"vec3 light_direction1 = normalize(vec3(temp) - eye_coordinates.xyz);" \
			"float tn_dot_ld1 = max(dot(transformed_normals, light_direction1),0.0);" \
			"vec3 ambient1 = u_La1 * u_Ka;" \
			"vec3 diffuse1 = u_Ld1* u_Kd * tn_dot_ld1;" \
			"vec3 reflection_vector1 = reflect(-light_direction1, transformed_normals);" \
			"vec3 viewer_vector = normalize(-eye_coordinates.xyz);" \
			"vec3 specular1 = u_Ls1 * u_Ks * pow(max(dot(reflection_vector1, viewer_vector), 0.0), u_material_shininess);" \

			"temp =u_light_position2 * u_light_rotation2;"\
			"vec3 light_direction2 = normalize(vec3(temp) - eye_coordinates.xyz);" \
			"float tn_dot_ld2 = max(dot(transformed_normals, light_direction2),0.0);" \
			"vec3 ambient2 = u_La2 * u_Ka;" \
			"vec3 diffuse2 = u_Ld2* u_Kd * tn_dot_ld2;" \
			"vec3 reflection_vector2 = reflect(-light_direction2, transformed_normals);" \
			"vec3 specular2 = u_Ls2 * u_Ks * pow(max(dot(reflection_vector2, viewer_vector), 0.0), u_material_shininess);" \
			
			"temp =u_light_position3 * u_light_rotation3;"\
			"vec3 light_direction3= normalize(vec3(temp) - eye_coordinates.xyz);" \
			"float tn_dot_ld3 = max(dot(transformed_normals, light_direction3),0.0);" \
			"vec3 ambient3 = u_La3 * u_Ka;" \
			"vec3 diffuse3 = u_Ld3 * u_Kd * tn_dot_ld3;" \
			
			"vec3 reflection_vector3 = reflect(-light_direction3, transformed_normals);" \
			
			"vec3 specular3 = u_Ls3 * u_Ks * pow(max(dot(reflection_vector3, viewer_vector), 0.0), u_material_shininess);" \
			
			"phong_ads_color= (ambient1+ambient2+ambient3) + (diffuse1+diffuse2+diffuse3)+ (specular1+specular2+specular3);" \
		"}"\
		"else"\
		"{"\
			"phong_ads_color = vec3(1.0, 1.0, 1.0);" \
		"}"\
			
				"gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
	"}";

    glShaderSource(gVertexShaderObject_vertexShader, 1, (const GLchar **)&vertexShaderSourceCode_vertexShader, NULL);
    
    // compile shader
    glCompileShader(gVertexShaderObject_vertexShader);
    GLint iInfoLogLength = 0;
    GLint iShaderCompiledStatus = 0;
    char *szInfoLog = NULL;
    glGetShaderiv(gVertexShaderObject_vertexShader, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(gVertexShaderObject_vertexShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(gVertexShaderObject_vertexShader, iInfoLogLength, &written, szInfoLog);
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
    gFragmentShaderObject_vertexShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    // provide source code to shader
     const GLchar *fragmentShaderSourceCode_vertexshader =
    "#version 330" \
    "\n" \
  		"out vec4 FragColor;" \
		"in vec3 phong_ads_color;" \
		"void main(void)" \
		"{" \
			"FragColor = vec4(phong_ads_color, 1.0);" \
		"}";
    
    glShaderSource(gFragmentShaderObject_vertexShader, 1, (const GLchar **)&fragmentShaderSourceCode_vertexshader, NULL);
    
    // compile shader
    glCompileShader(gFragmentShaderObject_vertexShader);
    glGetShaderiv(gFragmentShaderObject_vertexShader, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(gFragmentShaderObject_vertexShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(gFragmentShaderObject_vertexShader, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    // *** SHADER PROGRAM ***
    // create
    gShaderProgramObject_vertexShader = glCreateProgram();
    
    // attach vertex shader to shader program
    glAttachShader(gShaderProgramObject_vertexShader, gVertexShaderObject_vertexShader);
    
    // attach fragment shader to shader program
    glAttachShader(gShaderProgramObject_vertexShader, gFragmentShaderObject_vertexShader);
    
    // pre-link binding of shader program object with vertex shader position attribute
    glBindAttribLocation(gShaderProgramObject_vertexShader, VDG_ATTRIBUTE_VERTEX, "vPosition");
    
    //PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITTH COLOR SHADER ATTRIBUTE
    glBindAttribLocation(gShaderProgramObject_vertexShader, VDG_ATTRIBUTE_NORMAL, "vNormal");

    // link shader
    glLinkProgram(gShaderProgramObject_vertexShader);
    GLint iShaderProgramLinkStatus = 0;
    glGetProgramiv(gShaderProgramObject_vertexShader, GL_LINK_STATUS, &iShaderProgramLinkStatus);
    if (iShaderProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(gShaderProgramObject_vertexShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength>0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetProgramInfoLog(gShaderProgramObject_vertexShader, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    

    //////////////////////////////////////////////////////////////////////////////////////////////////////


   // *** VERTEX SHADER ***
    // create shader
    gVertexShaderObject_fragmentShader = glCreateShader(GL_VERTEX_SHADER);
    
    // provide source code to shader
    const GLchar *vertexShaderSourceCode_fragmentShader =
    "#version 330" \
    "\n" \
  		"uniform int u_lighting_enabled;" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform vec4 u_light_position1;" \
		"uniform mat4 u_light_rotation1;"\
		"uniform vec4 u_light_position2;" \
		"uniform mat4 u_light_rotation2;"\
		"uniform vec4 u_light_position3;" \
		"uniform mat4 u_light_rotation3;"\

		"out vec3 transformed_normals;" \
		"out vec3 light_direction1;" \
		"out vec3 light_direction2;" \
		"out vec3 light_direction3;" \
		"out vec3 viewer_vector;" \

		"void main(void)" \
		"{" \
			"if(u_lighting_enabled == 1)" \
			"{" \
			"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" \
			"transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;" \
			"viewer_vector = -eye_coordinates.xyz;" \

			"vec4 temp1= u_light_position1 * u_light_rotation1 ;"\
			"vec4 temp2= u_light_position2 * u_light_rotation2 ;"\
			"vec4 temp3= u_light_position3 * u_light_rotation3 ;"\

				
			"light_direction1 = vec3(temp1) - eye_coordinates.xyz;" \
			"light_direction2 = vec3(temp2) - eye_coordinates.xyz;" \
			"light_direction3 = vec3(temp3) - eye_coordinates.xyz;" \
				
			"}" \
			"gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		"}";

    glShaderSource(gVertexShaderObject_fragmentShader, 1, (const GLchar **)&vertexShaderSourceCode_fragmentShader, NULL);
    
    // compile shader
    glCompileShader(gVertexShaderObject_fragmentShader);
    iInfoLogLength = 0;
    iShaderCompiledStatus = 0;
    szInfoLog = NULL;
    glGetShaderiv(gVertexShaderObject_fragmentShader, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(gVertexShaderObject_fragmentShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(gVertexShaderObject_fragmentShader, iInfoLogLength, &written, szInfoLog);
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
    gFragmentShaderObject_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    // provide source code to shader
    const GLchar *fragmentShaderSourceCode_fragmentShader =
    "#version 330" \
    "\n" \
 		"in vec3 transformed_normals;" \
		"in vec3 viewer_vector;" \
		"out vec4 FragColor;" \
	
		"in vec3 light_direction1;" \
		"in vec3 light_direction2;" \
		"in vec3 light_direction3;" \

		"uniform vec3 u_La1;" \
		"uniform vec3 u_Ld1;" \
		"uniform vec3 u_Ls1;" \

		"uniform vec3 u_La2;" \
		"uniform vec3 u_Ld2;" \
		"uniform vec3 u_Ls2;" \

		"uniform vec3 u_La3;" \
		"uniform vec3 u_Ld3;" \
		"uniform vec3 u_Ls3;" \


		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \
		"uniform int u_lighting_enabled;" \
		"void main(void)" \
		"{" \
			"vec3 phong_ads_color;" \
			"if(u_lighting_enabled==1)" \
			"{" \
				"vec3 normalized_transformed_normals=normalize(transformed_normals);" \
				"vec3 normalized_viewer_vector=normalize(viewer_vector);" \

				"vec3 normalized_light_direction1 =normalize(light_direction1);"\
				"vec3 ambient1 = u_La1 * u_Ka;" \
				"float tn_dot_ld1 = max(dot(normalized_transformed_normals, normalized_light_direction1),0.0);" \
				"vec3 diffuse1 = u_Ld1 * u_Kd * tn_dot_ld1;" \
				"vec3 reflection_vector1 = reflect(-normalized_light_direction1, normalized_transformed_normals);" \
				"vec3 specular1 = u_Ls1 * u_Ks * pow(max(dot(reflection_vector1, normalized_viewer_vector), 0.0), u_material_shininess);" \
				
				"vec3 normalized_light_direction2 =normalize(light_direction2);"\
				"vec3 ambient2 = u_La2 * u_Ka;" \
				"float tn_dot_ld2 = max(dot(normalized_transformed_normals, normalized_light_direction2),0.0);" \
				"vec3 diffuse2 = u_Ld2 * u_Kd * tn_dot_ld2;" \
				"vec3 reflection_vector2 = reflect(-normalized_light_direction2, normalized_transformed_normals);" \
				"vec3 specular2 = u_Ls2 * u_Ks * pow(max(dot(reflection_vector2, normalized_viewer_vector), 0.0), u_material_shininess);" \
				

				"vec3 normalized_light_direction3 =normalize(light_direction3);"\
				"vec3 ambient3 = u_La3 * u_Ka;" \
				"float tn_dot_ld3 = max(dot(normalized_transformed_normals, normalized_light_direction3),0.0);" \
				"vec3 diffuse3 = u_Ld3 * u_Kd * tn_dot_ld3;" \
				"vec3 reflection_vector3 = reflect(-normalized_light_direction3, normalized_transformed_normals);" \
				"vec3 specular3 = u_Ls3 * u_Ks * pow(max(dot(reflection_vector3, normalized_viewer_vector), 0.0), u_material_shininess);" \

				"phong_ads_color= (ambient1+ambient2+ambient3) + (diffuse1+diffuse2+diffuse3) + (specular1+specular2+specular3);" \

			"}" \
			"else" \
			"{" \
				"phong_ads_color = vec3(1.0, 1.0, 1.0);" \
			"}" \
			"FragColor = vec4(phong_ads_color, 1.0);" \
		"}";
    
    glShaderSource(gFragmentShaderObject_fragmentShader, 1, (const GLchar **)&fragmentShaderSourceCode_fragmentShader, NULL);
    
    // compile shader
    glCompileShader(gFragmentShaderObject_fragmentShader);
    glGetShaderiv(gFragmentShaderObject_fragmentShader, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(gFragmentShaderObject_fragmentShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(gFragmentShaderObject_fragmentShader, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    // *** SHADER PROGRAM ***
    // create
    gShaderProgramObject_fragmentShader = glCreateProgram();
    
    // attach vertex shader to shader program
    glAttachShader(gShaderProgramObject_fragmentShader, gVertexShaderObject_fragmentShader);
    
    // attach fragment shader to shader program
    glAttachShader(gShaderProgramObject_fragmentShader, gFragmentShaderObject_fragmentShader);
    
    // pre-link binding of shader program object with vertex shader position attribute
    glBindAttribLocation(gShaderProgramObject_fragmentShader, VDG_ATTRIBUTE_VERTEX, "vPosition");
    
    //PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITTH COLOR SHADER ATTRIBUTE
    glBindAttribLocation(gShaderProgramObject_fragmentShader, VDG_ATTRIBUTE_NORMAL, "vNormal");

    // link shader
    glLinkProgram(gShaderProgramObject_fragmentShader);
    iShaderProgramLinkStatus = 0;
    glGetProgramiv(gShaderProgramObject_fragmentShader, GL_LINK_STATUS, &iShaderProgramLinkStatus);
    if (iShaderProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(gShaderProgramObject_fragmentShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength>0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetProgramInfoLog(gShaderProgramObject_fragmentShader, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }





    //////////////////////////////////////////////////////////////////////////////////////////////////////
   
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
    gPerspectiveProjectionMatrix = vmath::mat4::identity();
    
    gbLight = false;

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
    
   
    gPerspectiveProjectionMatrix =vmath::perspective(45.05f,width /height, 0.1f, 100.0f);

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
	if (angleRed >= 720.0f)
		angleRed = 360.0f;
	else 
		angleRed = angleRed + 0.3f;


	if (angleBlue >= 720.0f)
		angleBlue = 360.0f;
	else 
		angleBlue = angleBlue + 0.3f;


	if (angleGreen >= 720.0f)
		angleGreen = 360.0f;
	else 
		angleGreen = angleGreen + 0.3f;


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

	

	//Decide Which Shader to use per Fragment or per Vertex shader for Lightinh
	//if (bIsVkeyPressed == true || gbLight == false)
	//{
		//GET MVP UNIFORM LOCATION
		model_matrix_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_model_matrix");
		view_matrix_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_view_matrix");
		projection_matrix_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_projection_matrix");
		L_KeyPressed_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_lighting_enabled");

		// 'L' HANDLE KEY TO TOGGLE LIGHT.

		//AMBIENT COLOR TOKEN
		La1_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_La1");

		// DIFUSE COLOR TOKEN
		Ld1_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ld1");

		// SPECULAR COLOR TOKEN
		Ls1_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ls1");

		// LIGHT POSITION TOKEN
		light_position1_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_light_position1");

		// LIGHT rotation TOKEN
		light_rotation1_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_light_rotation1");




		//AMBIENT COLOR TOKEN
		La2_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_La2");

		// DIFUSE COLOR TOKEN
		Ld2_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ld2");

		// SPECULAR COLOR TOKEN
		Ls2_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ls2");

		// LIGHT POSITION TOKEN
		light_position2_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_light_position2");


		// LIGHT rotation TOKEN
		light_rotation2_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_light_rotation2");





		//AMBIENT COLOR TOKEN
		La3_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_La3");

		// DIFUSE COLOR TOKEN
		Ld3_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ld3");

		// SPECULAR COLOR TOKEN
		Ls3_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ls3");

		// LIGHT POSITION TOKEN
		light_position3_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_light_position3");

		// LIGHT rotation TOKEN
		light_rotation3_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_light_rotation3");



		// AMBIENT REFLECTIVE TOKEN
		Ka_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ka");

		// DIFUUSE REFLECTIVE TOKEN
		Kd_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Kd");

		// SPECULAR REFLECTIVE TOKEN
		Ks_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ks");

		//SHINYNESS COLOR TOKEN
		material_shininess_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_material_shininess");


		glUseProgram(gShaderProgramObject_vertexShader);
	//}


	if (bIsFkeyPressed)
	{	
		//GET MVP UNIFORM LOCATION
		model_matrix_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_model_matrix");
		view_matrix_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_view_matrix");
		projection_matrix_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_projection_matrix");
		L_KeyPressed_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_lighting_enabled");

		// 'L' HANDLE KEY TO TOGGLE LIGHT.


	
		//AMBIENT COLOR TOKEN
		La1_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_La1");

		// DIFUSE COLOR TOKEN
		Ld1_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ld1");

		// SPECULAR COLOR TOKEN
		Ls1_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ls1");

		// LIGHT POSITION TOKEN
		light_position1_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_light_position1");

		// LIGHT rotation TOKEN
		light_rotation1_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_light_rotation1");


		//AMBIENT COLOR TOKEN
		La2_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_La2");

		// DIFUSE COLOR TOKEN
		Ld2_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ld2");

		// SPECULAR COLOR TOKEN
		Ls2_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ls2");

		// LIGHT POSITION TOKEN
		light_position2_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_light_position2");


		// LIGHT rotation TOKEN
		light_rotation2_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_light_rotation2");





		//AMBIENT COLOR TOKEN
		La3_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_La3");

		// DIFUSE COLOR TOKEN
		Ld3_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ld3");

		// SPECULAR COLOR TOKEN
		Ls3_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ls3");

		// LIGHT POSITION TOKEN
		light_position3_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_light_position3");

		// LIGHT rotation TOKEN
		light_rotation3_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_light_rotation3");
		


		// AMBIENT REFLECTIVE TOKEN
		Ka_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ka");

		// DIFUUSE REFLECTIVE TOKEN
		Kd_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Kd");

		// SPECULAR REFLECTIVE TOKEN
		Ks_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ks");

		//SHINYNESS COLOR TOKEN
		material_shininess_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_material_shininess");
		
		glUseProgram(gShaderProgramObject_fragmentShader);
	}


	/*DRAWING SPHERE*/
	vmath::mat4 modelMatrix = vmath::mat4::identity();
	vmath::mat4 viewMatrix = vmath::mat4::identity();
	vmath::mat4 rotateMatrix = vmath::mat4::identity();



	modelMatrix = vmath::translate(0.0f, 0.0f, -8.0f);


	
	// FILL DATA TO SHADERS

	if (gbLight == true)
	{	

		static GLfloat light1_ambient[] = { 0.0f,0.0f,0.0f,0.0f };
		static GLfloat light1_difuse[] = { 1.0f,0.0f,0.0f,0.0f };
		static GLfloat light1_specular[] = { 1.0f,0.0f,0.0f,0.0f };
		static GLfloat light1_position[] = { 0.0f,0.0f,0.0f,0.0f };


		static GLfloat light2_ambient[] = { 0.0f,0.0f,0.0f,0.0f };
		static GLfloat light2_difuse[] = { 0.0f,1.0f,0.0f,0.0f };
		static GLfloat light2_specular[] = { 0.0f,1.0f,0.0f,0.0f };
		static GLfloat light2_position[] = { 0.0f,0.0f,0.0f,0.0f };


		static GLfloat light3_ambient[] = { 0.0f,0.0f,0.0f,0.0f };
		static GLfloat light3_difuse[] = { 0.0f,0.0f,1.0f,0.0f };
		static GLfloat light3_specular[] = { 0.0f,0.0f,1.0f,0.0f };
		static GLfloat light3_position[] = { 0.0f,0.0f,0.0f,0.0f };


		static GLfloat material_ambient[] = { 0.0f,0.0f,0.0f,0.0f };
		static GLfloat material_difuse[] = { 1.0f,1.0f,1.0f,0.0f };
		static GLfloat material_specular[] = { 1.0f,1.0f,1.0f,0.0f };
		static GLfloat material_shinyness = 50.0f;



		glUniform1i(L_KeyPressed_uniform, 1);
		// set 'u_lighting_enabled' uniform
		
		// setting light's properties
		rotateMatrix = vmath::rotate(angleRed, 1.0f, 0.0f, 0.0f);
		light1_position[1] = angleRed;

		glUniform3fv(La1_uniform, 1, light1_ambient);
		glUniform3fv(Ld1_uniform, 1, light1_difuse);
		glUniform3fv(Ls1_uniform, 1, light1_specular);
		glUniform4fv(light_position1_uniform, 1, light1_position);
		glUniformMatrix4fv(light_rotation1_uniform, 1, GL_FALSE,rotateMatrix);


		// setting light's properties
		rotateMatrix = vmath::mat4::identity();
		rotateMatrix = vmath::rotate(angleGreen, 0.0f, 1.0f, 0.0f);
		light2_position[0] = angleGreen;

		glUniform3fv(La2_uniform, 1, light2_ambient);
		glUniform3fv(Ld2_uniform, 1, light2_difuse);
		glUniform3fv(Ls2_uniform, 1, light2_specular);
		glUniform4fv(light_position2_uniform, 1, light2_position);
		glUniformMatrix4fv(light_rotation2_uniform, 1, GL_FALSE, rotateMatrix);


		// setting light's properties
		rotateMatrix = vmath::mat4::identity();
		rotateMatrix = vmath::rotate(angleBlue, 0.0f, 0.0f, 1.0f);
		light3_position[0] = angleBlue;

		glUniform3fv(La3_uniform, 1, light3_ambient);
		glUniform3fv(Ld3_uniform, 1, light3_difuse);
		glUniform3fv(Ls3_uniform, 1, light3_specular);
		glUniform4fv(light_position3_uniform, 1, light3_position);
		glUniformMatrix4fv(light_rotation3_uniform, 1, GL_FALSE,rotateMatrix);



		// setting material's properties
		glUniform3fv(Ka_uniform, 1, material_ambient);
		glUniform3fv(Kd_uniform, 1, material_difuse);
		glUniform3fv(Ks_uniform, 1, material_specular);
		glUniform1f(material_shininess_uniform, material_shinyness);
	}
	else
	{
		// set 'u_lighting_enabled' uniform
		glUniform1i(L_KeyPressed_uniform, 0);
	}


	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);

	// BIND VAO
	

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
    // code
    int key=(int)[[theEvent characters]characterAtIndex:0];
    switch(key)
    {
        case 27: // Esc key
            //[ self release];
            //[NSApp terminate:self];
         	 [[self window]toggleFullScreen:self];
            break;

        case 'Q':
        case 'q': // quit key
            [ self release];
            [NSApp terminate:self];
            break;

       
        case 'F':
        case 'f':
            if (bIsFkeyPressed == false)
			{
				bIsFkeyPressed = true;
				bIsVkeyPressed = false;
			}
            break;

         case 'V': //for L or l
        case 'v':

			if (bIsVkeyPressed == false)
			{				
				bIsVkeyPressed = true;
				bIsFkeyPressed = false;
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
    // destroy vao

     cleanupSphereBuffers();
     cleanupSphereMeshData();
    
   //DETACH THE FRAGMENT SHADER OBJECT
	glDetachShader(gShaderProgramObject_vertexShader, gFragmentShaderObject_vertexShader);

	//DETACH THE VERTEX SHADER OBJECT
	glDetachShader(gShaderProgramObject_vertexShader, gVertexShaderObject_vertexShader);


	//DELETE VERTEX SHADER OBJECT
	glDeleteShader(gVertexShaderObject_vertexShader);
	gVertexShaderObject_vertexShader = 0;

	//DELETE FRAGMENT SHADER OBJECT
	glDeleteShader(gFragmentShaderObject_vertexShader);
	gFragmentShaderObject_vertexShader = 0;

	//DELETE SHADER PROGRAM OBJECT
	glDeleteProgram(gShaderProgramObject_vertexShader);
	gShaderProgramObject_vertexShader = 0;



	//DETACH THE FRAGMENT SHADER OBJECT
	glDetachShader(gShaderProgramObject_fragmentShader, gFragmentShaderObject_fragmentShader);

	//DETACH THE VERTEX SHADER OBJECT
	glDetachShader(gShaderProgramObject_fragmentShader, gVertexShaderObject_fragmentShader);


	//DELETE VERTEX SHADER OBJECT
	glDeleteShader(gVertexShaderObject_fragmentShader);
	gVertexShaderObject_fragmentShader = 0;

	//DELETE FRAGMENT SHADER OBJECT
	glDeleteShader(gFragmentShaderObject_fragmentShader);
	gFragmentShaderObject_fragmentShader = 0;

	//DELETE SHADER PROGRAM OBJECT
	glDeleteProgram(gShaderProgramObject_fragmentShader);
	gShaderProgramObject_fragmentShader = 0;

	//UNLINK THE PROGRAM
	glUseProgram(0);
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
