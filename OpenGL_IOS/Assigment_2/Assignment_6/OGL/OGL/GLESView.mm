

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
                    printf("Fragment Shader Compilation Log : %s\n", szInfoLog);
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
                    printf( "Shader Program Link Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
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
    
    [self oglUpdate];
	
	glBindRenderbuffer(GL_RENDERBUFFER,colorRenderBuffer);
	[eaglContext presentRenderbuffer:GL_RENDERBUFFER];
}


-(void) oglUpdate
{
    if (anglePyramid >= 360.0f) anglePyramid = 0.0f;
    anglePyramid = anglePyramid + 0.5f;
    
    if (angleCube >= 360) angleCube = 0.0f;
    angleCube = angleCube + 0.5f;
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
    
    PerspectiveProjectionMatrix = vmath::perspective(45.05f,fwidth /fheight, 0.1f, 100.0f);
    
    
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
