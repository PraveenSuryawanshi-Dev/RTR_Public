// global variables
var canvas=null;
var gl=null; // webgl context
var bFullscreen=false;
var canvas_original_width;
var canvas_original_height;

const WebGLMacros= // when whole 'WebGLMacros' is 'const', all inside it are automatically 'const'
{
VDG_ATTRIBUTE_VERTEX:0,
VDG_ATTRIBUTE_COLOR:1,
VDG_ATTRIBUTE_NORMAL:2,
VDG_ATTRIBUTE_TEXTURE0:3,
};

var vertexShaderObject_v;
var fragmentShaderObject_v;
var shaderProgramObject_v;

var vertexShaderObject_f;
var fragmentShaderObject_f;
var shaderProgramObject_f;

var light_ambient=[0.0,0.0,0.0];
var light_diffuse=[1.0,1.0,1.0];
var light_specular=[1.0,1.0,1.0];
var light_position=[100.0,100.0,100.0,1.0];

var material_ambient= [0.0,0.0,0.0];
var material_diffuse= [1.0,1.0,1.0];
var material_specular= [1.0,1.0,1.0];
var material_shininess= 50.0;

var sphere=null;

var perspectiveProjectionMatrix;

var modelMatrixUniform, viewMatrixUniform, projectionMatrixUniform;
var laUniform1, ldUniform1, lsUniform1, lightPositionUniform1,lightRotationUniform1;
var laUniform2, ldUniform2, lsUniform2, lightPositionUniform2,lightRotationUniform2;
var laUniform3, ldUniform3, lsUniform3, lightPositionUniform3,lightRotationUniform3;

var kaUniform, kdUniform, ksUniform, materialShininessUniform;
var LKeyPressedUnifor0m,LKeyPressedUniform_patch;

var bLKeyPressed=false;

// To start animation : To have requestAnimationFrame() to be called "cross-browser" compatible
var requestAnimationFrame =
window.requestAnimationFrame ||
window.webkitRequestAnimationFrame ||
window.mozRequestAnimationFrame ||
window.oRequestAnimationFrame ||
window.msRequestAnimationFrame;

// To stop animation : To have cancelAnimationFrame() to be called "cross-browser" compatible
var cancelAnimationFrame =
window.cancelAnimationFrame ||
window.webkitCancelRequestAnimationFrame || window.webkitCancelAnimationFrame ||
window.mozCancelRequestAnimationFrame || window.mozCancelAnimationFrame ||
window.oCancelRequestAnimationFrame || window.oCancelAnimationFrame ||
window.msCancelRequestAnimationFrame || window.msCancelAnimationFrame;


var light1_ambient=[ 0.0,0.0,0.0];
var light1_difuse=[ 1.0,0.0,0.0];
var light1_specular=[ 1.0,0.0,0.0];
var light1_position=[ 0.0,0.0,0.0,0.0];


var light2_ambient=[ 0.0,0.0,0.0];
var light2_difuse=[ 0.0,1.0,0.0];
var light2_specular=[ 0.0,1.0,0.0];
var light2_position=[ 0.0,0.0,0.0,0.0];


var light3_ambient=[ 0.0,0.0,0.0];
var light3_difuse=[ 0.0,0.0,1.0];
var light3_specular=[0.0,0.0,1.0];
var light3_position=[ 0.0,0.0,0.0,0.0];


var material_ambient=[ 0.0,0.0,0.0]
var material_difuse=[ 1.0,1.0,1.0];
var material_specular=[ 1.0,1.0,1.0];
var material_shinyness = 50.0;



var angleRed = 0.0;
var angleBlue = 0.0;
var angleGreen = 0.0;


var bLKeyPressed= false;
var bIsQKeyPressed =false;
var bIsAKeyPressed = false;
var bIsLKeyPressed = false;
var bIsFkeyPressed = false;
var bIsVkeyPressed = true;
var bIsEscKeyPressed = false;

// onload function
function main()
{
    // get <canvas> element
    canvas = document.getElementById("AMC");
    if(!canvas)
        console.log("Obtaining Canvas Failed\n");
    else
        console.log("Obtaining Canvas Succeeded\n");
    canvas_original_width=canvas.width;
    canvas_original_height=canvas.height;
    
    // register keyboard's keydown event handler
    window.addEventListener("keydown", keyDown, false);
    window.addEventListener("click", mouseDown, false);
    window.addEventListener("resize", resize, false);

    // initialize WebGL
    init();
    
    // start drawing here as warming-up
    resize();
    draw();
}

