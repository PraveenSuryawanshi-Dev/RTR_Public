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

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var gbLight =false;

var sphere=null;

var perspectiveProjectionMatrix;

var modelMatrixUniform, viewMatrixUniform, projectionMatrixUniform;
var laUniform, ldUniform, lsUniform, lightPositionUniform,lightRotationUniform;
var kaUniform, kdUniform, ksUniform, materialShininessUniform;
var LKeyPressedUniform;

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


var lightAmbient = [ 0.0,0.0,0.0];
var lightDiffuse = [ 1.0,1.0,1.0];
var lightSpecular =[ 1.0,1.0,1.0];
var lightPosition =[ 0.0,0.0,0.0,0.0 ];


var gIsXKeyPressed = false;
var gIsYKeyPressed = false;
var gIsZKeyPressed = false;

var sphere_angle = 0.0;	

class Material
{
	constructor(ambient_material,diffuse_material,specular_material,shininess)
	{
		this.ambient_material=ambient_material;
		this.diffuse_material =diffuse_material;
		this.specular_material =specular_material;
		this.shininess =shininess;
	}
}

var material = new Array(6);

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
   
    // vertex shader
    var vertexShaderSourceCode=
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
	"}";

    vertexShaderObject=gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject,vertexShaderSourceCode);
    gl.compileShader(vertexShaderObject);
    if(gl.getShaderParameter(vertexShaderObject,gl.COMPILE_STATUS)==false)
    {
        var error=gl.getShaderInfoLog(vertexShaderObject);
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
    "in vec3 phong_ads_color;" +
	"out vec4 FragColor;" +
	"void main(void)" +
	"{" +
		"FragColor = vec4(phong_ads_color, 1.0);" +
	"}";
    
    fragmentShaderObject=gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject,fragmentShaderSourceCode);
    gl.compileShader(fragmentShaderObject);
    if(gl.getShaderParameter(fragmentShaderObject,gl.COMPILE_STATUS)==false)
    {
        var error=gl.getShaderInfoLog(fragmentShaderObject);
        if(error.length > 0)
        {
            alert(error);
            uninitialize();
        }
    }
    
    // shader program
    shaderProgramObject=gl.createProgram();
    gl.attachShader(shaderProgramObject,vertexShaderObject);
    gl.attachShader(shaderProgramObject,fragmentShaderObject);
    
    // pre-link binding of shader program object with vertex shader attributes
    gl.bindAttribLocation(shaderProgramObject,WebGLMacros.VDG_ATTRIBUTE_VERTEX,"vPosition");
    gl.bindAttribLocation(shaderProgramObject,WebGLMacros.VDG_ATTRIBUTE_NORMAL,"vNormal");

    // linking
    gl.linkProgram(shaderProgramObject);
    if (!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS))
    {
        var error=gl.getProgramInfoLog(shaderProgramObject);
        if(error.length > 0)
        {
            alert(error);
            uninitialize();
        }
    }

    // get Model Matrix uniform location
    modelMatrixUniform=gl.getUniformLocation(shaderProgramObject,"u_model_matrix");
    // get View Matrix uniform location
    viewMatrixUniform=gl.getUniformLocation(shaderProgramObject,"u_view_matrix");
    // get Projection Matrix uniform location
    projectionMatrixUniform=gl.getUniformLocation(shaderProgramObject,"u_projection_matrix");
    
    // get single tap detecting uniform
    LKeyPressedUniform=gl.getUniformLocation(shaderProgramObject,"u_lighting_enabled");
    
    // ambient color intensity of light
    laUniform=gl.getUniformLocation(shaderProgramObject,"u_La");
    // diffuse color intensity of light
    ldUniform=gl.getUniformLocation(shaderProgramObject,"u_Ld");
    // specular color intensity of light
    lsUniform=gl.getUniformLocation(shaderProgramObject,"u_Ls");
    // position of light
    lightPositionUniform=gl.getUniformLocation(shaderProgramObject,"u_light_position");
	
	lightRotationUniform = gl.getUniformLocation(shaderProgramObject, "u_light_rotation");
    
    // ambient reflective color intensity of material
    kaUniform=gl.getUniformLocation(shaderProgramObject,"u_Ka");
    // diffuse reflective color intensity of material
    kdUniform=gl.getUniformLocation(shaderProgramObject,"u_Kd");
    // specular reflective color intensity of material
    ksUniform=gl.getUniformLocation(shaderProgramObject,"u_Ks");
    // shininess of material ( value is conventionally between 1 to 200 )
    materialShininessUniform=gl.getUniformLocation(shaderProgramObject,"u_material_shininess");
    
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
    gl.clearColor(0.25, 0.25, 0.25, 1.0); // black
    
    // initialize projection matrix
    perspectiveProjectionMatrix=mat4.create();
	
	InitializeMaterials();

}

