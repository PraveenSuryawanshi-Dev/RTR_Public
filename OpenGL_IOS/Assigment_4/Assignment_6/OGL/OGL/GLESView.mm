

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

typedef struct material
{
    GLfloat ambient_material[4];
    GLfloat diffuse_material[4];
    GLfloat specular_material[4];
    GLfloat shininess;
    
}MATERIAL;

@implementation GLESView
{
    
	EAGLContext *eaglContext;
	
	GLuint defaultFramebuffer;
	GLuint colorRenderBuffer;
	GLuint depthRenderBuffer;
	
	id displayLink;
	NSInteger animationFrameInterval;
    BOOL isAnimating;
    
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
    
    GLfloat oglwidth,oglheight;
    
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
        vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        
        // provide source code to shader
        const GLchar *vertexShaderSourceCode =
        "#version 300 es" \
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
        fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
        
        // provide source code to shader
        const GLchar *fragmentShaderSourceCode =
        "#version 300 es" \
        "\n" \
        "precision highp float;"\
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
                    printf( "Fragment Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
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
                    printf( "Shader Program Link Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
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
        PerspectiveProjectionMatrix = vmath::mat4::identity();
        
        bIsLKeyPressed = false;
        
        gbLight = false;
        
        lightPosition[0] =0.0f;
        lightPosition[1] =0.0f;
        
        lightPosition[2] =0.0f;
        lightPosition[3] =0.0f;
        
        [self InitializeMaterials];

        
        
		
		
		
		
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
	
    ///START USING SHADER OBJECT
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
            GLfloat newWidth = (oglwidth / 4.0f);
            GLfloat newHeight = (oglheight) / 6.0f;
            
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
    
    
    [self oglUpdate];
	
	glBindRenderbuffer(GL_RENDERBUFFER,colorRenderBuffer);
	[eaglContext presentRenderbuffer:GL_RENDERBUFFER];
}


-(void) oglUpdate
{
    if (sphere_angle >= 720.0f)
        sphere_angle = 360.0f;
    else
        sphere_angle = sphere_angle + 1.0f;
    
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
    oglwidth=width;
    oglheight=height;
    PerspectiveProjectionMatrix=vmath::perspective(45.05f, fwidth/fheight, 0.1f, 100.0f);
    
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
    
    lightPosition[0] = 0.0f;
    lightPosition[1] = 0.0f;
    lightPosition[2] = 0.0f;
    
    static int tap_count= 1;
    
    if (tap_count == 4)
    {
        tap_count = 1;
    }
    
    if (tap_count == 1)
    {
        gIsXKeyPressed = true;
        gIsYKeyPressed= false;
        gIsZKeyPressed= false;
        
    }
    if (tap_count == 2)
    {
        gIsYKeyPressed = true;
        gIsXKeyPressed = false;
        gIsZKeyPressed = false;
        
    }
    if (tap_count == 3)
    {
        gIsZKeyPressed = true;
        gIsXKeyPressed = false;
        gIsYKeyPressed = false;
    }
    
    tap_count++;
    // code
    
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