function toggleFullScreen()
{
    // code
    var fullscreen_element =
    document.fullscreenElement ||
    document.webkitFullscreenElement ||
    document.mozFullScreenElement ||
    document.msFullscreenElement ||
    null;

    // if not fullscreen
    if(fullscreen_element==null)
    {
        if(canvas.requestFullscreen)
            canvas.requestFullscreen();
        else if(canvas.mozRequestFullScreen)
            canvas.mozRequestFullScreen();
        else if(canvas.webkitRequestFullscreen)
            canvas.webkitRequestFullscreen();
        else if(canvas.msRequestFullscreen)
            canvas.msRequestFullscreen();
        bFullscreen=true;
    }
    else // if already fullscreen
    {
        if(document.exitFullscreen)
            document.exitFullscreen();
        else if(document.mozCancelFullScreen)
            document.mozCancelFullScreen();
        else if(document.webkitExitFullscreen)
            document.webkitExitFullscreen();
        else if(document.msExitFullscreen)
            document.msExitFullscreen();
        bFullscreen=false;
    }
}


function initializationsOfVertexShaderProgram()
{
	// vertex shader
    var vertexShaderSourceCode=
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
	"}";

    vertexShaderObject_v=gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject_v,vertexShaderSourceCode);
    gl.compileShader(vertexShaderObject_v);
    if(gl.getShaderParameter(vertexShaderObject_v,gl.COMPILE_STATUS)==false)
    {
        var error=gl.getShaderInfoLog(vertexShaderObject_v);
        if(error.length > 0)
        {
            alert(error);
            uninitialize();
        }
    }
    
    // fragment shader
    var fragmentShaderSourceCode=
    "#version 300 es"+
    "\n"+
	"precision highp float;"+
    "out vec4 FragColor;" +
 	"in vec3 phong_ads_color;" +
	"void main(void)" +
	"{" +
		"FragColor = vec4(phong_ads_color, 1.0);" +
	"}"
    
    fragmentShaderObject_v=gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject_v,fragmentShaderSourceCode);
    gl.compileShader(fragmentShaderObject_v);
    if(gl.getShaderParameter(fragmentShaderObject_v,gl.COMPILE_STATUS)==false)
    {
        var error=gl.getShaderInfoLog(fragmentShaderObject_v);
        if(error.length > 0)
        {
            alert(error);
            uninitialize();
        }
    }
    
    // shader program
    shaderProgramObject_v=gl.createProgram();
    gl.attachShader(shaderProgramObject_v,vertexShaderObject_v);
    gl.attachShader(shaderProgramObject_v,fragmentShaderObject_v);
    
    // pre-link binding of shader program object with vertex shader attributes
    gl.bindAttribLocation(shaderProgramObject_v,WebGLMacros.VDG_ATTRIBUTE_VERTEX,"vPosition");
    gl.bindAttribLocation(shaderProgramObject_v,WebGLMacros.VDG_ATTRIBUTE_NORMAL,"vNormal");

    // linking
    gl.linkProgram(shaderProgramObject_v);
    if (!gl.getProgramParameter(shaderProgramObject_v, gl.LINK_STATUS))
    {
        var error=gl.getProgramInfoLog(shaderProgramObject_v);
        if(error.length > 0)
        {
            alert(error);
            uninitialize();
        }
    }
	
}