function resize()
{
    // code
    if(bFullscreen==true)
    {
        canvas.width=window.width;
        canvas.height=window.height;
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

function draw()
{
    // code
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    
    gl.useProgram(shaderProgramObject);
    
	if(gbLight)
		gl.uniform1i(LKeyPressedUniform, 1);
	else
		gl.uniform1i(LKeyPressedUniform, 0);


	//DRAWING SPHERE
	 var modelMatrix=mat4.create();
	 var viewMatrix=mat4.create();
	 var rotateMatrix=mat4.create();

	 mat4.translate(modelMatrix, modelMatrix, [0.0,0.0,-4.5]);
			
	//model view
	gl.uniformMatrix4fv(viewMatrixUniform,false,viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform,false,perspectiveProjectionMatrix);
	

	// setting light's properties
    gl.uniform3fv(laUniform, lightAmbient); // ambient intensity of light
    gl.uniform3fv(ldUniform, lightDiffuse); // diffuse intensity of light
    gl.uniform3fv(lsUniform, lightSpecular); // specular intensity of light
    gl.uniform4fv(lightPositionUniform, lightPosition); // light position
		

	var  col = 5;
	for (var i = 0; i < 6; i++)
	{
		for (var j = 0; j < 4; j++)
		{


			var newWidth = (canvas.width / 4.0);
			var newHeight =(canvas.height / 6.0);

			gl.viewport((newWidth*j), newHeight*col, newWidth, newHeight);
			mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(newWidth)/parseFloat(newHeight), 0.1, 100.0);
			

			// setting material's properties
			
			gl.uniform3fv(kaUniform, material[i][j].ambient_material); // ambient reflectivity of material
			gl.uniform3fv(kdUniform, material[i][j].diffuse_material); // diffuse reflectivity of material
			gl.uniform3fv(ksUniform, material[i][j].specular_material); // specular reflectivity of material
			gl.uniform1f(materialShininessUniform, material[i][j].shininess); // material shininess
	
		

			rotateMatrix = mat4.create();
			
			if (gIsXKeyPressed == true)
			{
				mat4.rotateX(rotateMatrix ,rotateMatrix,sphere_angle);
			
				lightPosition[1] = sphere_angle;
				gl.uniform4fv(lightPositionUniform, lightPosition);
				gl.uniformMatrix4fv(lightRotationUniform, false, rotateMatrix);
			}
			else
			if (gIsYKeyPressed == true)
			{
				mat4.rotateY(rotateMatrix ,rotateMatrix,sphere_angle);
			
				lightPosition[0] = sphere_angle;
				gl.uniform4fv(lightPositionUniform,  lightPosition);
				gl.uniformMatrix4fv(lightRotationUniform, false , rotateMatrix);
			}
			else
			if (gIsZKeyPressed == true)
			{
				mat4.rotateZ(rotateMatrix ,rotateMatrix,sphere_angle);

				lightPosition[0] = sphere_angle;
				gl.uniform4fv(lightPositionUniform, lightPosition);
				gl.uniformMatrix4fv(lightRotationUniform, false, rotateMatrix);
			}
			else
			{
				lightPosition[3] = 0.0;
				lightPosition[1] = 0.0;
				lightPosition[2] = 1.0;
				gl.uniform4fv(lightPositionUniform, lightPosition);
			}
			
			gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);

			//START DRAWING
			sphere.draw();
			// UNBING VAO

		}

		col--;
	}

    gl.useProgram(null);
    
	if (sphere_angle >= 720.0)
		sphere_angle = 360.0;
	else
		sphere_angle = sphere_angle + 0.06;
	
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
    
    if(shaderProgramObject)
    {
        if(fragmentShaderObject)
        {
            gl.detachShader(shaderProgramObject,fragmentShaderObject);
            gl.deleteShader(fragmentShaderObject);
            fragmentShaderObject=null;
        }
        
        if(vertexShaderObject)
        {
            gl.detachShader(shaderProgramObject,vertexShaderObject);
            gl.deleteShader(vertexShaderObject);
            vertexShaderObject=null;
        }
        
        gl.deleteProgram(shaderProgramObject);
        shaderProgramObject=null;
    }
}

