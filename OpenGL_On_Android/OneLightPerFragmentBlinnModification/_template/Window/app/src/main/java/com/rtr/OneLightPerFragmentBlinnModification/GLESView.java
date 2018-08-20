package com.rtr.OneLightPerFragmentBlinnModification;

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
	
	
	private int[] vao_sphere = new int[1];
    
	private int[] vbo_sphere_position = new int[1];
    
	private int[] vbo_sphere_normal = new int[1];
    
	private int[] vbo_sphere_element = new int[1];

	private int gNumElements;
	
	private int gNumVertices;
	
	
	private float[]lightAmbient = new float[]{ 0.0f,0.0f,0.0f,1.0f };
	
	private float []lightDiffuse = new float[] { 1.0f,1.0f,1.0f,1.0f };

	private float  []lightSpecular = new float[] { 1.0f,1.0f,1.0f,1.0f };

	private float []lightPosition = new float[]{ 100.0f,100.0f,100.0f,1.0f };

	private boolean gbLight;


	
	private float []material_ambient = new float[]{ 0.0f,0.0f,0.0f,1.0f };
	
	private float []material_diffuse = new float[]{ 1.0f,1.0f,1.0f,1.0f };
	
	private float []material_specular = new float[] { 1.0f,1.0f,1.0f,1.0f };
	
	private float material_shininess = 50.0f;

	
	private int model_matrix_uniform;
	
	private int view_matrix_uniform;
	
	private int projection_matrix_uniform;
	
	private float  perspectiveProjectionMatrix[]=new float[16]; // 4x4 matrix
	
	
	private int gbLight_doubleTab_uniform;
	
	private int La_uniform;
	
	private int Ld_uniform;
	
	private int Ls_uniform;
	
	private int light_position_uniform;
	
	private int L_KeyPressed_uniform;

	private int Ka_uniform;
	
	private int Kd_uniform;
	
	private int Ks_uniform;
	
	private int material_shininess_uniform;
	
	private int gbLight_doubleTab;
	
	private int numVertices;
        
	private int numElements;
	
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
         "uniform mat4 u_model_matrix;"+
         "uniform mat4 u_view_matrix;"+
         "uniform mat4 u_projection_matrix;"+
         "uniform mediump int u_double_tap;"+
         "uniform vec4 u_light_position;"+
         "out vec3 transformed_normals;"+
         "out vec3 light_direction;"+
         "out vec3 viewer_vector;"+
         "void main(void)"+
         "{"+
			"if (u_double_tap == 1)"+
			"{"+
				"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;"+
				"transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;"+
				"light_direction = vec3(u_light_position) - eye_coordinates.xyz;"+
				"viewer_vector = -eye_coordinates.xyz;"+
			"}"+
			"gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"+
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
         "in vec3 transformed_normals;"+
         "in vec3 light_direction;"+
         "in vec3 viewer_vector;"+
         "out vec4 FragColor;"+
         "uniform vec3 u_La;"+
         "uniform vec3 u_Ld;"+
         "uniform vec3 u_Ls;"+
         "uniform vec3 u_Ka;"+
         "uniform vec3 u_Kd;"+
         "uniform vec3 u_Ks;"+
         "uniform float u_material_shininess;"+
         "uniform int u_double_tap;"+
         "void main(void)"+
         "{"+
			"vec3 phong_ads_color;"+
			"if(u_double_tap==1)"+
			"{"+
				"vec3 normalized_transformed_normals=normalize(transformed_normals);"+
				"vec3 normalized_light_direction=normalize(light_direction);"+
				"vec3 normalized_viewer_vector=normalize(viewer_vector);"+
				"vec3 ambient = u_La * u_Ka;"+
				"float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction),0.0);"+
				"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;"+
				/*"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);"+*/
				"vec3 H= normalize(u_Ls + normalized_viewer_vector);"+
				"vec3 specular = u_Ls * u_Ks * pow(max(dot(H, transformed_normals), 0.0), u_material_shininess);"+
				/*"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_material_shininess);"+*/
				"phong_ads_color=ambient + diffuse + specular;"+
			"}"+
			"else"+
			"{"+
				"phong_ads_color = vec3(1.0, 1.0, 1.0);"+
			"}"+
         "FragColor = vec4(phong_ads_color, 1.0);"+
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
		
		
		gbLight_doubleTab_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_double_tap");
	
		La_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_La");
		
		Ld_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ld");
		
		Ls_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ls");
		
	
		light_position_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_light_position");
		
			
		Ka_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ka");
		
		
		Kd_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Kd");
		
		
		Ks_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_Ks");
		
		
		material_shininess_uniform = GLES30.glGetUniformLocation(shaderProgramObject, "u_material_shininess");
	//
	 
		//SPHERE
		Sphere sphere=new Sphere();
        
		float sphere_vertices[]=new float[1146];
        float sphere_normals[]=new float[1146];
        float sphere_textures[]=new float[764];
        short sphere_elements[]=new short[2280];
        
		sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        numVertices = sphere.getNumberOfSphereVertices();
        numElements = sphere.getNumberOfSphereElements();
		//
	
		/*SPHERE*/
		GLES30.glGenVertexArrays(1,vao_sphere,0);
		GLES30.glBindVertexArray(vao_sphere[0]);
		
		
		
		/*POSITIONS*/
		GLES30.glGenBuffers(1,vbo_sphere_position ,0);
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,vbo_sphere_position[0]);
		
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
		GLES30.glGenBuffers(1,vbo_sphere_normal ,0);
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER,vbo_sphere_normal[0]);
		
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
		GLES30.glGenBuffers(1,vbo_sphere_element ,0);
		GLES30.glBindBuffer(GLES30.GL_ELEMENT_ARRAY_BUFFER,vbo_sphere_element[0]);

		
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
		GLES30.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			
		
		
		// initialization
		
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
			// set 'u_lighting_enabled' uniform
			GLES30.glUniform1i(gbLight_doubleTab_uniform, 1);
			
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
			GLES30.glUniform1i(gbLight_doubleTab_uniform, 0);
		}
		/**/
		
		
        float modelMatrix[]=new float[16];
        float viewMatrix[]=new float[16];
        
        // SPhere CODE
        // set modelMatrix, modelViewMatrix, rotation matrices to identity matrix
        Matrix.setIdentityM(modelMatrix,0);
        Matrix.setIdentityM(viewMatrix,0);
        //Matrix.setIdentityM(rotationMatrix,0);

        Matrix.translateM(modelMatrix,0,0.0f,0.0f,-1.5f);
        
        //Matrix.setRotateM(rotationMatrix,0,gAngle,1.0f,1.0f,1.0f); // ALL axes rotation by angleCube angle
        
		//Matrix.multiplyMM(modelViewMatrix,0,modelMatrix,0,rotationMatrix,0);
        
        GLES30.glUniformMatrix4fv(model_matrix_uniform,1,false,modelMatrix,0);
		
		GLES30.glUniformMatrix4fv(view_matrix_uniform,1,false,viewMatrix,0);
        
		GLES30.glUniformMatrix4fv(projection_matrix_uniform,1,false,perspectiveProjectionMatrix,0);
        
		
        // bind vao
        GLES30.glBindVertexArray(vao_sphere[0]);
        
        GLES30.glBindBuffer(GLES30.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
        GLES30.glDrawElements(GLES30.GL_TRIANGLES, numElements, GLES30.GL_UNSIGNED_SHORT, 0);
        
        // unbind vao
        GLES30.glBindVertexArray(0);
        
        //delete shader program
        GLES30.glUseProgram(0);
		
		//UPDATE
		//if (gbAnimate_singleTap == 1)
			//Update();
        
		// SWAP BUFFER
        requestRender();
		
	}
	
	public void uninitialize()
	{
   		
		//
		//DESTROY VAO
		if (vao_sphere[0] != 0)
		{
			GLES30.glDeleteVertexArrays(1,vao_sphere,0);
			vao_sphere[0] = 0;
		}


		//DESTROY VBO
		if (vbo_sphere_position[0] != 0)
		{
			GLES30.glDeleteVertexArrays(1, vbo_sphere_position,0);
			vbo_sphere_position[0] = 0;
		}
		if (vbo_sphere_normal[0] != 0)
		{
			GLES30.glDeleteVertexArrays(1, vbo_sphere_normal,0);
			vbo_sphere_normal[0] = 0;
		}
		if (vbo_sphere_element[0] != 0)
		{
			GLES30.glDeleteVertexArrays(1, vbo_sphere_element,0);
			vbo_sphere_element[0] = 0;
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