function initializationsOfFragmentShaderProgram()
{
 
 // vertex shader
   var  vertexShaderSourceCode=
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
			"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" +
			"transformed_normals = mat3(u_view_matrix * u_model_matrix) * vNormal;" +
			
			"viewer_vector = -eye_coordinates.xyz;" +
	
			"vec4 temp1 = u_light_position1 * u_light_rotation1;"+
			"vec4 temp2 = u_light_position2 * u_light_rotation2;"+
			"vec4 temp3 = u_light_position3 * u_light_rotation3;"+
	
				
			"light_direction1 = vec3(temp1) - eye_coordinates.xyz;" +
			"light_direction2 = vec3(temp2) - eye_coordinates.xyz;" +
			"light_direction3 = vec3(temp3) - eye_coordinates.xyz;" +
			
		"}" +
		
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
	"}";

    vertexShaderObject_f = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject_f,vertexShaderSourceCode);
    gl.compileShader(vertexShaderObject_f);
    if(gl.getShaderParameter(vertexShaderObject_f,gl.COMPILE_STATUS)==false)
    {
         error=gl.getShaderInfoLog(vertexShaderObject_f);
        if(error.length > 0)
        {
            alert(error);
            uninitialize();
        }
    }
    
    // fragment shader
   var  fragmentShaderSourceCode=
    "#version 300 es"+
    "\n"+
    "precision highp float;"+
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
	"uniform int u_lighting_enabled_patch;" +
	
	"void main(void)" +
	"{" +
		"vec3 phong_ads_color;" +
		"if(u_lighting_enabled_patch == 1)" +
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
    
    fragmentShaderObject_f=gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject_f,fragmentShaderSourceCode);
    gl.compileShader(fragmentShaderObject_f);
    if(gl.getShaderParameter(fragmentShaderObject_f,gl.COMPILE_STATUS)==false)
    {
        var error=gl.getShaderInfoLog(fragmentShaderObject_f);
        if(error.length > 0)
        {
            alert(error);
            uninitialize();
        }
    }
    
    // shader program
    shaderProgramObject_f=gl.createProgram();
    gl.attachShader(shaderProgramObject_f,vertexShaderObject_f);
    gl.attachShader(shaderProgramObject_f,fragmentShaderObject_f);
    
    // pre-link binding of shader program object with vertex shader attributes
    gl.bindAttribLocation(shaderProgramObject_f,WebGLMacros.VDG_ATTRIBUTE_VERTEX,"vPosition");
    gl.bindAttribLocation(shaderProgramObject_f,WebGLMacros.VDG_ATTRIBUTE_NORMAL,"vNormal");

    // linking
    gl.linkProgram(shaderProgramObject_f);
    if (!gl.getProgramParameter(shaderProgramObject_f, gl.LINK_STATUS))
    {
         var error=gl.getProgramInfoLog(shaderProgramObject_f);
        if(error.length > 0)
        {
            alert(error);
            uninitialize();
        }
    }

	
}



function init()
{
    // code
    // get WebGL 2.0 context
    gl = canvas.getContext("webgl2");
    if(gl==null) // failed to get context
    {
        console.log("Failed to get the rendering context for WebGL");
        return;
    }
    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;
   
/********************************** vertex Shader ******************************************************************/
	initializationsOfVertexShaderProgram();
/******************** Fragment Shader ***********************************************************/
	initializationsOfFragmentShaderProgram();
/************************************************************************************************/
    // *** vertices, colors, shader attribs, vbo, vao initializations ***
    sphere=new Mesh();
    makeSphere(sphere,2.0,30,30);

    // Depth test will always be enabled
    gl.enable(gl.DEPTH_TEST);
    
    // depth test to do
    gl.depthFunc(gl.LEQUAL);
    
    // We will always cull back faces for better performance
    gl.enable(gl.CULL_FACE);
    
    // set clear color
    gl.clearColor(0.0, 0.0, 0.0, 1.0); // black
    
    // initialize projection matrix
    perspectiveProjectionMatrix=mat4.create();
}

function resize()
{
    // code
    if(bFullscreen==true)
    {
        canvas.width=window.innerWidth;
        canvas.height=window.innerHeight;
    }
    else
    {
        canvas.width=canvas_original_width;
        canvas.height=canvas_original_height;
    }
   
    // set the viewport to match
    gl.viewport(0, 0, canvas.width, canvas.height);
    
    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width)/parseFloat(canvas.height), 0.1, 100.0);
}

