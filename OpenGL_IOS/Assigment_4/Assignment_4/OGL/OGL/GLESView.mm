

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "GLESView.h"
#import "vmath.h"

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
    
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;
    GLuint shaderProgramObject;
    
    GLuint model_matrix_uniform;
    GLuint view_matrix_uniform;
    GLuint projection_matrix_uniform;
    vmath::mat4 gPerspectiveProjectionMatrix;
    
    
    
    GLuint La1_uniform;
    GLuint Ld1_uniform;
    GLuint Ls1_uniform;
    GLuint light_position1_uniform;
    
    GLuint La2_uniform;
    GLuint Ld2_uniform;
    GLuint Ls2_uniform;
    GLuint light_position2_uniform;
    
    
    GLuint L_KeyPressed_uniform;
    
    GLuint Ka_uniform;
    GLuint Kd_uniform;
    GLuint Ks_uniform;
    GLuint material_shininess_uniform;
    
    GLuint gVao_pyramid;
    GLuint gVbo_pyramid_position;
    GLuint gVbo_pyramid_normal;
    float anglePyramid;
    
    bool gbLight;
    
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
        "uniform vec3 u_La1;" \
        "uniform vec3 u_Ld1;" \
        "uniform vec3 u_Ls1;" \
        "uniform vec4 u_light_position1;" \
        "uniform vec3 u_La2;" \
        "uniform vec3 u_Ld2;" \
        "uniform vec3 u_Ls2;" \
        "uniform vec4 u_light_position2;" \
        "uniform vec3 u_Ka;" \
        "uniform vec3 u_Kd;" \
        "uniform vec3 u_Ks;" \
        "uniform float u_material_shininess;" \
        "out vec3 phong_ads_color;" \
        "void main(void)" \
        "{" \
        "if(u_lighting_enabled==1)" \
        "{" \
        "vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" \
        "vec3 transformed_normals=normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
        
        "vec3 light_direction1 = normalize(vec3(u_light_position1) - eye_coordinates.xyz);" \
        "float tn_dot_ld1 = max(dot(transformed_normals, light_direction1),0.0);" \
        "vec3 ambient1 = u_La1 * u_Ka;" \
        "vec3 diffuse1 = u_Ld1* u_Kd * tn_dot_ld1;" \
        "vec3 reflection_vector1 = reflect(-light_direction1, transformed_normals);" \
        "vec3 viewer_vector = normalize(-eye_coordinates.xyz);" \
        "vec3 specular1 = u_Ls1 * u_Ks * pow(max(dot(reflection_vector1, viewer_vector), 0.0), u_material_shininess);" \
        
        "vec3 light_direction2 = normalize(vec3(u_light_position2) - eye_coordinates.xyz);" \
        "float tn_dot_ld2 = max(dot(transformed_normals, light_direction2),0.0);" \
        "vec3 ambient2 = u_La2 * u_Ka;" \
        "vec3 diffuse2 = u_Ld2* u_Kd * tn_dot_ld2;" \
        "vec3 reflection_vector2 = reflect(-light_direction2, transformed_normals);" \
        "vec3 specular2 = u_Ls2 * u_Ks * pow(max(dot(reflection_vector2, viewer_vector), 0.0), u_material_shininess);" \
        
        
        "phong_ads_color= (ambient1+ambient2) +( diffuse1+diffuse2 )+ (specular1+specular2);" \
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
                    printf("Shader Program Link Log : %s\n", szInfoLog);
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
        La1_uniform = glGetUniformLocation(shaderProgramObject, "u_La1");
        
        // DIFUSE COLOR TOKEN
        Ld1_uniform= glGetUniformLocation(shaderProgramObject, "u_Ld1");
        
        // SPECULAR COLOR TOKEN
        Ls1_uniform= glGetUniformLocation(shaderProgramObject, "u_Ls1");
        
        // LIGHT POSITION TOKEN
        light_position1_uniform = glGetUniformLocation(shaderProgramObject, "u_light_position1");
        
        
        
        //AMBIENT COLOR TOKEN
        La2_uniform = glGetUniformLocation(shaderProgramObject, "u_La2");
        
        // DIFUSE COLOR TOKEN
        Ld2_uniform = glGetUniformLocation(shaderProgramObject, "u_Ld2");
        
        // SPECULAR COLOR TOKEN
        Ls2_uniform = glGetUniformLocation(shaderProgramObject, "u_Ls2");
        
        // LIGHT POSITION TOKEN
        light_position2_uniform = glGetUniformLocation(shaderProgramObject, "u_light_position2");
        
        
        
        
        // AMBIENT REFLECTIVE TOKEN
        Ka_uniform = glGetUniformLocation(shaderProgramObject, "u_Ka");
        
        // DIFUUSE REFLECTIVE TOKEN
        Kd_uniform = glGetUniformLocation(shaderProgramObject, "u_Kd");
        
        // SPECULAR REFLECTIVE TOKEN
        Ks_uniform = glGetUniformLocation(shaderProgramObject, "u_Ks");
        
        //SHINYNESS COLOR TOKEN
        material_shininess_uniform = glGetUniformLocation(shaderProgramObject, "u_material_shininess");;
        
        
        
        // *** vertices, colors, shader attribs, vbo, vao initializations ***
        
        
        
        const GLfloat pyramidVertices[] =
        {
            // Front face
            0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            
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
        const GLfloat PyramidNormals[] =
        {
            // Front face
            0.0f, 0.447214f, 0.894427f,
            0.0f, 0.447214f, 0.894427f,
            0.0f, 0.447214f, 0.894427f,
            
            //Back Face
            0.0f, 0.4472141f, -0.894427f,
            0.0f, 0.4472141f, -0.894427f,
            0.0f, 0.4472141f, -0.894427f,
            
            //Right face
            0.894427f, 0.4472141f, 0.0f,
            0.894427f, 0.4472141f, 0.0f,
            0.894427f, 0.4472141f, 0.0f,
            //Left face
            
            -0.894427f, 0.4472141f, 0.0f,
            -0.894427f, 0.4472141f, 0.0f,
            -0.894427f, 0.4472141f, 0.0f,
        };
        
        
        
        
        /*Pyramid*/
        glGenVertexArrays(1, &gVao_pyramid);
        glBindVertexArray(gVao_pyramid);
        
        /*POSITION*/
        glGenBuffers(1, &gVbo_pyramid_position);
        glBindBuffer(GL_ARRAY_BUFFER, gVbo_pyramid_position);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
        
        glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        
        glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        /*END OF Pyramid*/
        
        /*normals*/
        glGenBuffers(1, &gVbo_pyramid_normal);
        glBindBuffer(GL_ARRAY_BUFFER, gVbo_pyramid_normal);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PyramidNormals), PyramidNormals, GL_STATIC_DRAW);
        
        glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        
        glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        /*END OF normals*/
        
        
        glBindVertexArray(0);
        /***/
        
        
        
        gbLight = false;
        
        
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
    
    static GLfloat left_lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
    static GLfloat left_lightDiffuse[] = { 1.0f,0.0f,0.0f,0.0f };
    static GLfloat left_lightSpecular[] = { 0.0f,0.0f,1.0f,0.0f };
    static GLfloat left_lightPosition[] = { -2.0f,1.0f,1.0f,0.0f };
    
    
    static GLfloat right_lightAmbient[] = { 0.0f,0.0f,0.0f,0.0f };
    static GLfloat right_lightDiffuse[] = { 0.0f,0.0f,1.0f,0.0f };
    static GLfloat right_lightSpecular[] ={ 1.0f,0.0f,0.0f,0.0f };
    static GLfloat right_lightPosition[] = { 2.0f,1.0f,1.0f,0.0f };
    
    
    
    static GLfloat material_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
    static GLfloat material_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
    static GLfloat material_specular[] = { 1.0f,1.0f,1.0f,1.0f };
    static GLfloat material_shininess = 50.0f;
    
	//code
	[EAGLContext setCurrentContext:eaglContext];
	
	glBindFramebuffer(GL_FRAMEBUFFER,defaultFramebuffer);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
    //START USING SHADER OBJECT
    glUseProgram(shaderProgramObject);
    
    
    
    if (gbLight == true)
    {
        
        
        // set 'u_lighting_enabled' uniform
        glUniform1i(L_KeyPressed_uniform, 1);
        
        // setting light's properties
        glUniform3fv(La1_uniform, 1, left_lightAmbient);
        glUniform3fv(Ld1_uniform, 1, left_lightDiffuse);
        glUniform3fv(Ls1_uniform, 1, left_lightSpecular);
        glUniform4fv(light_position1_uniform, 1, left_lightPosition);
        
        
        // setting light's properties
        glUniform3fv(La2_uniform, 1, right_lightAmbient);
        glUniform3fv(Ld2_uniform, 1, right_lightDiffuse);
        glUniform3fv(Ls2_uniform, 1, right_lightSpecular);
        glUniform4fv(light_position2_uniform, 1, right_lightPosition);
        
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
    vmath::mat4 rotateMatrix = vmath::mat4::identity();
    
    
    modelMatrix = vmath::translate(0.0f, 0.0f, -5.0f);
    
    rotateMatrix = vmath::rotate<GLfloat>(anglePyramid, 0.0f,1.0f, 0.0f);
    
    modelMatrix = modelMatrix * rotateMatrix;
    
    glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
    
    // BIND VAO
    glBindVertexArray(gVao_pyramid);
    
    //START DRAWING
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVao_pyramid);
    glDrawArrays(GL_TRIANGLES, 0, 12);
    
    // UNBING VAO
    glBindVertexArray(0);
    
    //STOP USING SHADER
    glUseProgram(0);

    
    [self oglUpdate];
	
	glBindRenderbuffer(GL_RENDERBUFFER,colorRenderBuffer);
	[eaglContext presentRenderbuffer:GL_RENDERBUFFER];
}


-(void) oglUpdate
{
    if (anglePyramid >= 360.0f)
        anglePyramid = 0.0f;
    
    anglePyramid = anglePyramid + 0.5f;
    
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
    //DESTROY VAO
    if (gVao_pyramid)
    {
        glDeleteVertexArrays(1, &gVao_pyramid);
        gVao_pyramid = 0;
    }
    
    //DESTROY VBO
    if (gVbo_pyramid_position)
    {
        glDeleteVertexArrays(1, &gVbo_pyramid_position);
        gVbo_pyramid_position = 0;
    }
    if (gVbo_pyramid_normal)
    {
        glDeleteVertexArrays(1, &gVbo_pyramid_normal);
        gVbo_pyramid_normal = 0;
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
