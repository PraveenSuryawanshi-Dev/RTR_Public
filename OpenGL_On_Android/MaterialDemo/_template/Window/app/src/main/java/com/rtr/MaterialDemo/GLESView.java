package com.rtr.MaterialDemo;

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

  class  MATERIAL
{
	public float ambient_material[] = new float[4];
	public float diffuse_material[]= new float[4];
	public float specular_material[]= new float[4];
	public float shininess;

}

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener , OnDoubleTapListener
{ 
	
	private final Context _context; 
	
	private GestureDetector _gestureDetectort;
	
	private int gVertexShaderObject;
	
	private int gFragmentShaderObject;
	
	private int gShaderProgramObject;
	
	private float sphere_angle = 0.0f;
	
	
	        
	private float sphere_vertices[]=new float[1146];
	
    private   float sphere_normals[]=new float[1146];
	
    private   float sphere_textures[]=new float[764];
	
    private   short sphere_elements[]=new short[2280];
	
	private   int gNumElements;
	
	private   int gNumVertices;
	
		
	
	private int[] gVao_sphere = new int[1];
    
	private int[] gVbo_sphere_position = new int[1];
    
	private int[] gVbo_sphere_normal = new int[1];
    
	private int[] gVbo_sphere_element = new int[1];

	
	
	private int model_matrix_uniform;
	
	private int view_matrix_uniform;
	
	private int projection_matrix_uniform;
	
	private float  gPerspectiveProjectionMatrix[]=new float[16]; // 4x4 matrix
	
	
	private int La_uniform;
	
	private int Ld_uniform;
	
	private int Ls_uniform;
	
	private int light_position_uniform;
	
	private int light_rotation_uniform;
	
	private int  L_KeyPressed_uniform;
	
	private int Ka_uniform;

	private int Kd_uniform;

	private int Ks_uniform;

	private int material_shininess_uniform;


	private MATERIAL  material[][] = new MATERIAL[6][4];
	
	private boolean gbLight;
	
	private int tapCount = 0;
	
	private int wi;
	private int hi;
	
	private float[]lightAmbient = new float[]{ 0.0f,0.0f,0.0f,1.0f };
	
	private float []lightDiffuse = new float[] {1.0f,1.0f,1.0f,1.0f };

	private float  []lightSpecular = new float[] {1.0f,1.0f,1.0f,1.0f };

	private float []lightPosition = new float[]{ 0.0f,0.0f,0.0f,0.0f };

	
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
		if (sphere_angle >= 720.0f)
			sphere_angle = 360.0f;
		else
			sphere_angle = sphere_angle + 0.5f;
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
	
		sphere_angle = 0.0f;
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
		if(tapCount == 2)
			tapCount = 0;
		else
			tapCount ++;
		
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
		System.out.println("PRAVEEN IN : initialize");
		//VERTEX SHADER 
		
		gVertexShaderObject = GLES30.glCreateShader(GLES30.GL_VERTEX_SHADER);
		
		final String vertexshadersourcecode = String.format(
		 "#version 300 es"+
         "\n"+
			"in vec4 vPosition;" +
			"in vec3 vNormal;" +
			"uniform mat4 u_model_matrix;" +
			"uniform mat4 u_view_matrix;" +
			"uniform mat4 u_projection_matrix;" +
			"uniform int u_lighting_enabled;" +
			"uniform vec3 u_La;" +
			"uniform vec3 u_Ld;" +
			"uniform vec3 u_Ls;" +
			"uniform vec4 u_light_position;" +
			"uniform mat4 u_light_rotation;"+

			"uniform vec3 u_Ka;" +
			"uniform vec3 u_Kd;" +
			"uniform vec3 u_Ks;" +
			"uniform float u_material_shininess;" +
			"out vec3 phong_ads_color;" +

			"void main(void)" +
			"{" +
				"if(u_lighting_enabled == 1)" +
				"{" +
					"vec4 temp= u_light_position*u_light_rotation;"+

					"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" +
					"vec3 transformed_normals=normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" +
					"vec3 light_direction = normalize(vec3(temp) - eye_coordinates.xyz);" +
					"float tn_dot_ld = max(dot(transformed_normals, light_direction),0.0);" +
					"vec3 ambient = u_La * u_Ka;" +
					"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" +
					"vec3 reflection_vector = reflect(-light_direction, transformed_normals);" +
					"vec3 viewer_vector = normalize(-eye_coordinates.xyz);" +
					"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess);" +
					"phong_ads_color=ambient + diffuse + specular;" +
				"}" +
				"else" +
				"{" +
					"phong_ads_color = vec3(1.0, 1.0, 1.0);" +
				"}" +

			"gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
			"}"
	);
	
	// provide source code to vertex shader object.
	GLES30.glShaderSource(gVertexShaderObject,vertexshadersourcecode);
	
	//Compile Shader
	GLES30.glCompileShader(gVertexShaderObject);
	
	//Check for compilation error
	
	int[] ishadercompilationstatus = new int[1];
	int[] iinfologlength = new int[1];
	
	String szInfoLog = null;
	
	GLES30.glGetShaderiv(gVertexShaderObject,GLES30.GL_COMPILE_STATUS,ishadercompilationstatus,0);
	
	if(ishadercompilationstatus[0] == GLES30.GL_FALSE)
	{
		GLES30.glGetShaderiv(gVertexShaderObject, GLES30.GL_INFO_LOG_LENGTH, iinfologlength, 0);
		if(iinfologlength[0] > 0 )
		{
			szInfoLog = GLES30.glGetShaderInfoLog(gVertexShaderObject);
			System.out.println("PRAVEEN :"+szInfoLog);
			uninitialize();
			System.exit(0);
		}
	}	
	//END OF VERTEX SHADER
	
	
	//CREATE FRAGMENT SHADER 
	gFragmentShaderObject = GLES30.glCreateShader(GLES30.GL_FRAGMENT_SHADER);
		
		final String fragmentshadersourcecode = String.format(
		"#version 300 es"+
			"\n" +
			"in vec3 phong_ads_color;" +
			"out vec4 FragColor;" +
			"void main(void)" +
			"{" +
				"FragColor = vec4(phong_ads_color, 1.0);" +
			"}"
	);
	
	// provide source code to vertex shader object.
	GLES30.glShaderSource(gFragmentShaderObject,fragmentshadersourcecode);
	
	//Compile Shader
	GLES30.glCompileShader(gFragmentShaderObject);
	
	//Check for compilation error
	
	ishadercompilationstatus[0]= 0;
	iinfologlength[0]= 0;
	szInfoLog = null;
	
	GLES30.glGetShaderiv(gFragmentShaderObject,GLES30.GL_COMPILE_STATUS,ishadercompilationstatus,0);
	
	if(ishadercompilationstatus[0] == GLES30.GL_FALSE)
	{
		GLES30.glGetShaderiv(gFragmentShaderObject,GLES30.GL_INFO_LOG_LENGTH,iinfologlength,0);
		if(iinfologlength[0] > 0 )
		{
			szInfoLog = GLES30.glGetShaderInfoLog(gFragmentShaderObject);
			System.out.println("PRAVEEN :"+szInfoLog);
			uninitialize();
			System.exit(0);
		}
	}	
	
	//END of Fragment Shader
  
  
	//CREATE SHADER PROGRAM OBJECT*/
	 gShaderProgramObject = GLES30.glCreateProgram();
	 
	 //ATTACH VERTEX SHADER OBJECT
	 GLES30.glAttachShader(gShaderProgramObject, gVertexShaderObject);
	 
	//ATTACH FRAGMENT SHADER OBJECT
	GLES30.glAttachShader(gShaderProgramObject, gFragmentShaderObject);
	
	
	//PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITH VERTEX SHADER POSITION ATTRIBUTE
	GLES30.glBindAttribLocation(gShaderProgramObject, GLESMacros.VDG_ATTRIBUTE_POSITION, "vPosition");
	
	//PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITTH Normal ATTRIBUTE
	GLES30.glBindAttribLocation(gShaderProgramObject, GLESMacros.VDG_ATTRIBUTE_NORMAL, "vNormal");
	//

		
	//LINK TWO SHADER TOGHER TO ONE SHADER PROGRAM OBJECT
	GLES30.glLinkProgram(gShaderProgramObject);
	//Check for compilation error
	
	ishadercompilationstatus[0]= 0;
	iinfologlength[0]= 0;
	szInfoLog = null;
	
	GLES30.glGetShaderiv(gShaderProgramObject,GLES30.GL_COMPILE_STATUS,ishadercompilationstatus,0);
	
	if(ishadercompilationstatus[0] == GLES30.GL_FALSE)
	{
		GLES30.glGetShaderiv(gShaderProgramObject,GLES30.GL_INFO_LOG_LENGTH,iinfologlength,0);
		if(iinfologlength[0] > 0 )
		{
			szInfoLog = GLES30.glGetShaderInfoLog(gShaderProgramObject);
			System.out.println("PRAVEEN :"+szInfoLog);
			uninitialize();
			System.exit(0);
		}
	}	
		 
		//GET MVP UNIFORM LOCATION
		model_matrix_uniform = GLES30.glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	
		view_matrix_uniform = GLES30.glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	
		projection_matrix_uniform = GLES30.glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
		
		
		L_KeyPressed_uniform = GLES30.glGetUniformLocation(gShaderProgramObject, "u_lighting_enabled");
	
		La_uniform = GLES30.glGetUniformLocation(gShaderProgramObject, "u_La");
		
		Ld_uniform = GLES30.glGetUniformLocation(gShaderProgramObject, "u_Ld");
		
		Ls_uniform = GLES30.glGetUniformLocation(gShaderProgramObject, "u_Ls");
		
	
		light_position_uniform = GLES30.glGetUniformLocation(gShaderProgramObject, "u_light_position");
		
		light_rotation_uniform = GLES30.glGetUniformLocation(gShaderProgramObject, "u_light_rotation");
		
			
		Ka_uniform = GLES30.glGetUniformLocation(gShaderProgramObject, "u_Ka");
		
		
		Kd_uniform = GLES30.glGetUniformLocation(gShaderProgramObject, "u_Kd");
		
		
		Ks_uniform = GLES30.glGetUniformLocation(gShaderProgramObject, "u_Ks");
		
		
		material_shininess_uniform = GLES30.glGetUniformLocation(gShaderProgramObject, "u_material_shininess");
	//
	 
		//SPHERE
		Sphere sphere=new Sphere();

        
		sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        gNumVertices = sphere.getNumberOfSphereVertices();
        gNumElements = sphere.getNumberOfSphereElements();
		//
	
		/*SPHERE*/
		GLES30.glGenVertexArrays(1,gVao_sphere,0);
		GLES30.glBindVertexArray(gVao_sphere[0]);
		
		
		
		/*POSITIONS*/
		GLES30.glGenBuffers(1,gVbo_sphere_position ,0);
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,gVbo_sphere_position[0]);
		
		ByteBuffer byteBuffer=ByteBuffer.allocateDirect(sphere_vertices.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		
		FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
		verticesBuffer.put(sphere_vertices);
		verticesBuffer.position(0);
		
		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,
							sphere_vertices.length * 4,
							verticesBuffer,
							GLES30.GL_STATIC_DRAW);
							
							
		
		GLES30.glVertexAttribPointer(GLESMacros.VDG_ATTRIBUTE_POSITION,
										3,
										GLES30.GL_FLOAT,
										false,0,0);
		
		GLES30.glEnableVertexAttribArray(GLESMacros.VDG_ATTRIBUTE_POSITION);
		
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);
		/**/
		
		
		
		/*NORMALS*/
		GLES30.glGenBuffers(1,gVbo_sphere_normal ,0);
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,gVbo_sphere_normal[0]);
		
		byteBuffer =ByteBuffer.allocateDirect(sphere_normals.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		
		verticesBuffer=byteBuffer.asFloatBuffer();
		verticesBuffer.put(sphere_normals);
		verticesBuffer.position(0);
		
		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER,
							sphere_normals.length * 4,
							verticesBuffer,
							GLES30.GL_STATIC_DRAW);
		
		GLES30.glVertexAttribPointer(GLESMacros.VDG_ATTRIBUTE_NORMAL,3,GLES30.GL_FLOAT,false,0,0);
		
		GLES30.glEnableVertexAttribArray(GLESMacros.VDG_ATTRIBUTE_NORMAL);
		
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,0);
		/**/
		
		
		/*ELEMENTS*/
		GLES30.glGenBuffers(1,gVbo_sphere_element ,0);
		GLES30.glBindBuffer(GLES30.GL_ELEMENT_ARRAY_BUFFER,gVbo_sphere_element[0]);

		
		////////////////
		byteBuffer=ByteBuffer.allocateDirect(sphere_elements.length * 2);
        byteBuffer.order(ByteOrder.nativeOrder());
        ShortBuffer elementsBuffer=byteBuffer.asShortBuffer();
        elementsBuffer.put(sphere_elements);
        elementsBuffer.position(0);
		////////////////
		
		GLES30.glBufferData(GLES30.GL_ELEMENT_ARRAY_BUFFER,
							sphere_elements.length * 2,
							elementsBuffer,
							GLES30.GL_STATIC_DRAW);
		
		GLES30.glBindBuffer(GLES30.GL_ELEMENT_ARRAY_BUFFER,0);
		/**/
				
		GLES30.glBindVertexArray(0);
		/**/
		
		
		
		// ENABLE DEPTH TEST
		GLES30.glEnable(GLES30.GL_DEPTH_TEST);
		
		GLES30.glDepthFunc(GLES30.GL_LEQUAL);
		
		// CULLING BACK FACES ALWAYS FOR PERFORMANCE.
		GLES30.glEnable(GLES30.GL_CULL_FACE);
		
		// Set the background frame color
		GLES30.glClearColor(0.25f, 0.25f, 0.25f, 0.0f);
			
		
		
		// initialization
		
		//gbLight_doubleTab = 0;
		gbLight = false;
		InitializeMaterials();
		Matrix.setIdentityM(gPerspectiveProjectionMatrix,0);
		
		System.out.println("PRAVEEN OUT : initialize");
    }
	
	private void resize(int width ,int height)
	{
		System.out.println("PRAVEEN IN : resize");
		if(height == 0)
			height = 1;
		
		wi = width;
		hi = height;
		
		
		System.out.println("PRAVEEN IN : resize"+wi+"*"+hi);
		//Set Viewport on window.
		//GLES30.glViewport(0,0,width , height);
		
		Matrix.perspectiveM(gPerspectiveProjectionMatrix,0,45.05f,(float)width/(float)height,0.1f, 100.0f);
		
		System.out.println("PRAVEEN IN : resize");
	}
	
	public void draw()
	{
		 // CLEAR BUFFERS
        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT | GLES30.GL_DEPTH_BUFFER_BIT);
        
        // START USING SHADER PROGRAM OBJECT
        GLES30.glUseProgram(gShaderProgramObject);

        // OpenGL-ES drawing
		
		/**/
		if (gbLight)
		{
			// set 'u_lighting_enabled' uniform
			GLES30.glUniform1i(L_KeyPressed_uniform, 1);		
		
		}
		else
		{
			GLES30.glUniform1i(L_KeyPressed_uniform, 0);
		}
		/**/
		
		
        float modelMatrix[]=new float[16];
        float viewMatrix[]=new float[16];
        float rotateMatrix[] = new float[16];
       
	   // SPhere CODE
        // set modelMatrix, modelViewMatrix, rotation matrices to identity matrix
        Matrix.setIdentityM(modelMatrix,0);
        Matrix.setIdentityM(viewMatrix,0);
		Matrix.setIdentityM(rotateMatrix,0);
      
		GLES30.glUniformMatrix4fv(view_matrix_uniform, 1, false, viewMatrix,0);
		GLES30.glUniformMatrix4fv(projection_matrix_uniform, 1, false, gPerspectiveProjectionMatrix,0);
	

	// setting light's properties
		//
		ByteBuffer byteBuffer=ByteBuffer.allocateDirect(lightAmbient.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		
		FloatBuffer buffer=byteBuffer.asFloatBuffer();
		buffer.put(lightAmbient);
		buffer.position(0);
				
		// setting light's properties
		GLES30.glUniform3fv(La_uniform, 1, buffer);
		
		byteBuffer=ByteBuffer.allocateDirect(lightDiffuse.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		
		buffer=byteBuffer.asFloatBuffer();
		buffer.put(lightDiffuse);
		buffer.position(0);
		
		GLES30.glUniform3fv(Ld_uniform, 1, buffer);
		
		
		
		byteBuffer=ByteBuffer.allocateDirect(lightSpecular.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		
		buffer=byteBuffer.asFloatBuffer();
		buffer.put(lightSpecular);
		buffer.position(0);
		
		GLES30.glUniform3fv(Ls_uniform, 1, buffer);
		
		byteBuffer=ByteBuffer.allocateDirect(lightPosition.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		
		buffer=byteBuffer.asFloatBuffer();
		buffer.put(lightPosition);
		buffer.position(0);
		
		GLES30.glUniform4fv(light_position_uniform, 1, buffer);

	////////////
	
	
		int col = 5;
		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				Matrix.setIdentityM(modelMatrix,0);
				Matrix.setIdentityM(rotateMatrix,0);
				
				float newWidth = wi/4.0f;
				float newHeight = hi/6.0f;
	
				GLES30.glViewport((int)(newWidth*j),(int) newHeight*col,(int) newWidth,(int) newHeight);
				Matrix.perspectiveM(gPerspectiveProjectionMatrix,0,45.05f,(float)newWidth/(float)newHeight,0.1f, 100.0f);
	
				// setting material's properties
				byteBuffer=ByteBuffer.allocateDirect(material[i][j].ambient_material.length * 4);
				byteBuffer.order(ByteOrder.nativeOrder());
			
				buffer=byteBuffer.asFloatBuffer();
				buffer.put(material[i][j].ambient_material);
				buffer.position(0);
				
				GLES30.glUniform3fv(Ka_uniform, 1, buffer);
				
				byteBuffer=ByteBuffer.allocateDirect(material[i][j].diffuse_material.length * 4);
				byteBuffer.order(ByteOrder.nativeOrder());
			
				buffer=byteBuffer.asFloatBuffer();
				buffer.put(material[i][j].diffuse_material);
				buffer.position(0);
				
				GLES30.glUniform3fv(Kd_uniform, 1, buffer);
				
				byteBuffer=ByteBuffer.allocateDirect(material[i][j].specular_material.length * 4);
				byteBuffer.order(ByteOrder.nativeOrder());
			
				buffer=byteBuffer.asFloatBuffer();
				buffer.put(material[i][j].specular_material);
				buffer.position(0);
				
				GLES30.glUniform3fv(Ks_uniform, 1, buffer);
				
				
				GLES30.glUniform1f(material_shininess_uniform, material[i][j].shininess);
				//
	
				
				Matrix.translateM(modelMatrix,0,0.0f,0.0f,-2.0f);
				
				if (tapCount == 0)
				{
					Matrix.setRotateM(rotateMatrix,0,sphere_angle,1.0f,0.0f,0.0f);
				
					lightPosition[1] = sphere_angle;
	
					byteBuffer=ByteBuffer.allocateDirect(lightPosition.length * 4);
					byteBuffer.order(ByteOrder.nativeOrder());
			
					buffer=byteBuffer.asFloatBuffer();
					buffer.put(lightPosition);
					buffer.position(0);
				
					GLES30.glUniform4fv(light_position_uniform, 1, buffer);
					GLES30.glUniformMatrix4fv(light_rotation_uniform, 1, false, rotateMatrix,0);
				}
				else
				if (tapCount == 1)
				{
				
					Matrix.setRotateM(rotateMatrix,0,sphere_angle,0.0f,1.0f,0.0f);
				
	
					lightPosition[0] = sphere_angle;
					
					byteBuffer=ByteBuffer.allocateDirect(lightPosition.length * 4);
					byteBuffer.order(ByteOrder.nativeOrder());
			
					buffer=byteBuffer.asFloatBuffer();
					buffer.put(lightPosition);
					buffer.position(0);
				
					GLES30.glUniform4fv(light_position_uniform, 1, buffer);
	
					GLES30.glUniformMatrix4fv(light_rotation_uniform, 1, false, rotateMatrix,0);
				}
				else
				if (tapCount == 2)
				{
				
					Matrix.setRotateM(rotateMatrix,0,sphere_angle,0.0f,0.0f,1.0f); 
				
					lightPosition[0] = sphere_angle;
					byteBuffer=ByteBuffer.allocateDirect(lightPosition.length * 4);
					byteBuffer.order(ByteOrder.nativeOrder());
			
					buffer=byteBuffer.asFloatBuffer();
					buffer.put(lightPosition);
					buffer.position(0);
				
					GLES30.glUniform4fv(light_position_uniform, 1, buffer);
					GLES30.glUniformMatrix4fv(light_rotation_uniform, 1, false, rotateMatrix,0);
				}
				/*else
				{
					lightPosition[3] = 0.0f;
					lightPosition[1] = 0.0f;
					lightPosition[2] = 1.0f;
					byteBuffer=ByteBuffer.allocateDirect(lightPosition.length * 4);
					byteBuffer.order(ByteOrder.nativeOrder());
			
					buffer=byteBuffer.asFloatBuffer();
					buffer.put(lightPosition);
					buffer.position(0);
				
					GLES30.glUniform4fv(light_position_uniform, 1, buffer);
				}*/
				
				GLES30.glUniformMatrix4fv(model_matrix_uniform, 1, false, modelMatrix,0);
			
				
				// BIND VAO
				GLES30.glBindVertexArray(gVao_sphere[0]);
	
				//START DRAWING
				GLES30.glBindBuffer(GLES30.GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element[0]);
				GLES30.glDrawElements(GLES30.GL_TRIANGLES, gNumElements, GLES30.GL_UNSIGNED_SHORT, 0);
				// UNBING VAO
				
	
			}
	
			col--;
	
		}

		Update();
		
        GLES30.glUseProgram(0);
		// SWAP BUFFER
        requestRender();
		
	}
	
	public void uninitialize()
	{
   		System.out.println("PRAVEEN IN : uninitialize");
		//
		//DESTROY VAO
		if (gVao_sphere[0] != 0)
		{
			GLES30.glDeleteVertexArrays(1,gVao_sphere,0);
			gVao_sphere[0] = 0;
		}


		//DESTROY VBO
		if (gVbo_sphere_position[0] != 0)
		{
			GLES30.glDeleteVertexArrays(1, gVbo_sphere_position,0);
			gVbo_sphere_position[0] = 0;
		}
		if (gVbo_sphere_normal[0] != 0)
		{
			GLES30.glDeleteVertexArrays(1, gVbo_sphere_normal,0);
			gVbo_sphere_normal[0] = 0;
		}
		if (gVbo_sphere_element[0] != 0)
		{
			GLES30.glDeleteVertexArrays(1, gVbo_sphere_element,0);
			gVbo_sphere_element[0] = 0;
		}
		//

        if(gShaderProgramObject != 0)
        {
            if(gVertexShaderObject != 0)
            {
                GLES30.glDetachShader(gShaderProgramObject, gVertexShaderObject);
                GLES30.glDeleteShader(gVertexShaderObject);
                gVertexShaderObject = 0;
            }
            
            if(gFragmentShaderObject != 0)
            {
                GLES30.glDetachShader(gShaderProgramObject, gFragmentShaderObject);
               	GLES30.glDeleteShader(gFragmentShaderObject);
                gFragmentShaderObject = 0;
            }
        }

        // DELETE SHADER PROGRAM OBJECT
        if(gShaderProgramObject != 0)
        {
            GLES30.glDeleteProgram(gShaderProgramObject);
            gShaderProgramObject = 0;
        }
		
		System.out.println("PRAVEEN OUT : uninitialize");
	} 
	
	private void InitializeMaterials()
	{
		
		for(int i = 0 ; i < 6 ; i++)
		{
			for(int j = 0 ; j < 4 ; j++)
			{
				material[i][j] = new MATERIAL();
			}
		}
		System.out.println("PRAVEEN IN : InitializeMaterials");
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
	
	for(int i = 0 ; i < 6 ; i++)
		{
			for(int j = 0 ; j < 4 ; j++)
			{
				System.out.println("PRAVEEN OUT : InitializeMaterials "+material[i][j].specular_material[0]);
			}
		}
		
	System.out.println("PRAVEEN OUT : InitializeMaterials");
	}
}