function initializeVertexShaderUniforms()
{
	
    // get Model Matrix uniform location
    modelMatrixUniform=gl.getUniformLocation(shaderProgramObject_v,"u_model_matrix");
    // get View Matrix uniform location
    viewMatrixUniform=gl.getUniformLocation(shaderProgramObject_v,"u_view_matrix");
    // get Projection Matrix uniform location
    projectionMatrixUniform=gl.getUniformLocation(shaderProgramObject_v,"u_projection_matrix");
    
    // get single tap detecting uniform
    LKeyPressedUniform=gl.getUniformLocation(shaderProgramObject_v,"u_lighting_enabled");
	
    // ambient color intensity of light
    laUniform1=gl.getUniformLocation(shaderProgramObject_v,"u_La1");
    // diffuse color intensity of light
    ldUniform1=gl.getUniformLocation(shaderProgramObject_v,"u_Ld1");
    // specular color intensity of light
    lsUniform1=gl.getUniformLocation(shaderProgramObject_v,"u_Ls1");
    // position of light
    lightPositionUniform1=gl.getUniformLocation(shaderProgramObject_v,"u_light_position1");
	// rotation of light
    lightRotationUniform1=gl.getUniformLocation(shaderProgramObject_v,"u_light_rotation1");
	
	
	 // ambient color intensity of light
    laUniform2=gl.getUniformLocation(shaderProgramObject_v,"u_La2");
    // diffuse color intensity of light
    ldUniform2=gl.getUniformLocation(shaderProgramObject_v,"u_Ld2");
    // specular color intensity of light
    lsUniform2=gl.getUniformLocation(shaderProgramObject_v,"u_Ls2");
    // position of light
    lightPositionUniform2=gl.getUniformLocation(shaderProgramObject_v,"u_light_position2");
	// rotation of light
    lightRotationUniform2=gl.getUniformLocation(shaderProgramObject_v,"u_light_rotation2");
	
	
	 // ambient color intensity of light
    laUniform3=gl.getUniformLocation(shaderProgramObject_v,"u_La3");
    // diffuse color intensity of light
    ldUniform3=gl.getUniformLocation(shaderProgramObject_v,"u_Ld3");
    // specular color intensity of light
    lsUniform3=gl.getUniformLocation(shaderProgramObject_v,"u_Ls3");
    // position of light
    lightPositionUniform3=gl.getUniformLocation(shaderProgramObject_v,"u_light_position3");
	// rotation of light
    lightRotationUniform3=gl.getUniformLocation(shaderProgramObject_v,"u_light_rotation3");
	
	
    // ambient reflective color intensity of material
    kaUniform=gl.getUniformLocation(shaderProgramObject_v,"u_Ka");
    // diffuse reflective color intensity of material
    kdUniform=gl.getUniformLocation(shaderProgramObject_v,"u_Kd");
    // specular reflective color intensity of material
    ksUniform=gl.getUniformLocation(shaderProgramObject_v,"u_Ks");
    // shininess of material ( value is conventionally between 1 to 200 )
    materialShininessUniform=gl.getUniformLocation(shaderProgramObject_v,"u_material_shininess");
    
}