function keyDown(event)
{
    // code
    switch(event.keyCode)
    {
        case 27: // Escape
            // uninitialize
            uninitialize();
            // close our application's tab
            window.close(); // may not work in Firefox but works in Safari and chrome
            break;
        
        case 76: // for 'L' or 'l'
            if(gbLight==false)
                gbLight=true;
            else
                gbLight=false;
            break;
     
        case 70: // for 'F' or 'f'
            toggleFullScreen();
            break;
		case 88:
			lightPosition[0] = 0.0;
			lightPosition[1] = 0.0;
			lightPosition[2] = 0.0;
			if (gIsXKeyPressed == true)
			{
				gIsXKeyPressed = false;
			}
			else
			{
				gIsXKeyPressed = true;
				gIsYKeyPressed = false;
				gIsZKeyPressed = false;
			}
			break;


		case 89:
			lightPosition[0] = 0.0;
			lightPosition[1] = 0.0;
			lightPosition[2] = 0.0;
			if (gIsYKeyPressed == true)
			{
				gIsYKeyPressed = false;
			}
			else
			{
				gIsYKeyPressed = true;
				gIsXKeyPressed = false;
				gIsZKeyPressed = false;
			}
			break;

		case 90:
			lightPosition[0] = 0.0;
			lightPosition[1] = 0.0;
			lightPosition[2] = 0.0;
			if (gIsZKeyPressed == true)
			{
				gIsZKeyPressed = false;
			}
			else
			{
				gIsZKeyPressed = true;
				gIsXKeyPressed = false;
				gIsYKeyPressed = false;
			}
			break;
    }
}

function mouseDown()
{
    // code
}



