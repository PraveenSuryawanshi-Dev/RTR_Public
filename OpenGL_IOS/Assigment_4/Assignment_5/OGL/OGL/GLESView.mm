

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "GLESView.h"
#import "vmath.h"
#import "MySphere.h"

enum
{
    VDG_ATTRIBUTE_VERTEX = 0,
    VDG_ATTRIBUTE_COLOR,
    VDG_ATTRIBUTE_NORMAL,
    VDG_ATTRIBUTE_TEXTURE0,
};

@implementation GLESView
{
    
	EAGLContext *eaglContext;
	
	GLuint defaultFramebuffer;
	GLuint colorRenderBuffer;
	GLuint depthRenderBuffer;
	
	id displayLink;
	NSInteger animationFrameInterval;
    BOOL isAnimating;
    
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

-(id) initWithFrame:(CGRect)frame
{
     
	self = [super initWithFrame:frame];
	
	if(self)
	{
		//initialize the code here
		
        CAEAGLLayer *eaglLayer=(CAEAGLLayer *)super.layer;
        
        eaglLayer.opaque=YES;
        eaglLayer.drawableProperties=[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:FALSE],
                                      kEAGLDrawablePropertyRetainedBacking,kEAGLColorFormatRGBA8,kEAGLDrawablePropertyColorFormat,nil];
        
        eaglContext=[[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES3];
        if(eaglContext==nil)
        {
            [self release];
            return(nil);
        }
        [EAGLContext setCurrentContext:eaglContext];
        
        glGenFramebuffers(1,&defaultFramebuffer);
        glGenRenderbuffers(1,&colorRenderBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER,defaultFramebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER,colorRenderBuffer);
        
        [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,colorRenderBuffer);
        
        GLint backingWidth;
        GLint backingHeight;
        glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_WIDTH,&backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_HEIGHT,&backingHeight);
        
        glGenRenderbuffers(1,&depthRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER,depthRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,backingWidth,backingHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthRenderBuffer);
        
		
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
		{
			printf("Failed To Create Complete Framebuffer Object %x\n",glCheckFramebufferStatus(GL_FRAMEBUFFER));
            glDeleteFramebuffers(1,&defaultFramebuffer);
            glDeleteRenderbuffers(1,&colorRenderBuffer);
            glDeleteRenderbuffers(1,&depthRenderBuffer);
			
			return (nil);
		}
		
		printf("Renderer : %s | GL Version : %s | GLSL Version : %s\n",glGetString(GL_RENDERER),glGetString(GL_VERSION),glGetString(GL_SHADING_LANGUAGE_VERSION));

        // hard coded initializations
        isAnimating=NO;
        animationFrameInterval=60; // default since iOS 8.2
        
        
        
        
        // *** VERTEX SHADER ***
        // create shader
        gVertexShaderObject_vertexShader = glCreateShader(GL_VERTEX_SHADER);
        
        // provide source code to shader
        const GLchar *vertexShaderSourceCode_vertexShader =
        "#version 300 es" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;" \
        
        "uniform mediump int u_lighting_enabled;" \
        
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
                    printf("Vertex Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
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
        "#version 300 es" \
        "\n" \
        "precision highp float;"\
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
                    printf("Fragment Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
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
                    printf("Shader Program Link Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
                }
            }
        }
        
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        
        
        // *** VERTEX SHADER ***
        // create shader
        gVertexShaderObject_fragmentShader = glCreateShader(GL_VERTEX_SHADER);
        
        // provide source code to shader
        const GLchar *vertexShaderSourceCode_fragmentShader =
        "#version 300 es" \
        "\n" \
        "uniform mediump int u_lighting_enabled;" \
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
                    printf( "Vertex Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
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
        "#version 300 es" \
        "\n" \
        "precision highp float;"\
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
                    printf( "Fragment Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
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
                    printf( "Shader Program Link Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
                }
            }
        }
        
        
        
        
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        
        /*VERTICES ,COLOR ,NORMAL  INITIALIZATION*/
        makeSphere(2.0f,30.0f,30.0f);
        
        
        
        //glClearDepth(1.0f);
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

        
        
		
		
		
		
		//Gesture Recogination

		//Tap gesture code
		
		UITapGestureRecognizer *singleTapGestureRecognizer= [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
		[singleTapGestureRecognizer setNumberOfTapsRequired:1];
        [singleTapGestureRecognizer setNumberOfTouchesRequired:1];
		[singleTapGestureRecognizer setDelegate:self];
		[self addGestureRecognizer:singleTapGestureRecognizer];
		
		
		UITapGestureRecognizer *doubleTapGestureRecognizer= [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
		[doubleTapGestureRecognizer setNumberOfTapsRequired:2];
		[doubleTapGestureRecognizer setNumberOfTouchesRequired:1];
		[doubleTapGestureRecognizer setDelegate:self];
		[self addGestureRecognizer:doubleTapGestureRecognizer];
		
		
		//this will allow to differentiate between single tap and double tap.
		[singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
		
		
		//swipe gesture
		UISwipeGestureRecognizer *swipeGestureRecognizer = [[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
		[self addGestureRecognizer:swipeGestureRecognizer];
		
		//long-press gesture
		UILongPressGestureRecognizer *longPressGestureRecognizer = [[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
		[self addGestureRecognizer:longPressGestureRecognizer];
		
		}
	return (self);
}

-(GLuint)loadTextureFromBMPFile:(NSString *)texFileName :(NSString *)extension
{
    NSString *textureFileNameWithPath=[[NSBundle mainBundle]pathForResource:texFileName ofType:extension];
    
    UIImage *bmpImage=[[UIImage alloc]initWithContentsOfFile:textureFileNameWithPath];
    if (!bmpImage)
    {
        NSLog(@"can't find %@", textureFileNameWithPath);
        return(0);
    }
    
    CGImageRef cgImage=bmpImage.CGImage;
    
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


//only override drawRect: if you perform custom drawing.
//an empty impelementation adverserly affects performance during animation.
/*
-(void) drawRect:(CGRect)rect
{
	//black background
	UIColor *fillcolor = [UIColor blackColor];
	[fillcolor set];
	UIRectFill(rect);
	
	
	 // dictionary with kvc
    NSDictionary *dictionaryForTextAttributes=[NSDictionary dictionaryWithObjectsAndKeys:
                                               [UIFont fontWithName:@"Helvetica" size:24], NSFontAttributeName,
                                               [UIColor greenColor], NSForegroundColorAttributeName,
                                               nil];
    
    CGSize textSize=[centralText sizeWithAttributes:dictionaryForTextAttributes];
    
    CGPoint point;
    point.x=(rect.size.width/2)-(textSize.width/2);
    point.y=(rect.size.height/2)-(textSize.height/2)+12;
    
    [centralText drawAtPoint:point withAttributes:dictionaryForTextAttributes];
}
*/

+(Class) layerClass
{
	//code
	return ([CAEAGLLayer class]);
}

-(void) drawView:(id)sender
{
	//code
	[EAGLContext setCurrentContext:eaglContext];
	
	glBindFramebuffer(GL_FRAMEBUFFER,defaultFramebuffer);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
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
    

    
    
    [self oglUpdate];
	
	glBindRenderbuffer(GL_RENDERBUFFER,colorRenderBuffer);
	[eaglContext presentRenderbuffer:GL_RENDERBUFFER];
}


-(void) oglUpdate
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

-(void) layoutSubviews
{
	//code
	 // code
    GLint width;
    GLint height;
    
    glBindRenderbuffer(GL_RENDERBUFFER,colorRenderBuffer);
    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_WIDTH,&width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_HEIGHT,&height);
    
    glGenRenderbuffers(1,&depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER,depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,width,height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthRenderBuffer);
    
    glViewport(0,0,width,height);
    
    //glOrtho(left,right,bottom,top,near,far);
    GLfloat fwidth = (GLfloat)width;
    GLfloat fheight = (GLfloat)height;
    
    gPerspectiveProjectionMatrix=vmath::perspective(45.05f, fwidth/fheight, 0.1f, 100.0f);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Failed To Create Complete Framebuffer Object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
    
    [self drawView:nil];
}


-(void) startAnimation
{
	if (!isAnimating)
    {
        displayLink=[NSClassFromString(@"CADisplayLink")displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink setPreferredFramesPerSecond:animationFrameInterval];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

        isAnimating=YES;
        
        
    }
}


-(void) stopAnimation
{
	if(isAnimating)
	{
	   [displayLink invalidate];
        displayLink=nil;
        
        isAnimating=NO;
	
	}
}

// to become first responder
-(BOOL)acceptsFirstResponder
{
    // code
    return(YES);
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    // code
/*
    centralText = @"'touchesBegan' Event Occured";
    [self setNeedsDisplay]; // repainting
*/
}

-(void)onSingleTap:(UITapGestureRecognizer *)gr
{
    // code
    static int cnt=0;
    if(cnt %2 == 0)
        bIsFkeyPressed = false;
    else
        bIsFkeyPressed = true;
    cnt++;
    
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr
{
    // code
    if (gbLight == false)
    {
        gbLight = true;
    }
    else
    {
        gbLight = false;
    }
    
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr
{
    // code
    [self release];
    exit(0);
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gr
{
    // code
}

- (void)dealloc
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
    

    

    
    
    
    // code
     if(depthRenderBuffer)
    {
        glDeleteRenderbuffers(1,&depthRenderBuffer);
        depthRenderBuffer=0;
    }
    
    if(colorRenderBuffer)
    {
        glDeleteRenderbuffers(1,&colorRenderBuffer);
        colorRenderBuffer=0;
    }
    
    if(defaultFramebuffer)
    {
        glDeleteFramebuffers(1,&defaultFramebuffer);
        defaultFramebuffer=0;
    }
    
    if([EAGLContext currentContext]==eaglContext)
    {
        [EAGLContext setCurrentContext:nil];
    }
    [eaglContext release];
    eaglContext=nil;
    
    [super dealloc];
}

@end
