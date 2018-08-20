package com.rtr.TwoLightPerVertex;

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

/*Sphere*/
import java.nio.ShortBuffer;


public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener , OnDoubleTapListener
{ 
	
	private final Context _context; 
	
	private GestureDetector _gestureDetectort;
	
	private int vertexShaderObject;
	
	private int fragmentShaderObject;
	
	private int shaderProgramObject;
	
	
	
	
	private int model_matrix_uniform;
	
	private int view_matrix_uniform;
	
	private int projection_matrix_uniform;
	
	private float  perspectiveProjectionMatrix[]=new float[16]; // 4x4 matrix
	
	
	private int  La1_uniform;
	
	private int  Ld1_uniform;
	
	private int Ls1_uniform;
	
	private int light_position1_uniform;
	
	
	private int  La2_uniform;
	
	private int  Ld2_uniform;
	
	private int  Ls2_uniform;
	
	private int  light_position2_uniform;


	
	private int  L_KeyPressed_uniform;

	private int  Ka_uniform;
	
	private int  Kd_uniform;
	
	private int  Ks_uniform;
	
	private int  material_shininess_uniform;



	
	private float []left_lightAmbient = new float[] { 0.0f,0.0f,0.0f,1.0f };
	private float [] left_lightDiffuse = new float[] {1.0f,0.0f,0.0f,0.0f };
	private float [] left_lightSpecular = new float[]{ 0.0f,0.0f,1.0f,0.0f };
	private float [] left_lightPosition = new float[]{ -2.0f,1.0f,1.0f,0.0f };

	

	private float [] right_lightAmbient = new float[]{ 0.0f,0.0f,0.0f,0.0f };
	private float [] right_lightDiffuse = new float[]{ 0.0f,0.0f,1.0f,0.0f };
	private float [] right_lightSpecular = new float[]{1.0f,0.0f,0.0f,0.0f  };
	private float [] right_lightPosition = new float[]{ 2.0f,1.0f,1.0f,0.0f };

	
	private boolean gbLight;


	private float [] material_ambient = new float[]{ 0.0f,0.0f,0.0f,1.0f };
	private float [] material_diffuse = new float[]{ 1.0f,1.0f,1.0f,1.0f };
	private float [] material_specular = new float[]{ 1.0f,1.0f,1.0f,1.0f };
	private float 	 material_shininess = 50.0f;

	private int [] gVao_pyramid = new int[1];
	private int [] gVbo_pyramid_position = new int[1];
	private int [] gVbo_pyramid_normal = new int[1];
	
	private float anglePyramid = 0.0f;

	
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
		
		if (anglePyramid >= 360.0f) 
			anglePyramid = 0.0f;
		anglePyramid = anglePyramid + 0.3f;
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
		 if(gbLight)
			 gbLight = false;
		 else
			gbLight = true;
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
		"in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform mat4 u_model_matrix;" +
		"uniform mat4 u_view_matrix;" +
		"uniform mat4 u_projection_matrix;" +
		"uniform int u_lighting_enabled;" +
		"uniform vec3 u_La1;" +
		"uniform vec3 u_Ld1;" +
		"uniform vec3 u_Ls1;" +
		"uniform vec4 u_light_position1;" +
		"uniform vec3 u_La2;" +
		"uniform vec3 u_Ld2;" +
		"uniform vec3 u_Ls2;" +
		"uniform vec4 u_light_position2;" +
		"uniform vec3 u_Ka;" +
		"uniform vec3 u_Kd;" +
		"uniform vec3 u_Ks;" +
		"uniform float u_material_shininess;" +
		"out vec3 phong_ads_color;" +
		"void main(void)" +
		"{" +
			"if(u_lighting_enabled==1)" +
			"{" +
				"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" +
				"vec3 transformed_normals=normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" +
				
				"vec3 light_direction1 = normalize(vec3(u_light_position1) - eye_coordinates.xyz);" +
				"float tn_dot_ld1 = max(dot(transformed_normals, light_direction1),0.0);" +
				"vec3 ambient1 = u_La1 * u_Ka;" +
				"vec3 diffuse1 = u_Ld1* u_Kd * tn_dot_ld1;" +
				"vec3 reflection_vector1 = reflect(-light_direction1, transformed_normals);" +
				"vec3 viewer_vector = normalize(-eye_coordinates.xyz);" +
				"vec3 specular1 = u_Ls1 * u_Ks * pow(max(dot(reflection_vector1, viewer_vector), 0.0), u_material_shininess);" +

				"vec3 light_direction2 = normalize(vec3(u_light_position2) - eye_coordinates.xyz);" +
				"float tn_dot_ld2 = max(dot(transformed_normals, light_direction2),0.0);" +
				"vec3 ambient2 = u_La2 * u_Ka;" +
				"vec3 diffuse2 = u_Ld2* u_Kd * tn_dot_ld2;" +
				"vec3 reflection_vector2 = reflect(-light_direction2, transformed_normals);" +
				"vec3 specular2 = u_Ls2 * u_Ks * pow(max(dot(reflection_vector2, viewer_vector), 0.0), u_material_shininess);" +


				"phong_ads_color= (ambient1+ambient2) +( diffuse1+diffuse2 )+ (specular1+specular2);" +
			"}" +
			"else" +
			"{" +
				"phong_ads_color = vec3(1.0, 1.0, 1.0);" +
			"}" +
			"gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
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
		"in vec3 phong_ads_color;" +
		"out vec4 FragColor;" +
		"void main(void)" +
		"{" +
			"FragColor = vec4(phong_ads_color, 1.0);" +
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
	model_matrix_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_model_matrix");
	view_matrix_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_view_matrix");
	projection_matrix_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_projection_matrix");

	// 'L' HANDLE KEY TO TOGGLE LIGHT.
	L_KeyPressed_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_lighting_enabled");

	//AMBIENT COLOR TOKEN
	La1_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_La1");
	
	// DIFUSE COLOR TOKEN
	Ld1_uniform= GLES30.glGetUniformLocation(shaderProgramObject, "u_Ld1");
	
	// SPECULAR COLOR TOKEN
	Ls1_uniform= GLES30.glGetUniformLocation(shaderProgramObject, "u_Ls1");
	
	// LIGHT POSITION TOKEN
	light_position1_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_light_position1");



	//AMBIENT COLOR TOKEN
	La2_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_La2");

	// DIFUSE COLOR TOKEN
	Ld2_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ld2");

	// SPECULAR COLOR TOKEN
	Ls2_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ls2");

	// LIGHT POSITION TOKEN
	light_position2_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_light_position2");




	// AMBIENT REFLECTIVE TOKEN
	Ka_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ka");
	
	// DIFUUSE REFLECTIVE TOKEN
	Kd_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Kd");
	
	// SPECULAR REFLECTIVE TOKEN
	Ks_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ks");
	
	//SHINYNESS COLOR TOKEN
	material_shininess_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_material_shininess");;

	/*VERTICES ,COLOR ,NORMAL  INITIALIZATION*/

	float []pyramidVertices = new float[]
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

	float  []PyramidNormals = new float[]
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

	
	
	
	//VERTICES , COLORS ,SHADERS ATTRIBS ,VBO ,VAO INITIALIZATION

		/*TRIANGLE*/
		GLES30.glGenVertexArrays(1,gVao_pyramid,0);
		GLES30.glBindVertexArray(gVao_pyramid[0]);
		
		/*POSITIONS*/
		GLES30.glGenBuffers(1,gVbo_pyramid_position ,0);
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,gVbo_pyramid_position[0]);
		
		ByteBuffer byteBuffer=ByteBuffer.allocateDirect(pyramidVertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        
		FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(pyramidVertices);
        verticesBuffer.position(0);
		
		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,
                            pyramidVertices.length * 4,
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
		GLES30.glGenBuffers(1,gVbo_pyramid_normal ,0);
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,gVbo_pyramid_normal[0]);
		
		byteBuffer =ByteBuffer.allocateDirect(PyramidNormals.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        
		verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(PyramidNormals);
        verticesBuffer.position(0);
		
		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,
                            PyramidNormals.length * 4,
                            verticesBuffer,
                            GLES30.GL_STATIC_DRAW);
							
							
        
        GLES30.glVertexAttribPointer(GLESMacros.VDG_ATTRIBUTE_NORMAL,3,GLES30.GL_FLOAT,false,0,0);
        
        GLES30.glEnableVertexAttribArray(GLESMacros.VDG_ATTRIBUTE_NORMAL);
        
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);
		/**/
		
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
		if (gbLight == true)
		{
			// set 'u_lighting_enabled' uniform
			GLES30.glUniform1i(L_KeyPressed_uniform, 1);
			
			ByteBuffer byteBuffer=ByteBuffer.allocateDirect(left_lightAmbient.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			FloatBuffer buffer=byteBuffer.asFloatBuffer();
			buffer.put(left_lightAmbient);
			buffer.position(0);
					
			
			GLES30.glUniform3fv(La1_uniform, 1, buffer);
			
			
			// setting left light's properties
		
			
			
			byteBuffer=ByteBuffer.allocateDirect(left_lightDiffuse.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(left_lightDiffuse);
			buffer.position(0);
			
			GLES30.glUniform3fv(Ld1_uniform, 1, buffer);
			
			
			
			byteBuffer=ByteBuffer.allocateDirect(left_lightSpecular.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(left_lightSpecular);
			buffer.position(0);
			
			GLES30.glUniform3fv(Ls1_uniform, 1, buffer);
			
			
			
			
			byteBuffer=ByteBuffer.allocateDirect(left_lightPosition.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(left_lightPosition);
			buffer.position(0);
			
			GLES30.glUniform4fv(light_position1_uniform, 1, buffer);
			
			
			
			
			
			// setting right light's properties
			
			byteBuffer=ByteBuffer.allocateDirect(right_lightAmbient.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(right_lightAmbient);
			buffer.position(0);
			
			GLES30.glUniform3fv(La2_uniform, 1, buffer);
			
			
			byteBuffer=ByteBuffer.allocateDirect(right_lightDiffuse.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(right_lightDiffuse);
			buffer.position(0);
			
			GLES30.glUniform3fv(Ld2_uniform, 1, buffer);
			
			
			
			byteBuffer=ByteBuffer.allocateDirect(right_lightSpecular.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(right_lightSpecular);
			buffer.position(0);
			
			GLES30.glUniform3fv(Ls2_uniform, 1, buffer);
			
			
			
			
			byteBuffer=ByteBuffer.allocateDirect(right_lightPosition.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(right_lightPosition);
			buffer.position(0);
			
			GLES30.glUniform4fv(light_position2_uniform, 1, buffer);
			
			
			
			// setting material's properties
			
			byteBuffer=ByteBuffer.allocateDirect(material_ambient.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(material_ambient);
			buffer.position(0);
			
			GLES30.glUniform3fv(Ka_uniform, 1, buffer);
			
			
			
			byteBuffer=ByteBuffer.allocateDirect(material_diffuse.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(material_diffuse);
			buffer.position(0);
			
			GLES30.glUniform3fv(Kd_uniform, 1, buffer);
			
			
			
			byteBuffer=ByteBuffer.allocateDirect(material_specular.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(material_specular);
			buffer.position(0);
			
			GLES30.glUniform3fv(Ks_uniform, 1, buffer);
			
			
			GLES30.glUniform1f(material_shininess_uniform, material_shininess);
		
		}
		else
		{
			GLES30.glUniform1i(L_KeyPressed_uniform, 0);
		}
		/**/
		
	
		
        float modelMatrix[]=new float[16];
        float viewMatrix[]=new float[16];
		float rotateMatrix[]=new float[16];
        
        // pyramid CODE
        // set modelMatrix, modelViewMatrix, rotation matrices to identity matrix
        Matrix.setIdentityM(modelMatrix,0);
        Matrix.setIdentityM(viewMatrix,0);
		Matrix.setIdentityM(rotateMatrix,0);

        Matrix.translateM(modelMatrix,0,0.0f,0.0f,-5.0f);
        
        Matrix.rotateM(modelMatrix,0,anglePyramid,0.0f,1.0f,0.0f); // y axes rotation by angleCube angle
        
		//Matrix.multiplyMM(modelMatrix,0,modelMatrix,0,rotateMatrix,0); //error because of this line
        
     

		GLES30.glUniformMatrix4fv(model_matrix_uniform,1,false,modelMatrix,0);
		
		GLES30.glUniformMatrix4fv(view_matrix_uniform,1,false,viewMatrix,0);
        
		GLES30.glUniformMatrix4fv(projection_matrix_uniform,1,false,perspectiveProjectionMatrix,0);
        
		
        // bind vao
        GLES30.glBindVertexArray(gVao_pyramid[0]);
		GLES30.glDrawArrays(GLES30.GL_TRIANGLES,0,12); // 3 (each with its x,y,z ) vertices in triangleVertices array
      
        // unbind vao
        GLES30.glBindVertexArray(0);
        
        //delete shader program
        GLES30.glUseProgram(0);
		
		//UPDATE
	//	if (gbAnimate_singleTap == 1)
		Update();
        
		// SWAP BUFFER
        requestRender();
		
	}
	
	public void uninitialize()
	{
   		
		//
		//DESTROY VAO
		if (gVao_pyramid[0] != 0)
		{
			GLES30.glDeleteVertexArrays(1,gVao_pyramid,0);
			gVao_pyramid[0] = 0;
		}


		//DESTROY VBO
		if (gVbo_pyramid_position[0] != 0)
		{
			GLES30.glDeleteVertexArrays(1, gVbo_pyramid_position,0);
			gVbo_pyramid_position[0] = 0;
		}
		if (gVbo_pyramid_normal[0] != 0)
		{
			GLES30.glDeleteVertexArrays(1, gVbo_pyramid_normal,0);
			gVbo_pyramid_normal[0] = 0;
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
