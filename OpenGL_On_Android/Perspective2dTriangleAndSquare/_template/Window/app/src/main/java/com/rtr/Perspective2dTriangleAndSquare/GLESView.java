package com.rtr.Perspective2dTriangleAndSquare;

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
	
	private int [] vbo = new int[1];

	private int [] vao_triangle = new int[1];
	
	private int [] vao_square = new int[1];

	private int mvpUniform;

	private float  perspectiveProjectionMatrix[]=new float[16]; // 4x4 matrix
	
	
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
		return true;
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
         "uniform mat4 u_mvp_matrix;"+
         "void main(void)"+
         "{"+
         "gl_Position = u_mvp_matrix * vPosition;"+
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
		"out vec4 FragColor;"+
		"void main(void)"+
		"{"+
		"FragColor =  vec4(1.0 , 1.0 , 1.0 , 1.0);"+
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
	
	// PRE - BINDING OF vposition attribute
    GLES30.glBindAttribLocation(shaderProgramObject,GLESMacros.VDG_ATTRIBUTE_POSITION,"vPosition");
		
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
	
	 mvpUniform =  GLES30.glGetUniformLocation(shaderProgramObject,"u_mvp_matrix");
	 
	 //VERTICES COLOR SHADER ATTRIBS VBO , VAO INITIALIZE
	  final float triangleVertices[]= new float[]
        {
		0.0f, 1.0f, 0.0f, // apex
		-1.0f,-1.0f, 0.0f, // left -bottom
		1.0f, -1.0f ,0.0f //right -bottm
		};
		
	final float squareVertices[]= new float[]
        {
			1.0f, 1.0f, 0.0f, // right top
			-1.0f, 1.0f, 0.0f, // left top
			-1.0f, -1.0f ,0.0f, //left -bottm
			1.0f, -1.0f ,0.0f//right -bottm
		};
		
		
		/*TRIANGLE*/
		GLES30.glGenVertexArrays(1,vao_triangle,0);
		GLES30.glBindVertexArray(vao_triangle[0]);
		
		GLES30.glGenBuffers(1,vbo,0);
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,vbo[0]);
		
		ByteBuffer byteBuffer=ByteBuffer.allocateDirect(triangleVertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        
		FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(triangleVertices);
        verticesBuffer.position(0);
		
		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,
                            triangleVertices.length * 4,
                            verticesBuffer,
                            GLES30.GL_STATIC_DRAW);
        
        GLES30.glVertexAttribPointer(GLESMacros.VDG_ATTRIBUTE_POSITION,
                                     3,
                                     GLES30.GL_FLOAT,
                                     false,0,0);
        
        GLES30.glEnableVertexAttribArray(GLESMacros.VDG_ATTRIBUTE_POSITION);
        
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);
        GLES30.glBindVertexArray(0);
		/**/
		
		
		/*SQUARE*/
		GLES30.glGenVertexArrays(1,vao_square,0);
		GLES30.glBindVertexArray(vao_square[0]);
		
		GLES30.glGenBuffers(1,vbo,0);
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,vbo[0]);
		
		byteBuffer=ByteBuffer.allocateDirect(squareVertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        
		verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(squareVertices);
        verticesBuffer.position(0);
		
		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,
                            squareVertices.length * 4,
                            verticesBuffer,
                            GLES30.GL_STATIC_DRAW);
        
        GLES30.glVertexAttribPointer(GLESMacros.VDG_ATTRIBUTE_POSITION,3,GLES30.GL_FLOAT,false,0,0);
        
        GLES30.glEnableVertexAttribArray(GLESMacros.VDG_ATTRIBUTE_POSITION);
        
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);
        GLES30.glBindVertexArray(0);
		/**/
		
        // ENABLE DEPTH TEST
        GLES30.glEnable(GLES30.GL_DEPTH_TEST);
       
        GLES30.glDepthFunc(GLES30.GL_LEQUAL);
        
		// CULLING BACK FACES ALWAYS FOR PERFORMANCE.
        GLES30.glEnable(GLES30.GL_CULL_FACE);
	
		// Set the background frame color
		GLES30.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
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
		
		/*DRAW TRIANGLE*/
        float modelViewMatrix[]=new float[16];
        float modelViewProjectionMatrix[]=new float[16];
        
        Matrix.setIdentityM(modelViewMatrix,0);
        Matrix.setIdentityM(modelViewProjectionMatrix,0);

		Matrix.translateM(modelViewMatrix,0, -1.5f, 0.0f, -6.0f);
		
        Matrix.multiplyMM(modelViewProjectionMatrix,0,perspectiveProjectionMatrix,0,modelViewMatrix,0);
        
        GLES30.glUniformMatrix4fv(mvpUniform,1,false,modelViewProjectionMatrix,0);
        
        // BINFD VAO
        GLES30.glBindVertexArray(vao_triangle[0]);
        
        GLES30.glDrawArrays(GLES30.GL_TRIANGLES,0,3); // 3 (each with its x,y,z ) vertices in triangleVertices array
        
        // UNBIND vao
        GLES30.glBindVertexArray(0);
        
		/*END OF TRIANGLE*/
		
		
		
		/*DRAW SQUARE*/
        modelViewMatrix=new float[16];
        modelViewProjectionMatrix=new float[16];
        
        Matrix.setIdentityM(modelViewMatrix,0);
        Matrix.setIdentityM(modelViewProjectionMatrix,0);

		Matrix.translateM(modelViewMatrix,0, 1.5f, 0.0f, -6.0f);
		
        Matrix.multiplyMM(modelViewProjectionMatrix,0,perspectiveProjectionMatrix,0,modelViewMatrix,0);
        
        GLES30.glUniformMatrix4fv(mvpUniform,1,false,modelViewProjectionMatrix,0);
        
        // BINFD VAO
        GLES30.glBindVertexArray(vao_square[0]);
        
        GLES30.glDrawArrays(GLES30.GL_TRIANGLE_FAN,0,4); // 3 (each with its x,y,z ) vertices in triangleVertices array
        
        // UNBIND vao
        GLES30.glBindVertexArray(0);
        
		/*END OF SQUARE*/
		
		
        // STOP USING SHADER PROGRAM OBJECT
       	GLES30.glUseProgram(0);

        // SWAP BUFFER
        requestRender();
		
	}
	
	public void uninitialize()
	{
        // DELETE vao
        if(vao_square[0] != 0)
        {
            GLES30.glDeleteVertexArrays(1, vao_square, 0);
            vao_square[0]=0;
        }
		
		if(vao_triangle[0] != 0)
        {
            GLES30.glDeleteVertexArrays(1, vao_triangle, 0);
            vao_triangle[0]=0;
        }
        
        // DELETE vao
        if(vbo[0] != 0)
        {
            GLES30.glDeleteBuffers(1, vbo, 0);
            vbo[0]=0;
        }

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