function initializeFragmentShaderUniforms()
{
	// get Model Matrix uniform location
    modelMatrixUniform=gl.getUniformLocation(shaderProgramObject_f,"u_model_matrix");
    // get View Matrix uniform location
    viewMatrixUniform=gl.getUniformLocation(shaderProgramObject_f,"u_view_matrix");
    // get Projection Matrix uniform location
    projectionMatrixUniform=gl.getUniformLocation(shaderProgramObject_f,"u_projection_matrix");
    
    // get single tap detecting uniform
    LKeyPressedUniform=gl.getUniformLocation(shaderProgramObject_f,"u_lighting_enabled");	
    LKeyPressedUniform_patch = gl.getUniformLocation(shaderProgramObject_f,"u_lighting_enabled_patch");
    
    // ambient color intensity of light
    laUniform1=gl.getUniformLocation(shaderProgramObject_f,"u_La1");
    // diffuse color intensity of light
    ldUniform1=gl.getUniformLocation(shaderProgramObject_f,"u_Ld1");
    // specular color intensity of light
    lsUniform1=gl.getUniformLocation(shaderProgramObject_f,"u_Ls1");
    // position of light
    lightPositionUniform1=gl.getUniformLocation(shaderProgramObject_f,"u_light_position1");
	// rotation of light
    lightRotationUniform1=gl.getUniformLocation(shaderProgramObject_f,"u_light_rotation1");
	
	
	 // ambient color intensity of light
    laUniform2=gl.getUniformLocation(shaderProgramObject_f,"u_La2");
    // diffuse color intensity of light
    ldUniform2=gl.getUniformLocation(shaderProgramObject_f,"u_Ld2");
    // specular color intensity of light
    lsUniform2=gl.getUniformLocation(shaderProgramObject_f,"u_Ls2");
    // position of light
    lightPositionUniform2=gl.getUniformLocation(shaderProgramObject_f,"u_light_position2");
	// rotation of light
    lightRotationUniform2=gl.getUniformLocation(shaderProgramObject_f,"u_light_rotation2");
	
	
	 // ambient color intensity of light
    laUniform3=gl.getUniformLocation(shaderProgramObject_f,"u_La3");
    // diffuse color intensity of light
    ldUniform3=gl.getUniformLocation(shaderProgramObject_f,"u_Ld3");
    // specular color intensity of light
    lsUniform3=gl.getUniformLocation(shaderProgramObject_f,"u_Ls3");
    // position of light
    lightPositionUniform3=gl.getUniformLocation(shaderProgramObject_f,"u_light_position3");
	// rotation of light
    lightRotationUniform3=gl.getUniformLocation(shaderProgramObject_f,"u_light_rotation3");
	
	
    // ambient reflective color intensity of material
    kaUniform=gl.getUniformLocation(shaderProgramObject_f,"u_Ka");
    // diffuse reflective color intensity of material
    kdUniform=gl.getUniformLocation(shaderProgramObject_f,"u_Kd");
    // specular reflective color intensity of material
    ksUniform=gl.getUniformLocation(shaderProgramObject_f,"u_Ks");
    // shininess of material ( value is conventionally between 1 to 200 )
    materialShininessUniform=gl.getUniformLocation(shaderProgramObject_f,"u_material_shininess");
}

function draw()
{
    // code
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    
   
	// vertex shader or fragment shader to use
	if(bIsVkeyPressed == true)
	{
		 gl.useProgram(null);
		 gl.useProgram(shaderProgramObject_v);
		 initializeVertexShaderUniforms();
		
    
	}
	else
	{
		console.log("fragment shader runnning...");
		gl.useProgram(null);
		gl.useProgram(shaderProgramObject_f);
		initializeFragmentShaderUniforms();
		
		if(bIsLKeyPressed == true)
		{
			gl.uniform1i(LKeyPressedUniform_patch, 1); 
		}
		else
		{
			gl.uniform1i(LKeyPressedUniform_patch, 0); 
		}
		
	}
	
	// turn on light and off
    if(bIsLKeyPressed==true)
    {
		var rotateMatrix = mat4.create(); 
        gl.uniform1i(LKeyPressedUniform, 1);        
		
		// setting light's properties. 1
		light1_position[1] = 10 * Math.cos(angleRed);
		light1_position[2] = 10 * Math.sin(angleRed);
		mat4.rotateX(rotateMatrix ,rotateMatrix, light1_position[1]);
		

		 gl.uniform3fv(laUniform1,  light1_ambient);
		 gl.uniform3fv(ldUniform1,  light1_difuse);
		 gl.uniform3fv(lsUniform1,  light1_specular);
		 gl.uniform4fv(lightPositionUniform1,  light1_position);
		 gl.uniformMatrix4fv(lightRotationUniform1,  false,rotateMatrix);


		 // setting light's properties  2
		  rotateMatrix = mat4.create();
		 light2_position[0] =  10 * Math.cos(angleGreen);
         light2_position[2] =  10 * Math.sin(angleGreen);		
		 mat4.rotateY(rotateMatrix ,rotateMatrix, light2_position[0]);
		

		 gl.uniform3fv(laUniform2,  light2_ambient);
		 gl.uniform3fv(ldUniform2,  light2_difuse);
		 gl.uniform3fv(lsUniform2,  light2_specular);
		 gl.uniform4fv(lightPositionUniform2, light2_position);
		 gl.uniformMatrix4fv(lightRotationUniform2,  false, rotateMatrix);


		 // setting light's properties 3 

		 rotateMatrix = mat4.create();
         light3_position[0] = 10 *  Math.cos(angleBlue);
         light3_position[1] = 10 *  Math.sin(angleBlue);
		 mat4.rotateZ(rotateMatrix ,rotateMatrix, light3_position[0]);

		 gl.uniform3fv(laUniform3, light3_ambient);
		 gl.uniform3fv(ldUniform3, light3_difuse);
		 gl.uniform3fv(lsUniform3, light3_specular);
		 gl.uniform4fv(lightPositionUniform3,  light3_position);
		 gl.uniformMatrix4fv(lightRotationUniform3,false,rotateMatrix);



		// setting material's properties
		 gl.uniform3fv(kaUniform, material_ambient);          // ambient reflectivity of material
		 gl.uniform3fv(kdUniform, material_difuse);           // diffuse reflectivity of material
		 gl.uniform3fv(ksUniform, material_specular);          // specular reflectivity of material
		 gl.uniform1f(materialShininessUniform,material_shinyness);// material shininess	 
		 
		 
    }
    else
    {
        gl.uniform1i(LKeyPressedUniform, 0);
    }
    
    var modelMatrix=mat4.create();
    var viewMatrix=mat4.create();

    mat4.translate(modelMatrix, modelMatrix, [0.0,0.0,-6.0]);
    
    gl.uniformMatrix4fv(modelMatrixUniform,false,modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform,false,viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform,false,perspectiveProjectionMatrix);
    
    sphere.draw();
   
    gl.useProgram(null);
    
	
	
	
	
	
	//Update.
	if (angleRed >= 360.0)
		angleRed = 0.0;
	
		angleRed = angleRed +  0.003;


	if (angleBlue >= 360.0)
		angleBlue = 0.0;
	
		angleBlue = angleBlue +  0.003;


	if (angleGreen >= 360.0)
		angleGreen = 0.0;
	
		angleGreen = angleGreen + 0.003;	
	//
	
	
    // animation loop
    requestAnimationFrame(draw, canvas);
}

