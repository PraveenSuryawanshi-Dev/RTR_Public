package com.rtr.ThreeLightPerFragmentPerVertex;

import java.lang.Math;
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
	
	private int gVertexShaderObject_vertexShader;
	private int gFragmentShaderObject_vertexShader;
	private int gShaderProgramObject_vertexShader;

	private int gVertexShaderObject_fragmentShader;
	private int gFragmentShaderObject_fragmentShader;
	private int gShaderProgramObject_fragmentShader;
	
	
	private int model_matrix_uniform;
	private int view_matrix_uniform;
	private int projection_matrix_uniform;
	private float  perspectiveProjectionMatrix[]=new float[16]; // 4x4 matrix

	
	private int  La1_uniform;
	private int  Ld1_uniform;
	private int  Ls1_uniform;
	private int  light_position1_uniform;
	private int  light_rotation1_uniform;

	private int  La2_uniform;
	private int  Ld2_uniform;
	private int  Ls2_uniform;
	private int  light_position2_uniform;
	private int  light_rotation2_uniform;

	private int  La3_uniform;
	private int  Ld3_uniform;
	private int  Ls3_uniform;
	private int  light_position3_uniform;
	private int  light_rotation3_uniform;


	private int  L_KeyPressed_uniform;

	private int  Ka_uniform;
	private int  Kd_uniform;
	private int  Ks_uniform;
	private int  material_shininess_uniform;


	private  float []light1_ambient = new float[]{ 0.0f,0.0f,0.0f,0.0f };
	private  float [] light1_difuse= new float[]{ 1.0f,0.0f,0.0f,0.0f };
	private  float [] light1_specular = new float[]{ 1.0f,0.0f,0.0f,0.0f };
	private  float [] light1_position = new float[]{ 0.0f,0.0f,0.0f,0.0f };


	private  float [] light2_ambient = new float[]{ 0.0f,0.0f,0.0f,0.0f };
	private  float [] light2_difuse = new float[]{ 0.0f,1.0f,0.0f,0.0f };
	private  float [] light2_specular= new float[]{ 0.0f,1.0f,0.0f,0.0f };
	private  float [] light2_position= new float[]{ 0.0f,0.0f,0.0f,0.0f };


	private  float [] light3_ambient = new float[]{ 0.0f,0.0f,0.0f,0.0f };
	private  float [] light3_difuse = new float[]{ 0.0f,0.0f,1.0f,0.0f };
	private  float [] light3_specular = new float[]{ 0.0f,0.0f,1.0f,0.0f };
	private  float [] light3_position = new float[]{ 0.0f,0.0f,0.0f,0.0f };
	
	private boolean gbLight;
	private boolean bIsFkeyPressed;
	private static int cnt = 0;
	
	private float angleRed = 0.0f;
	private float angleBlue = 0.0f;
	private float angleGreen = 0.0f;
	
	
	
	private  float []  material_ambient= new float[] { 0.0f,0.0f,0.0f,0.0f };
	private  float []  material_difuse= new float[] { 1.0f,1.0f,1.0f,0.0f };
	private  float []  material_specular = new float[] { 1.0f,1.0f,1.0f,0.0f };
	private  float   material_shinyness = 50.0f;


	
	
	private int[]  gVao_sphere = new int[1];
	private int[]  gVbo_sphere_position= new int[1];
	private int[]  gVbo_sphere_normal= new int[1];
	private int[]  gVbo_sphere_element= new int[1];
	private int gNumElements;
	private int gNumVertices;

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
		if (angleRed >= 360.0f)
			angleRed = 0.0f;
		
		angleRed = angleRed + 0.017f;
	
	
		if (angleBlue >= 360.0f)
			angleBlue = 0.0f;
		
		angleBlue = angleBlue + 0.017f;
	
	
		if (angleGreen >= 360.0f)
			angleGreen = 0.0f;
		
		angleGreen = angleGreen + 0.017f;
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
		 
		 if(cnt % 2 == 0 )
		 {
			 bIsFkeyPressed  = false;
		 }
		 else
		 {
			 bIsFkeyPressed = true;
		 }
		 
		 
		cnt++;
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
		
		gVertexShaderObject_vertexShader = GLES30.glCreateShader(GLES30.GL_VERTEX_SHADER);
		
		 String vertexshadersourcecode = String.format(
		 "#version 300 es"+
         "\n"+
		 "in vec4 vPosition;" +
		"in vec3 vNormal;" +

		"uniform int u_lighting_enabled;" +

		"uniform mat4 u_model_matrix;" +
		"uniform mat4 u_view_matrix;" +
		"uniform mat4 u_projection_matrix;" +

		"uniform vec3 u_La1;" +
		"uniform vec3 u_Ld1;" +
		"uniform vec3 u_Ls1;" +
		"uniform vec4 u_light_position1;" +
		"uniform mat4 u_light_rotation1;"+

		"uniform vec3 u_La2;" +
		"uniform vec3 u_Ld2;" +
		"uniform vec3 u_Ls2;" +
		"uniform vec4 u_light_position2;" +
		"uniform mat4 u_light_rotation2;"+

		"uniform vec3 u_La3;" +
		"uniform vec3 u_Ld3;" +
		"uniform vec3 u_Ls3;" +
		"uniform vec4 u_light_position3;" +
		"uniform mat4 u_light_rotation3;"+

		"uniform vec3 u_Ka;" +
		"uniform vec3 u_Kd;" +
		"uniform vec3 u_Ks;" +
		"uniform float u_material_shininess;" +

		"out vec3 phong_ads_color;" +
		"void main(void)" +
		"{" +

		"if(u_lighting_enabled == 1)"+
		"{"+
			"vec4 temp;"+
			"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" +
			"vec3 transformed_normals=normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" +
			
			"temp =u_light_position1 * u_light_rotation1;"+
			"vec3 light_direction1 = normalize(vec3(temp) - eye_coordinates.xyz);" +
			"float tn_dot_ld1 = max(dot(transformed_normals, light_direction1),0.0);" +
			"vec3 ambient1 = u_La1 * u_Ka;" +
			"vec3 diffuse1 = u_Ld1* u_Kd * tn_dot_ld1;" +
			"vec3 reflection_vector1 = reflect(-light_direction1, transformed_normals);" +
			"vec3 viewer_vector = normalize(-eye_coordinates.xyz);" +
			"vec3 specular1 = u_Ls1 * u_Ks * pow(max(dot(reflection_vector1, viewer_vector), 0.0), u_material_shininess);" +

			"temp =u_light_position2 * u_light_rotation2;"+
			"vec3 light_direction2 = normalize(vec3(temp) - eye_coordinates.xyz);" +
			"float tn_dot_ld2 = max(dot(transformed_normals, light_direction2),0.0);" +
			"vec3 ambient2 = u_La2 * u_Ka;" +
			"vec3 diffuse2 = u_Ld2* u_Kd * tn_dot_ld2;" +
			"vec3 reflection_vector2 = reflect(-light_direction2, transformed_normals);" +
			"vec3 specular2 = u_Ls2 * u_Ks * pow(max(dot(reflection_vector2, viewer_vector), 0.0), u_material_shininess);" +
			
			"temp =u_light_position3 * u_light_rotation3;"+
			"vec3 light_direction3= normalize(vec3(temp) - eye_coordinates.xyz);" +
			"float tn_dot_ld3 = max(dot(transformed_normals, light_direction3),0.0);" +
			"vec3 ambient3 = u_La3 * u_Ka;" +
			"vec3 diffuse3 = u_Ld3 * u_Kd * tn_dot_ld3;" +
			
			"vec3 reflection_vector3 = reflect(-light_direction3, transformed_normals);" +
			
			"vec3 specular3 = u_Ls3 * u_Ks * pow(max(dot(reflection_vector3, viewer_vector), 0.0), u_material_shininess);" +
			
			"phong_ads_color= (ambient1+ambient2+ambient3) + (diffuse1+diffuse2+diffuse3)+ (specular1+specular2+specular3);" +
		"}"+
		"else"+
		"{"+
			"phong_ads_color = vec3(1.0, 1.0, 1.0);" +
		"}"+
			
				"gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
	"}"
	);
	
	// provide source code to vertex shader object.
	GLES30.glShaderSource(gVertexShaderObject_vertexShader,vertexshadersourcecode);
	
	//Compile Shader
	GLES30.glCompileShader(gVertexShaderObject_vertexShader);
	
	//Check for compilation error
	
	int[] ishadercompilationstatus = new int[1];
	int[] iinfologlength = new int[1];
	
	String szInfoLog = null;
	
	GLES30.glGetShaderiv(gVertexShaderObject_vertexShader,GLES30.GL_COMPILE_STATUS,ishadercompilationstatus,0);
	
	if(ishadercompilationstatus[0] == GLES30.GL_FALSE)
	{
		GLES30.glGetShaderiv(gVertexShaderObject_vertexShader, GLES30.GL_INFO_LOG_LENGTH, iinfologlength, 0);
		if(iinfologlength[0] > 0 )
		{
			szInfoLog = GLES30.glGetShaderInfoLog(gVertexShaderObject_vertexShader);
			System.out.println("PRAVEEN :"+szInfoLog);
			uninitialize();
			System.exit(0);
		}
	}	
	//END OF VERTEX SHADER
	
	
	//CREATE FRAGMENT SHADER 
	gFragmentShaderObject_vertexShader = GLES30.glCreateShader(GLES30.GL_FRAGMENT_SHADER);
		
		 String fragmentshadersourcecode = String.format(
		"#version 300 es"+
		"\n"+
		"out vec4 FragColor;" +
		"in vec3 phong_ads_color;" +
		"void main(void)" +
		"{" +
			"FragColor = vec4(phong_ads_color, 1.0);" +
		"}"
	);
	
	// provide source code to vertex shader object.
	GLES30.glShaderSource(gFragmentShaderObject_vertexShader,fragmentshadersourcecode);
	
	//Compile Shader
	GLES30.glCompileShader(gFragmentShaderObject_vertexShader);
	
	//Check for compilation error
	
	ishadercompilationstatus[0]= 0;
	iinfologlength[0]= 0;
	szInfoLog = null;
	
	GLES30.glGetShaderiv(gFragmentShaderObject_vertexShader,GLES30.GL_COMPILE_STATUS,ishadercompilationstatus,0);
	
	if(ishadercompilationstatus[0] == GLES30.GL_FALSE)
	{
		GLES30.glGetShaderiv(gFragmentShaderObject_vertexShader,GLES30.GL_INFO_LOG_LENGTH,iinfologlength,0);
		if(iinfologlength[0] > 0 )
		{
			szInfoLog = GLES30.glGetShaderInfoLog(gFragmentShaderObject_vertexShader);
			System.out.println("PRAVEEN :"+szInfoLog);
			uninitialize();
			System.exit(0);
		}
	}	
	
	//END of Fragment Shader
  
  
	//CREATE SHADER PROGRAM OBJECT*/
	 gShaderProgramObject_vertexShader = GLES30.glCreateProgram();
	 
	 //ATTACH VERTEX SHADER OBJECT
	 GLES30.glAttachShader(gShaderProgramObject_vertexShader, gVertexShaderObject_vertexShader);
	 
	//ATTACH FRAGMENT SHADER OBJECT
	GLES30.glAttachShader(gShaderProgramObject_vertexShader, gFragmentShaderObject_vertexShader);
	
	
	//PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITH VERTEX SHADER POSITION ATTRIBUTE
	GLES30.glBindAttribLocation(gShaderProgramObject_vertexShader, GLESMacros.VDG_ATTRIBUTE_POSITION, "vPosition");
	
	//PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITTH Normal ATTRIBUTE
	GLES30.glBindAttribLocation(gShaderProgramObject_vertexShader, GLESMacros.VDG_ATTRIBUTE_NORMAL, "vNormal");
	//

		
	//LINK TWO SHADER TOGHER TO ONE SHADER PROGRAM OBJECT
	GLES30.glLinkProgram(gShaderProgramObject_vertexShader);
	//Check for compilation error
	
	ishadercompilationstatus[0]= 0;
	iinfologlength[0]= 0;
	szInfoLog = null;
	
	GLES30.glGetShaderiv(gShaderProgramObject_vertexShader,GLES30.GL_COMPILE_STATUS,ishadercompilationstatus,0);
	
	if(ishadercompilationstatus[0] == GLES30.GL_FALSE)
	{
		GLES30.glGetShaderiv(gShaderProgramObject_vertexShader,GLES30.GL_INFO_LOG_LENGTH,iinfologlength,0);
		if(iinfologlength[0] > 0 )
		{
			szInfoLog = GLES30.glGetShaderInfoLog(gShaderProgramObject_vertexShader);
			System.out.println("PRAVEEN :"+szInfoLog);
			uninitialize();
			System.exit(0);
		}
	}	
		 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//Fragment SHADER 
		
		gVertexShaderObject_fragmentShader = GLES30.glCreateShader(GLES30.GL_VERTEX_SHADER);
		
		vertexshadersourcecode = String.format(
		 "#version 300 es"+
         "\n"+
		"uniform int u_lighting_enabled;" +
		"in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform mat4 u_model_matrix;" +
		"uniform mat4 u_view_matrix;" +
		"uniform mat4 u_projection_matrix;" +
		"uniform vec4 u_light_position1;" +
		"uniform mat4 u_light_rotation1;"+
		"uniform vec4 u_light_position2;" +
		"uniform mat4 u_light_rotation2;"+
		"uniform vec4 u_light_position3;" +
		"uniform mat4 u_light_rotation3;"+

		"out vec3 transformed_normals;" +
		"out vec3 light_direction1;" +
		"out vec3 light_direction2;" +
		"out vec3 light_direction3;" +
		"out vec3 viewer_vector;" +

		"void main(void)" +
		"{" +
			"if(u_lighting_enabled == 1)" +
			"{" +
			"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" +
			"transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;" +
			"viewer_vector = -eye_coordinates.xyz;" +

			"vec4 temp1 = u_light_position1 * u_light_rotation1 ;"+
			"vec4 temp2 = u_light_position2 * u_light_rotation2 ;"+
			"vec4 temp3 = u_light_position3 * u_light_rotation3 ;"+

				
			"light_direction1 = vec3(temp1) - eye_coordinates.xyz;" +
			"light_direction2 = vec3(temp2) - eye_coordinates.xyz;" +
			"light_direction3 = vec3(temp3) - eye_coordinates.xyz;" +
				
			"}" +
			"gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
		"}"
	);
	
	// provide source code to vertex shader object.
	GLES30.glShaderSource(gVertexShaderObject_fragmentShader,vertexshadersourcecode);
	
	//Compile Shader
	GLES30.glCompileShader(gVertexShaderObject_fragmentShader);
	
	//Check for compilation error	
	ishadercompilationstatus[0]= 0;
	iinfologlength[0]= 0;
	szInfoLog = null;
	
	
	GLES30.glGetShaderiv(gVertexShaderObject_fragmentShader,GLES30.GL_COMPILE_STATUS,ishadercompilationstatus,0);
	
	if(ishadercompilationstatus[0] == GLES30.GL_FALSE)
	{
		GLES30.glGetShaderiv(gVertexShaderObject_fragmentShader, GLES30.GL_INFO_LOG_LENGTH, iinfologlength, 0);
		if(iinfologlength[0] > 0 )
		{
			szInfoLog = GLES30.glGetShaderInfoLog(gVertexShaderObject_fragmentShader);
			System.out.println("PRAVEEN :"+szInfoLog);
			uninitialize();
			System.exit(0);
		}
	}	
	//END OF VERTEX SHADER
	
	
	//CREATE FRAGMENT SHADER 
	gFragmentShaderObject_fragmentShader = GLES30.glCreateShader(GLES30.GL_FRAGMENT_SHADER);
		
		 fragmentshadersourcecode = String.format(
		"#version 300 es"+
		"\n"+
		"in vec3 transformed_normals;" +
		"in vec3 viewer_vector;" +
		"out vec4 FragColor;" +
	
		"in vec3 light_direction1;" +
		"in vec3 light_direction2;" +
		"in vec3 light_direction3;" +

		"uniform vec3 u_La1;" +
		"uniform vec3 u_Ld1;" +
		"uniform vec3 u_Ls1;" +

		"uniform vec3 u_La2;" +
		"uniform vec3 u_Ld2;" +
		"uniform vec3 u_Ls2;" +

		"uniform vec3 u_La3;" +
		"uniform vec3 u_Ld3;" +
		"uniform vec3 u_Ls3;" +


		"uniform vec3 u_Ka;" +
		"uniform vec3 u_Kd;" +
		"uniform vec3 u_Ks;" +
		"uniform float u_material_shininess;" +
		"uniform int u_lighting_enabled;" +
		"void main(void)" +
		"{" +
			"vec3 phong_ads_color;" +
			"if(u_lighting_enabled==1)" +
			"{" +
				"vec3 normalized_transformed_normals=normalize(transformed_normals);" +
				"vec3 normalized_viewer_vector=normalize(viewer_vector);" +

				"vec3 normalized_light_direction1 =normalize(light_direction1);"+
				"vec3 ambient1 = u_La1 * u_Ka;" +
				"float tn_dot_ld1 = max(dot(normalized_transformed_normals, normalized_light_direction1),0.0);" +
				"vec3 diffuse1 = u_Ld1 * u_Kd * tn_dot_ld1;" +
				"vec3 reflection_vector1 = reflect(-normalized_light_direction1, normalized_transformed_normals);" +
				"vec3 specular1 = u_Ls1 * u_Ks * pow(max(dot(reflection_vector1, normalized_viewer_vector), 0.0), u_material_shininess);" +
				
				"vec3 normalized_light_direction2 =normalize(light_direction2);"+
				"vec3 ambient2 = u_La2 * u_Ka;" +
				"float tn_dot_ld2 = max(dot(normalized_transformed_normals, normalized_light_direction2),0.0);" +
				"vec3 diffuse2 = u_Ld2 * u_Kd * tn_dot_ld2;" +
				"vec3 reflection_vector2 = reflect(-normalized_light_direction2, normalized_transformed_normals);" +
				"vec3 specular2 = u_Ls2 * u_Ks * pow(max(dot(reflection_vector2, normalized_viewer_vector), 0.0), u_material_shininess);" +
				

				"vec3 normalized_light_direction3 =normalize(light_direction3);"+
				"vec3 ambient3 = u_La3 * u_Ka;" +
				"float tn_dot_ld3 = max(dot(normalized_transformed_normals, normalized_light_direction3),0.0);" +
				"vec3 diffuse3 = u_Ld3 * u_Kd * tn_dot_ld3;" +
				"vec3 reflection_vector3 = reflect(-normalized_light_direction3, normalized_transformed_normals);" +
				"vec3 specular3 = u_Ls3 * u_Ks * pow(max(dot(reflection_vector3, normalized_viewer_vector), 0.0), u_material_shininess);" +

				"phong_ads_color= (ambient1+ambient2+ambient3) + (diffuse1+diffuse2+diffuse3) + (specular1+specular2+specular3);" +		
				

			"}" +
			"else" +
			"{" +
				"phong_ads_color = vec3(1.0, 1.0, 1.0);" +
			"}" +
			"FragColor = vec4(phong_ads_color, 1.0);" +
		"}"
	);
	
	// provide source code to vertex shader object.
	GLES30.glShaderSource(gFragmentShaderObject_fragmentShader,fragmentshadersourcecode);
	
	//Compile Shader
	GLES30.glCompileShader(gFragmentShaderObject_fragmentShader);
	
	//Check for compilation error
	
	ishadercompilationstatus[0]= 0;
	iinfologlength[0]= 0;
	szInfoLog = null;
	
	GLES30.glGetShaderiv(gFragmentShaderObject_fragmentShader,GLES30.GL_COMPILE_STATUS,ishadercompilationstatus,0);
	
	if(ishadercompilationstatus[0] == GLES30.GL_FALSE)
	{
		GLES30.glGetShaderiv(gFragmentShaderObject_fragmentShader,GLES30.GL_INFO_LOG_LENGTH,iinfologlength,0);
		if(iinfologlength[0] > 0 )
		{
			szInfoLog = GLES30.glGetShaderInfoLog(gFragmentShaderObject_fragmentShader);
			System.out.println("PRAVEEN :"+szInfoLog);
			uninitialize();
			System.exit(0);
		}
	}	
	
	//END of Fragment Shader
  
  
	//CREATE SHADER PROGRAM OBJECT*/
	 gShaderProgramObject_fragmentShader = GLES30.glCreateProgram();
	 
	 //ATTACH VERTEX SHADER OBJECT
	 GLES30.glAttachShader(gShaderProgramObject_fragmentShader, gVertexShaderObject_fragmentShader);
	 
	//ATTACH FRAGMENT SHADER OBJECT
	GLES30.glAttachShader(gShaderProgramObject_fragmentShader, gFragmentShaderObject_fragmentShader);
	
	
	//PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITH VERTEX SHADER POSITION ATTRIBUTE
	GLES30.glBindAttribLocation(gShaderProgramObject_fragmentShader, GLESMacros.VDG_ATTRIBUTE_POSITION, "vPosition");
	
	//PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITTH Normal ATTRIBUTE
	GLES30.glBindAttribLocation(gShaderProgramObject_fragmentShader, GLESMacros.VDG_ATTRIBUTE_NORMAL, "vNormal");
	//

		
	//LINK TWO SHADER TOGHER TO ONE SHADER PROGRAM OBJECT
	GLES30.glLinkProgram(gShaderProgramObject_fragmentShader);
	//Check for compilation error
	
	ishadercompilationstatus[0]= 0;
	iinfologlength[0]= 0;
	szInfoLog = null;
	
	GLES30.glGetShaderiv(gShaderProgramObject_fragmentShader,GLES30.GL_COMPILE_STATUS,ishadercompilationstatus,0);
	
	if(ishadercompilationstatus[0] == GLES30.GL_FALSE)
	{
		GLES30.glGetShaderiv(gShaderProgramObject_fragmentShader,GLES30.GL_INFO_LOG_LENGTH,iinfologlength,0);
		if(iinfologlength[0] > 0 )
		{
			szInfoLog = GLES30.glGetShaderInfoLog(gShaderProgramObject_fragmentShader);
			System.out.println("PRAVEEN :"+szInfoLog);
			uninitialize();
			System.exit(0);
		}
	}	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 
		//SPHERE
		Sphere sphere=new Sphere();
        
		float sphere_vertices[]=new float[1146];
        float sphere_normals[]=new float[1146];
        float sphere_textures[]=new float[764];
        short sphere_elements[]=new short[2280];
        
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
		GLES30.glUseProgram(gShaderProgramObject_vertexShader);
		
		light1_position[1] = 10 *(float) Math.cos(angleRed);
        light1_position[2] = 10 *(float) Math.sin(angleRed);

       light2_position[0] =  10 * (float) Math.cos(angleGreen);
       light2_position[2] =  10 *(float) Math.sin(angleGreen);

       light3_position[0] = 10 * (float) Math.cos(angleBlue);
       light3_position[1] = 10 * (float) Math.sin(angleBlue);

			
			
		model_matrix_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_model_matrix");
		view_matrix_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_view_matrix");
		projection_matrix_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_projection_matrix");
		L_KeyPressed_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_lighting_enabled");
		
		
			//AMBIENT COLOR TOKEN
		La1_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_La1");

		// DIFUSE COLOR TOKEN
		Ld1_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ld1");

		// SPECULAR COLOR TOKEN
		Ls1_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ls1");

		// LIGHT POSITION TOKEN
		light_position1_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_light_position1");

		// LIGHT rotation TOKEN
		light_rotation1_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_light_rotation1");




		//AMBIENT COLOR TOKEN
		La2_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_La2");

		// DIFUSE COLOR TOKEN
		Ld2_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ld2");

		// SPECULAR COLOR TOKEN
		Ls2_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ls2");

		// LIGHT POSITION TOKEN
		light_position2_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_light_position2");


		// LIGHT rotation TOKEN
		light_rotation2_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_light_rotation2");





		//AMBIENT COLOR TOKEN
		La3_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_La3");

		// DIFUSE COLOR TOKEN
		Ld3_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ld3");

		// SPECULAR COLOR TOKEN
		Ls3_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ls3");

		// LIGHT POSITION TOKEN
		light_position3_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_light_position3");

		// LIGHT rotation TOKEN
		light_rotation3_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_light_rotation3");



		// AMBIENT REFLECTIVE TOKEN
		Ka_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ka");

		// DIFUUSE REFLECTIVE TOKEN
		Kd_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Kd");

		// SPECULAR REFLECTIVE TOKEN
		Ks_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ks");

		//SHINYNESS COLOR TOKEN
		material_shininess_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_vertexShader, "u_material_shininess");


        
		
		// For Fragment Shader
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		if (bIsFkeyPressed == true)
		{	
		
		GLES30.glUseProgram(0);

		// START USING SHADER PROGRAM OBJECT
        GLES30.glUseProgram(gShaderProgramObject_fragmentShader);
		
		//GET MVP UNIFORM LOCATION
		model_matrix_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_model_matrix");
		view_matrix_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_view_matrix");
		projection_matrix_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_projection_matrix");
		L_KeyPressed_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_lighting_enabled");
		
		
			//AMBIENT COLOR TOKEN
		La1_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_La1");

		// DIFUSE COLOR TOKEN
		Ld1_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ld1");

		// SPECULAR COLOR TOKEN
		Ls1_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ls1");

		// LIGHT POSITION TOKEN
		light_position1_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_light_position1");

		// LIGHT rotation TOKEN
		light_rotation1_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_light_rotation1");




		//AMBIENT COLOR TOKEN
		La2_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_La2");

		// DIFUSE COLOR TOKEN
		Ld2_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ld2");

		// SPECULAR COLOR TOKEN
		Ls2_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ls2");

		// LIGHT POSITION TOKEN
		light_position2_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_light_position2");


		// LIGHT rotation TOKEN
		light_rotation2_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_light_rotation2");





		//AMBIENT COLOR TOKEN
		La3_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_La3");

		// DIFUSE COLOR TOKEN
		Ld3_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ld3");

		// SPECULAR COLOR TOKEN
		Ls3_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ls3");

		// LIGHT POSITION TOKEN
		light_position3_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_light_position3");

		// LIGHT rotation TOKEN
		light_rotation3_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_light_rotation3");



		// AMBIENT REFLECTIVE TOKEN
		Ka_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ka");

		// DIFUUSE REFLECTIVE TOKEN
		Kd_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Kd");

		// SPECULAR REFLECTIVE TOKEN
		Ks_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ks");

		//SHINYNESS COLOR TOKEN
		material_shininess_uniform = GLES30.glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_material_shininess");
      
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			
		
		
        float modelMatrix[]=new float[16];
        float viewMatrix[]=new float[16];
		float rotateMatrix[]=new float[16];
		  // set modelMatrix, modelViewMatrix, rotation matrices to identity matrix
        
		Matrix.setIdentityM(modelMatrix,0);
        Matrix.setIdentityM(viewMatrix,0);
        Matrix.setIdentityM(rotateMatrix,0);
		
		Matrix.translateM(modelMatrix,0,0.0f,0.0f,-1.5f);
        
		
        // OpenGL-ES drawing
		
		/**/
		if (gbLight == true)
		{
			// set 'u_lighting_enabled' uniform
			GLES30.glUniform1i(L_KeyPressed_uniform, 1);
			
			
			
			//FOR LIGHT 1
			///////////////////////////////////////////////////////////////////////////////////////////
			Matrix.setIdentityM(rotateMatrix,0);
			Matrix.rotateM(rotateMatrix,0,light1_position[1],1.0f,0.0f,0.0f); // ALL axes rotation by angleCube angle
			// Matrix.multiplyMM(modelMatrix,0,modelMatrix,0,rotateMatrix,0);
			//light1_position[1] = angleRed;
			
			
			ByteBuffer byteBuffer=ByteBuffer.allocateDirect(light1_ambient.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			FloatBuffer buffer=byteBuffer.asFloatBuffer();
			buffer.put(light1_ambient);
			buffer.position(0);
			
			GLES30.glUniform3fv(La1_uniform, 1, buffer);
			
			
			
			byteBuffer=ByteBuffer.allocateDirect(light1_difuse.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(light1_difuse);
			buffer.position(0);
			
			GLES30.glUniform3fv(Ld1_uniform, 1, buffer);
			
			
			
			
			byteBuffer=ByteBuffer.allocateDirect(light1_specular.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(light1_specular);
			buffer.position(0);
			
			GLES30.glUniform3fv(Ls1_uniform, 1, buffer);
			
			
			
			byteBuffer=ByteBuffer.allocateDirect(light1_position.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(light1_position);
			buffer.position(0);
			
			GLES30.glUniform4fv(light_position1_uniform, 1, buffer);
			
			GLES30.glUniformMatrix4fv(light_rotation1_uniform, 1, false ,rotateMatrix,0);
			
			
			
			///////////////////////////////////////////////////////////////////////////////////////////
			
			
			
			//FOR LIGHT 2
			///////////////////////////////////////////////////////////////////////////////////////////
			Matrix.setIdentityM(rotateMatrix,0);
			Matrix.rotateM(rotateMatrix,0,light2_position[0],0.0f,1.0f,0.0f); // ALL axes rotation by angleCube angle
			//light2_position[0] = angleGreen;
			
			byteBuffer=ByteBuffer.allocateDirect(light2_ambient.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(light2_ambient);
			buffer.position(0);
			
			GLES30.glUniform3fv(La2_uniform, 1, buffer);
			
			
			
			byteBuffer=ByteBuffer.allocateDirect(light2_difuse.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(light2_difuse);
			buffer.position(0);
			
			GLES30.glUniform3fv(Ld2_uniform, 1, buffer);
			
			
			
			
			byteBuffer=ByteBuffer.allocateDirect(light2_specular.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(light2_specular);
			buffer.position(0);
			
			GLES30.glUniform3fv(Ls2_uniform, 1, buffer);
			
			
			
			byteBuffer=ByteBuffer.allocateDirect(light2_position.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(light2_position);
			buffer.position(0);
			
			GLES30.glUniform4fv(light_position2_uniform, 1, buffer);
			
			GLES30.glUniformMatrix4fv(light_rotation2_uniform, 1, false ,rotateMatrix ,0);
			
			
			
			///////////////////////////////////////////////////////////////////////////////////////////
			
			
			
			//FOR LIGHT 3
			///////////////////////////////////////////////////////////////////////////////////////////
			Matrix.setIdentityM(rotateMatrix,0);
			Matrix.rotateM(rotateMatrix,0,light3_position[0],0.0f,0.0f,1.0f); // ALL axes rotation by angleCube angle
			//light3_position[0] = angleBlue;
			
			byteBuffer=ByteBuffer.allocateDirect(light3_ambient.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(light3_ambient);
			buffer.position(0);
			
			GLES30.glUniform3fv(La3_uniform, 1, buffer);
			
			
			
			byteBuffer=ByteBuffer.allocateDirect(light3_difuse.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(light3_difuse);
			buffer.position(0);
			
			GLES30.glUniform3fv(Ld3_uniform, 1, buffer);
			
			
			
			
			byteBuffer=ByteBuffer.allocateDirect(light3_specular.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(light3_specular);
			buffer.position(0);
			
			GLES30.glUniform3fv(Ls3_uniform, 1, buffer);
			
			
			
			byteBuffer=ByteBuffer.allocateDirect(light3_position.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(light3_position);
			buffer.position(0);
			
			GLES30.glUniform4fv(light_position3_uniform, 1, buffer);
			
			GLES30.glUniformMatrix4fv(light_rotation3_uniform, 1, false,rotateMatrix,0);
			
			
			
			///////////////////////////////////////////////////////////////////////////////////////////
			
			
			
			
			// setting material's properties
			
			byteBuffer=ByteBuffer.allocateDirect(material_ambient.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(material_ambient);
			buffer.position(0);
			
			GLES30.glUniform3fv(Ka_uniform, 1, buffer);
			
			byteBuffer=ByteBuffer.allocateDirect(material_difuse.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(material_difuse);
			buffer.position(0);
			
			GLES30.glUniform3fv(Kd_uniform, 1, buffer);
			
			byteBuffer=ByteBuffer.allocateDirect(material_specular.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
		
			buffer=byteBuffer.asFloatBuffer();
			buffer.put(material_specular);
			buffer.position(0);
			
			GLES30.glUniform3fv(Ks_uniform, 1, buffer);
			
			
			GLES30.glUniform1f(material_shininess_uniform, material_shinyness);
		
		}
		else
		{
			GLES30.glUniform1i(L_KeyPressed_uniform, 0);
		}
		/**/
		
		
        
        // SPhere CODE      
		
        
        GLES30.glUniformMatrix4fv(model_matrix_uniform,1,false,modelMatrix,0);
		
		GLES30.glUniformMatrix4fv(view_matrix_uniform,1,false,viewMatrix,0);
        
		GLES30.glUniformMatrix4fv(projection_matrix_uniform,1,false,perspectiveProjectionMatrix,0);
        
		
		
		
        // bind vao
        GLES30.glBindVertexArray(gVao_sphere[0]);
        
        GLES30.glBindBuffer(GLES30.GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element[0]);
        GLES30.glDrawElements(GLES30.GL_TRIANGLES, gNumElements, GLES30.GL_UNSIGNED_SHORT, 0);
        
        // unbind vao
        GLES30.glBindVertexArray(0);
		
		
        Update();
        
        //delete shader program
        GLES30.glUseProgram(0);
		
		
		// SWAP BUFFER
        requestRender();
		
	}
	
	public void uninitialize()
	{
   		
		//
		//DESTROY VAO
		if (gVao_sphere[0] != 0)
		{
			GLES30.glDeleteVertexArrays(1,gVao_sphere,0);
			gVao_sphere[0] = 0;
		}


		//DESTROY VBO
		if (gVbo_sphere_normal[0] != 0)
		{
			GLES30.glDeleteVertexArrays(1, gVbo_sphere_normal,0);
			gVbo_sphere_normal[0] = 0;
		}
		if (gVbo_sphere_position[0] != 0)
		{
			GLES30.glDeleteVertexArrays(1, gVbo_sphere_position,0);
			gVbo_sphere_position[0] = 0;
		}
		if (gVbo_sphere_element[0] != 0)
		{
			GLES30.glDeleteVertexArrays(1, gVbo_sphere_element,0);
			gVbo_sphere_element[0] = 0;
		}
		
		
		
		//
///
        if(gShaderProgramObject_vertexShader != 0)
        {
            if(gVertexShaderObject_vertexShader != 0)
            {
                GLES30.glDetachShader(gShaderProgramObject_vertexShader, gVertexShaderObject_vertexShader);
                GLES30.glDeleteShader(gVertexShaderObject_vertexShader);
                gVertexShaderObject_vertexShader = 0;
            }
            
            if(gFragmentShaderObject_vertexShader != 0)
            {
                GLES30.glDetachShader(gShaderProgramObject_vertexShader, gFragmentShaderObject_vertexShader);
               	GLES30.glDeleteShader(gFragmentShaderObject_vertexShader);
                gFragmentShaderObject_vertexShader = 0;
            }
        }

        // DELETE SHADER PROGRAM OBJECT
        if(gShaderProgramObject_vertexShader != 0)
        {
            GLES30.glDeleteProgram(gShaderProgramObject_vertexShader);
            gShaderProgramObject_vertexShader = 0;
        }
		
		
		
		
		///
        if(gShaderProgramObject_fragmentShader != 0)
        {
            if(gShaderProgramObject_fragmentShader != 0)
            {
                GLES30.glDetachShader(gShaderProgramObject_fragmentShader, gShaderProgramObject_fragmentShader);
                GLES30.glDeleteShader(gShaderProgramObject_fragmentShader);
                gShaderProgramObject_fragmentShader = 0;
            }
            
            if(gFragmentShaderObject_fragmentShader != 0)
            {
                GLES30.glDetachShader(gShaderProgramObject_fragmentShader, gFragmentShaderObject_fragmentShader);
               	GLES30.glDeleteShader(gFragmentShaderObject_fragmentShader);
                gFragmentShaderObject_fragmentShader = 0;
            }
        }

        // DELETE SHADER PROGRAM OBJECT
        if(gShaderProgramObject_fragmentShader != 0)
        {
            GLES30.glDeleteProgram(gShaderProgramObject_fragmentShader);
            gShaderProgramObject_fragmentShader = 0;
        }
		
	} 
}
