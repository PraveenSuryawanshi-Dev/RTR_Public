package com.rtr.Perspective3dCubeLight;

//import android.widget.TextView; /*NEED FOR CREATE TEXT LABEL*/
//import android.graphics.Color; /*NEED FOR COLOR THE TEXT*/
//import android.view.Gravity; /*NEED FOR SETTING TEXT TO CENTER*/
import android.content.Context; /*USED TO TYPECAST WINDOW OBJECT*/

/**/
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

/*OPENGL PACKAGES*/
import android.opengl.GLES30; /*OPENGL PACKAGE 3.0*/
import android.opengl.GLSurfaceView; /*USED FOR OPENGL SURFACE TO RENDER*/

/*EVENS PACKAGES*/
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

/*FOR VBO*/
import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.nio.ByteOrder;

/*FOR MATRIX*/
import android.opengl.Matrix;


public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener , OnDoubleTapListener
{ 
	
	private final Context _context; 
	
	private GestureDetector _gestureDetectort;
	
	private int vertexShaderObject;
	
	private int fragmentShaderObject;
	
	private int shaderProgramObject;
	
	private int []gVao_cube = new int[1];

	private int []gVbo_cube_position = new int[1];

	private int []gVbo_cube_normal = new int[1];

	private int gModelViewMatrixUniform;
	
	private int gProjectionMatrixUniform;

	private int gLdUniform;
	
	private int gKdUniform;

	private int gLightPositionUniform;
	
	private int gbLight_doubleTab_uniform;

	private float  perspectiveProjectionMatrix[]=new float[16]; // 4x4 matrix

	private float gAngle = 0.0f;

	private int gbAnimate_singleTap;
	
	private int gbLight_doubleTab;
	
	
	GLESView(Context context)
	{
		super(context);
			
		//ASSIGN CONTEXT
		_context = context;
		
		//SET OpenGL Context nearer what device 
		setEGLContextClientVersion(3);
		
		//Tell opengl to renderer function implemented in this class
		setRenderer(this);
		
		//Draw only when surface changes
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
		
		
		//ADD EVENT HANDLERS TO OUT LABEL
		_gestureDetectort = new GestureDetector(context,this,null,false);
		_gestureDetectort.setOnDoubleTapListener(this);
		
	
	}
	
	private void Update()
	{
		gAngle = gAngle + 0.3f;

		if (gAngle >= 360.0f)
			gAngle = gAngle - 360.0f;
	}
	//Intialize OpenGL here
	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		//GET THE OPENGL VERSION
		String version = gl.glGetString(GL10.GL_VERSION);
		
		//PRINT THE OPENGL VERSION ON CONSOLE
		System.out.println("PRAVEEN : OpenGL Version : "+version);
		
		//SHADING LANGUAGE COMPILER VERSION
		
		String glslVersion  = gl.glGetString(GLES30.GL_SHADING_LANGUAGE_VERSION);
		
		System.out.println("PRAVEEN : Shading language version : "+glslVersion);
			
		initialize(gl);
	}
	
	
	//On OpenGL window Resized.
	@Override
	public void onSurfaceChanged(GL10 unused,int width, int height)
	{
		resize(width,height);
	}
	
	//Draw Call on GL Surface view.
	@Override
	public void onDrawFrame(GL10 unsed)
	{
			draw();
	}
	


	// Handling 'onTouchEvent' Is The Most IMPORTANT,
    // Because It Triggers All Gesture And Tap Events
	@Override
	public boolean onTouchEvent(MotionEvent e)
	{
		int action = e.getAction();
		
		 if(!_gestureDetectort.onTouchEvent(e))
            super.onTouchEvent(e);
        return(true);
	}
	
	
	// abstract method from OnDoubleTapListener so must be implemented
	@Override
	public boolean onDoubleTap(MotionEvent e)
	{
		System.out.println("Double Tapped");

		 // code
        gbLight_doubleTab ++;
        if(gbLight_doubleTab > 1)
            gbLight_doubleTab=0;
        return(true);
		

	}
	
	
	 // abstract method from OnDoubleTapListener so must be implemented
	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
        // Do Not Write Any code Here Because Already Written 'onDoubleTap'
		return true;
	}
	
	 // abstract method from OnDoubleTapListener so must be implemented
	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
		 // code
         gbAnimate_singleTap++;
        if(gbAnimate_singleTap > 1)
            gbAnimate_singleTap=0;
		
		System.out.println("Single Tapped");
		return false;
	}
	
	 // abstract method from OnGestureListener so must be implemented
	@Override
	public boolean onDown(MotionEvent e)
	{
		// Do Not Write Any code Here Because Already Written 'onSingleTapConfirmed'
		return true;
	}
	
	 
	 // abstract method from OnGestureListener so must be implemented
	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2 ,float velocityX, float velocityY)
	{
		return true;
	}
	
	    // abstract method from OnGestureListener so must be implemented
    @Override
    public void onLongPress(MotionEvent e)
    {
        System.out.println("Long Pressed");
    }
    
    // abstract method from OnGestureListener so must be implemented
    @Override
    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
    {
        System.out.println("Scrolled");
		uninitialize();
		System.exit(0);
        return(true);
    }
    
    // abstract method from OnGestureListener so must be implemented
    @Override
    public void onShowPress(MotionEvent e)
    {
    }
    
    // abstract method from OnGestureListener so must be implemented
    @Override
    public boolean onSingleTapUp(MotionEvent e)
    {
        return(true);
    }
	
	private void initialize(GL10 gl)
    {
		//VERTEX SHADER 
		
		vertexShaderObject = GLES30.glCreateShader(GLES30.GL_VERTEX_SHADER);
		
		final String vertexshadersourcecode = String.format(
		 "#version 300 es"+
         "\n"+
		"in vec4 vPosition;"+
		"in vec3 vNormal;"+
		"uniform mat4 u_model_view_matrix;"+
		"uniform mat4 u_projection_matrix;"+
		"uniform mediump int u_double_tap;"+
		"uniform vec3 u_Ld;"+
		"uniform vec3 u_Kd;" +
		"uniform vec4 u_light_position;"+
		"out vec3 diffuse_light;"+
		"void main(void)"+
		"{"+
			"if(u_double_tap == 1)"+
			"{"+
				"vec4 eyeCoordinates  = u_model_view_matrix * vPosition;"+
				"vec3 tnorm = normalize(mat3(u_model_view_matrix)*vNormal);"+
				"vec3 s = normalize(vec3(u_light_position - eyeCoordinates)); "+
				"diffuse_light =u_Ld * u_Kd * max(dot(s,tnorm),0.0);"+
			"}"+
		"gl_Position = u_projection_matrix *u_model_view_matrix* vPosition;"+
		"}"
	);
	
	// provide source code to vertex shader object.
	GLES30.glShaderSource(vertexShaderObject,vertexshadersourcecode);
	
	//Compile Shader
	GLES30.glCompileShader(vertexShaderObject);
	
	//Check for compilation error
	
	int[] ishadercompilationstatus = new int[1];
	int[] iinfologlength = new int[1];
	
	String szInfoLog = null;
	
	GLES30.glGetShaderiv(vertexShaderObject,GLES30.GL_COMPILE_STATUS,ishadercompilationstatus,0);
	
	if(ishadercompilationstatus[0] == GLES30.GL_FALSE)
	{
		GLES30.glGetShaderiv(vertexShaderObject, GLES30.GL_INFO_LOG_LENGTH, iinfologlength, 0);
		if(iinfologlength[0] > 0 )
		{
			szInfoLog = GLES30.glGetShaderInfoLog(vertexShaderObject);
			System.out.println("PRAVEEN :"+szInfoLog);
			uninitialize();
			System.exit(0);
		}
	}	
	//END OF VERTEX SHADER
	
	
	//CREATE FRAGMENT SHADER 
	fragmentShaderObject = GLES30.glCreateShader(GLES30.GL_FRAGMENT_SHADER);
		
		final String fragmentshadersourcecode = String.format(
		"#version 300 es"+
		"\n"+
		"precision highp float;"+
		"in vec3 diffuse_light;"+
		"out vec4 FragColor;"+
		"uniform int u_double_tap;"+
		"void main(void)"+
		"{"+
		"vec4 color;"+
			"if(u_double_tap == 1)"+
			"{"+
				"color = vec4(diffuse_light,1.0);"+
			"}"+
			"else"+
			"{"+
				"color = vec4(1.0,1.0,1.0,1.0);"+
			"}"+
		"FragColor = color;"+
		"}"
	);
	
	// provide source code to vertex shader object.
	GLES30.glShaderSource(fragmentShaderObject,fragmentshadersourcecode);
	
	//Compile Shader
	GLES30.glCompileShader(fragmentShaderObject);
	
	//Check for compilation error
	
	ishadercompilationstatus[0]= 0;
	iinfologlength[0]= 0;
	szInfoLog = null;
	
	GLES30.glGetShaderiv(fragmentShaderObject,GLES30.GL_COMPILE_STATUS,ishadercompilationstatus,0);
	
	if(ishadercompilationstatus[0] == GLES30.GL_FALSE)
	{
		GLES30.glGetShaderiv(fragmentShaderObject,GLES30.GL_INFO_LOG_LENGTH,iinfologlength,0);
		if(iinfologlength[0] > 0 )
		{
			szInfoLog = GLES30.glGetShaderInfoLog(fragmentShaderObject);
			System.out.println("PRAVEEN :"+szInfoLog);
			uninitialize();
			System.exit(0);
		}
	}	
	
	//END of Fragment Shader
  
  
	//CREATE SHADER PROGRAM OBJECT*/
	 shaderProgramObject = GLES30.glCreateProgram();
	 
	 //ATTACH VERTEX SHADER OBJECT
	 GLES30.glAttachShader(shaderProgramObject, vertexShaderObject);
	 
	//ATTACH FRAGMENT SHADER OBJECT
	GLES30.glAttachShader(shaderProgramObject, fragmentShaderObject);
	
	
	//PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITH VERTEX SHADER POSITION ATTRIBUTE
	GLES30.glBindAttribLocation(shaderProgramObject, GLESMacros.VDG_ATTRIBUTE_POSITION, "vPosition");
	
	//PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITTH Normal ATTRIBUTE
	GLES30.glBindAttribLocation(shaderProgramObject, GLESMacros.VDG_ATTRIBUTE_NORMAL, "vNormal");
	//

		
	//LINK TWO SHADER TOGHER TO ONE SHADER PROGRAM OBJECT
	GLES30.glLinkProgram(shaderProgramObject);
	//Check for compilation error
	
	ishadercompilationstatus[0]= 0;
	iinfologlength[0]= 0;
	szInfoLog = null;
	
	GLES30.glGetShaderiv(shaderProgramObject,GLES30.GL_COMPILE_STATUS,ishadercompilationstatus,0);
	
	if(ishadercompilationstatus[0] == GLES30.GL_FALSE)
	{
		GLES30.glGetShaderiv(shaderProgramObject,GLES30.GL_INFO_LOG_LENGTH,iinfologlength,0);
		if(iinfologlength[0] > 0 )
		{
			szInfoLog = GLES30.glGetShaderInfoLog(shaderProgramObject);
			System.out.println("PRAVEEN :"+szInfoLog);
			uninitialize();
			System.exit(0);
		}
	}	
		 
	 //GET MVP UNIFORM LOCATION
	gModelViewMatrixUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_model_view_matrix");

	gProjectionMatrixUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_projection_matrix");

	gbLight_doubleTab_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_double_tap");

	gLdUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ld");

	gKdUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Kd");

	gLightPositionUniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_light_position");
	//
	 
	 //VERTICES COLOR SHADER ATTRIBS VBO , VAO INITIALIZE
	  float cubeVertices[]= new float[]
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

	
	float cubeNormals[]= new float[]
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
	
	
		/*CUBE POSITION*/
		GLES30.glGenVertexArrays(1,gVao_cube,0);
		GLES30.glBindVertexArray(gVao_cube[0]);
		
		/*POSITIONS*/
		GLES30.glGenBuffers(1,gVbo_cube_position ,0);
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,gVbo_cube_position[0]);
		
		ByteBuffer byteBuffer=ByteBuffer.allocateDirect(cubeVertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        
		FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(cubeVertices);
        verticesBuffer.position(0);
		
		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,
                            cubeVertices.length * 4,
                            verticesBuffer,
                            GLES30.GL_STATIC_DRAW);
        
        GLES30.glVertexAttribPointer(GLESMacros.VDG_ATTRIBUTE_POSITION,
                                     3,
                                     GLES30.GL_FLOAT,
                                     false,0,0);
        
        GLES30.glEnableVertexAttribArray(GLESMacros.VDG_ATTRIBUTE_POSITION);
        
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);
		/**/
		
		/*Normal*/
		GLES30.glGenBuffers(1,gVbo_cube_normal ,0);
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,gVbo_cube_normal[0]);
		
		byteBuffer =ByteBuffer.allocateDirect(cubeNormals.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        
		verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(cubeNormals);
        verticesBuffer.position(0);
		
		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,
                            cubeNormals.length * 4,
                            verticesBuffer,
                            GLES30.GL_STATIC_DRAW);
        
        GLES30.glVertexAttribPointer(GLESMacros.VDG_ATTRIBUTE_NORMAL,3,GLES30.GL_FLOAT,false,0,0);
        
        GLES30.glEnableVertexAttribArray(GLESMacros.VDG_ATTRIBUTE_NORMAL);
        
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);
		/**/
		
        GLES30.glBindVertexArray(0);
		/**/
		
		
	
        // ENABLE DEPTH TEST
        GLES30.glEnable(GLES30.GL_DEPTH_TEST);
       
        GLES30.glDepthFunc(GLES30.GL_LEQUAL);
        
		// CULLING BACK FACES ALWAYS FOR PERFORMANCE.
        GLES30.glEnable(GLES30.GL_CULL_FACE);
	
		// Set the background frame color
		GLES30.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			
		
		
		 // initialization
		gbAnimate_singleTap = 0;
	
		gbLight_doubleTab = 0;
 
 		Matrix.setIdentityM(perspectiveProjectionMatrix,0);
    }
	
	private void resize(int width ,int height)
	{
		if(height == 0)
			height = 1;
		
		//Set Viewport on window.
		GLES30.glViewport(0,0,width , height);
		
		Matrix.perspectiveM(perspectiveProjectionMatrix,0,45.05f,(float)width/(float)height,0.1f, 100.0f);
	}
	
	public void draw()
	{
		 // CLEAR BUFFERS
        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT | GLES30.GL_DEPTH_BUFFER_BIT);
        
        // START USING SHADER PROGRAM OBJECT
        GLES30.glUseProgram(shaderProgramObject);

        // OpenGL-ES drawing
		
		/**/
		if (gbLight_doubleTab == 1)
		{
			GLES30.glUniform1i(gbLight_doubleTab_uniform, 1);

			GLES30.glUniform3f(gLdUniform, 1.0f, 1.0f, 1.0f);
			GLES30.glUniform3f(gKdUniform, 0.5f, 0.5f, 0.5f);

			float []lightPosition = { 0.0f ,0.0f , 2.0f, 1.0f };
			GLES30.glUniform4fv(gLightPositionUniform, 1, lightPosition ,0);
		}
		else
		{
			GLES30.glUniform1i(gbLight_doubleTab_uniform, 0);
		}
		/**/
		
		
        float modelMatrix[]=new float[16];
        float modelViewMatrix[]=new float[16];
        float rotationMatrix[]=new float[16];
        
        // CUBE CODE
        // set modelMatrix, modelViewMatrix, rotation matrices to identity matrix
        Matrix.setIdentityM(modelMatrix,0);
        Matrix.setIdentityM(modelViewMatrix,0);
        Matrix.setIdentityM(rotationMatrix,0);

        Matrix.translateM(modelMatrix,0,0.0f,0.0f,-6.0f);
        
        Matrix.setRotateM(rotationMatrix,0,gAngle,1.0f,1.0f,1.0f); // ALL axes rotation by angleCube angle
        
		Matrix.multiplyMM(modelViewMatrix,0,modelMatrix,0,rotationMatrix,0);
        
        GLES30.glUniformMatrix4fv(gModelViewMatrixUniform,1,false,modelViewMatrix,0);

        GLES30.glUniformMatrix4fv(gProjectionMatrixUniform,1,false,perspectiveProjectionMatrix,0);
        
        // bind vao
        GLES30.glBindVertexArray(gVao_cube[0]);
        
        GLES30.glDrawArrays(GLES30.GL_TRIANGLE_FAN,0,4);
        GLES30.glDrawArrays(GLES30.GL_TRIANGLE_FAN,4,4);
        GLES30.glDrawArrays(GLES30.GL_TRIANGLE_FAN,8,4);
        GLES30.glDrawArrays(GLES30.GL_TRIANGLE_FAN,12,4);
        GLES30.glDrawArrays(GLES30.GL_TRIANGLE_FAN,16,4);
        GLES30.glDrawArrays(GLES30.GL_TRIANGLE_FAN,20,4);
        
        // unbind vao
        GLES30.glBindVertexArray(0);
        
        //delete shader program
        GLES30.glUseProgram(0);
		
		//UPDATE
		if (gbAnimate_singleTap == 1)
			Update();
        
		// SWAP BUFFER
        requestRender();
		
	}
	
	public void uninitialize()
	{
   		
		//
		//DESTROY VAO
		if (gVao_cube[0] != 0)
		{
			GLES30.glDeleteVertexArrays(1,gVao_cube,0);
			gVao_cube[0] = 0;
		}


		//DESTROY VBO
		if (gVbo_cube_position[0] != 0)
		{
			GLES30.glDeleteVertexArrays(1, gVbo_cube_position,0);
			gVbo_cube_position[0] = 0;
		}
		if (gVbo_cube_normal[0] != 0)
		{
			GLES30.glDeleteVertexArrays(1, gVbo_cube_normal,0);
			gVbo_cube_normal[0] = 0;
		}
		//

        if(shaderProgramObject != 0)
        {
            if(vertexShaderObject != 0)
            {
                GLES30.glDetachShader(shaderProgramObject, vertexShaderObject);
                GLES30.glDeleteShader(vertexShaderObject);
                vertexShaderObject = 0;
            }
            
            if(fragmentShaderObject != 0)
            {
                GLES30.glDetachShader(shaderProgramObject, fragmentShaderObject);
               	GLES30.glDeleteShader(fragmentShaderObject);
                fragmentShaderObject = 0;
            }
        }

        // DELETE SHADER PROGRAM OBJECT
        if(shaderProgramObject != 0)
        {
            GLES30.glDeleteProgram(shaderProgramObject);
            shaderProgramObject = 0;
        }
		
	} 
}