function uninitialize()
{
    // code
    if(sphere)
    {
        sphere.deallocate();
        sphere=null;
    }
    
    if(shaderProgramObject_v)
    {
        if(fragmentShaderObject_v)
        {
            gl.detachShader(shaderProgramObject_v,fragmentShaderObject_v);
            gl.deleteShader(fragmentShaderObject_v);
            fragmentShaderObject_v=null;
        }
        
        if(vertexShaderObject_v)
        {
            gl.detachShader(shaderProgramObject_v,vertexShaderObject_v);
            gl.deleteShader(vertexShaderObject_v);
            vertexShaderObject_v=null;
        }
        
        gl.deleteProgram(shaderProgramObject_v);
        shaderProgramObject_v=null;
    }
	
	
	if(shaderProgramObject_f)
    {
        if(fragmentShaderObject_f)
        {
            gl.detachShader(shaderProgramObject_f,fragmentShaderObject_f);
            gl.deleteShader(fragmentShaderObject_f);
            fragmentShaderObject_f=null;
        }
        
        if(vertexShaderObject_f)
        {
            gl.detachShader(shaderProgramObject_f,vertexShaderObject_f);
            gl.deleteShader(vertexShaderObject_f);
            vertexShaderObject_f=null;
        }
        
        gl.deleteProgram(shaderProgramObject_f);
        shaderProgramObject_f=null;
    }
}

function keyDown(event)
{
    // code
    switch(event.keyCode)
    {
       case 81:
			 bIsQKeyPressed = true;
			 window.close(); // may not work in Firefox but works in Safari and chrome
			break;
			
        case 27: // Escape
            // uninitialize
            toggleFullScreen();
            // close our application's tab           
            break;
			
        case 76: // for 'L' or 'l'
          
		    if (bIsLKeyPressed == false)
			{
				bIsLKeyPressed = true;
			}
			else
			{
				bIsLKeyPressed = false;
			}
			break;
			
        case 70: // for 'F' or 'f'
			if (bIsFkeyPressed == false)
			{
				bIsFkeyPressed = true;
				bIsVkeyPressed = false;
			}
			break;
		case 86: //for V or v

			if (bIsVkeyPressed == false)
			{				
				bIsVkeyPressed = true;
				bIsFkeyPressed = false;
			}
		
            break;
    }
}

function mouseDown()
{
    // code
}


function degToRad(degrees)
{
    // code
    return(degrees * Math.PI / 180);
}