function InitializeMaterials()
{
	material [0] = new Array(4)
	material [1] = new Array(4)
	material [2] = new Array(4)
	material [3] = new Array(4)
	material [4] = new Array(4)
	material [5] = new Array(4)

	/***emerald**************************************************************************************/
	material[0][0] = new Material([0.0215,0.1745,0.0215],
								  [0.07568,0.61424,0.07568],
								  [0.633,0.727811,0.633],
								  0.6 * 128.0);


	/***jade**************************************************************************************/
	material[1][0] = new Material([0.135,0.135,0.1575],
								  [0.54,0.89,0.63],
								  [0.316228,0.7316228,0.316228],
								  0.1 * 128.0);


	/***obsidian**************************************************************************************/
	material[2][0] = new Material([0.05375,0.05,0.06625],
								  [0.18275,0.17,0.22525],
								  [0.332741,0.328634,0.346435],
								  0.3 * 128.0);
								  
	/***pearl**************************************************************************************/
	
	material[3][0] = new Material([0.25,0.2075,0.20725],
								  [1.0,0.829,0.829],
								  [0.296648,0.296648,0.296648],
								  0.088* 128.0);
								 

	/***ruby**************************************************************************************/
	material[4][0]= new Material([0.1745,0.01175,0.01175],
								  [0.61424,0.04136,0.04136],
								  [0.727811,0.626959,0.626959],
								  0.6 * 128.0);


	/***turquoise**************************************************************************************/
	
	material[5][0] = new Material([0.1,0.18725,0.1745],
								  [0.396,0.74151,0.69102],
								  [0.297254,0.30829,0.306678],
								  0.1 * 128.0);
								
	/***brass**************************************************************************************/
	
	material[0][1] = new Material([0.329412,0.223529,0.027451],
								  [0.780392,0.568627,0.113725],
								  [0.992157,0.941176,0.807843],
								  0.21794872 * 128.0);


	/***bronze**************************************************************************************/
	
	material[1][1] = new Material([0.2125,0.1275,0.054],
								  [0.714,0.4284,0.4284],
								  [0.393548,0.271906,0.166721],
								  0.2 * 128.0);
								  
	/***chrome**************************************************************************************/
	
	material[2][1]= new Material([0.25,0.25,0.25],
								  [0.4,0.4,0.4],
								  [0.774597,0.774597,0.774597],
								  0.6 * 128.0);

	/****copper*************************************************************************************/
	
	material[3][1] = new Material([0.19125,0.0735,0.0225],
								  [0.7038,0.27048,0.0828],
								  [0.256777,0.137622,0.086014],
								  0.1 * 128.0);
								  

	/***gold**************************************************************************************/
	
	material[4][1] = new Material([0.24725,0.1995,0.0745],
								  [0.75164,0.60648,0.22648],
								  [0.628281,0.555802,0.366065],
								  0.4 * 128.0);
								  
	/***silver**************************************************************************************/
	
	material[5][1] = new Material([0.19225,0.19225,0.19225],
								  [0.50754,0.50754,0.50754],
								  [0.508273,0.508273,0.508273],
								  0.2 * 128.0);


	/***black**************************************************************************************/
	
	material[0][2] = new Material([0.0,0.0,0.0],
								  [0.01,0.01,0.01],
								  [0.50,0.50,0.50],
								  0.25 * 128.0);
								 

	/***cyan**************************************************************************************/
	
	material[1][2] = new Material([0.0,0.1,0.6],
								  [0.0,0.50980392,0.50980392],
								  [0.50196078,0.50196078,0.50196078],
								  0.25 * 128.0);
								 

	/***green**************************************************************************************/
	
	material[2][2]= new Material([0.0,0.0,0.0],
								  [0.1,0.3,0.1],
								  [0.45,0.55,0.45],
								  0.25 * 128.0);								  


	/***red**************************************************************************************/
	
	material[3][2] = new Material([0.0,0.0,0.0],
								  [0.5,0.0,0.0],
								  [0.7,0.6,0.6],
								  0.25 * 128.0);

	/***white**************************************************************************************/
	
	material[4][2] = new Material([0.0,0.0,0.0],
								  [0.55,0.55,0.55],
								  [0.70,0.70,0.70],
								  0.25 * 128.0);

	/***plastic**************************************************************************************/
	
	material[5][2] = new Material([0.0,0.0,0.0],
								  [0.5,0.5,0.0],
								  [0.60,0.60,0.50],
								  0.25 * 128.0);


	/***black**************************************************************************************/
	
	material[0][3] = new Material([0.02,0.02,0.02],
								  [0.01,0.01,0.01],
								  [0.50,0.50,0.50],
								  0.07812 * 128.0);



	/***cyan**************************************************************************************/
	
	material[1][3] = new Material([0.0,0.05,0.05],
								  [0.4,0.5,0.5],
								  [0.04,0.7,0.7],
								  0.078125 * 128.0);
								  
	/********************************************************************************************/


	/***green**************************************************************************************/
	
	material[2][3] = new Material([0.0,0.05,0.0],
								  [0.4,0.5,0.4],
								  [0.04,0.7,0.04],
								  0.078125 * 128.0);

	/***red**************************************************************************************/
	
	material[3][3] = new Material([0.05,0.0,0.0],
								  [0.5,0.4,0.4],
								  [0.7,0.04,0.04],
								  0.078125 * 128.0);

								  

	/***white**************************************************************************************/
	
	material[4][3] = new Material([0.05,0.05,0.05],
								  [0.5,0.5,0.5],
								  [0.7,0.7,0.7],
								  0.078125 * 128.0);
								  

	/***yellow rubber**************************************************************************************/
	
	material[5][3] = new Material([0.05,0.05,0.0],
								  [0.5,0.5,0.4],
								  [0.7,0.7,0.04],
								  0.078125 * 128.0);
								  
	/*******************************************************************************************/
}
