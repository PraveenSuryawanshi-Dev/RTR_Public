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

var vao_pyramid;
var vbo_pyramid_position;
var vbo_pyramid_normal;



var model_matrix_uniform;
var view_matrix_uniform;
var projection_matrix_uniform;
var perspectiveProjectionMatrix;


var La1_uniform;
var Ld1_uniform;
var Ls1_uniform;
var light_position1_uniform;

var La2_uniform;
var Ld2_uniform;
var Ls2_uniform;
var light_position2_uniform;


var Ka_uniform;
var Kd_uniform;
var Ks_uniform;
var material_shininess_uniform;

var L_KeyPressed_uniform;


var anglePyramid=0.0;

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

	
    //GET MVP UNIFORM LOCATION
	model_matrix_uniform = gl.getUniformLocation(shaderProgramObject, "u_model_matrix");
	view_matrix_uniform = gl.getUniformLocation(shaderProgramObject, "u_view_matrix");
	projection_matrix_uniform = gl.getUniformLocation(shaderProgramObject, "u_projection_matrix");

	// 'L' HANDLE KEY TO TOGGLE LIGHT.
	L_KeyPressed_uniform = gl.getUniformLocation(shaderProgramObject, "u_lighting_enabled");

	//AMBIENT COLOR TOKEN
	La1_uniform = gl.getUniformLocation(shaderProgramObject, "u_La1");
	
	// DIFUSE COLOR TOKEN
	Ld1_uniform= gl.getUniformLocation(shaderProgramObject, "u_Ld1");
	
	// SPECULAR COLOR TOKEN
	Ls1_uniform= gl.getUniformLocation(shaderProgramObject, "u_Ls1");
	
	// LIGHT POSITION TOKEN
	light_position1_uniform = gl.getUniformLocation(shaderProgramObject, "u_light_position1");



	//AMBIENT COLOR TOKEN
	La2_uniform = gl.getUniformLocation(shaderProgramObject, "u_La2");

	// DIFUSE COLOR TOKEN
	Ld2_uniform = gl.getUniformLocation(shaderProgramObject, "u_Ld2");

	// SPECULAR COLOR TOKEN
	Ls2_uniform = gl.getUniformLocation(shaderProgramObject, "u_Ls2");

	// LIGHT POSITION TOKEN
	light_position2_uniform = gl.getUniformLocation(shaderProgramObject, "u_light_position2");




	// AMBIENT REFLECTIVE TOKEN
	Ka_uniform = gl.getUniformLocation(shaderProgramObject, "u_Ka");
	
	// DIFUUSE REFLECTIVE TOKEN
	Kd_uniform = gl.getUniformLocation(shaderProgramObject, "u_Kd");
	
	// SPECULAR REFLECTIVE TOKEN
	Ks_uniform = gl.getUniformLocation(shaderProgramObject, "u_Ks");
	
	//SHINYNESS COLOR TOKEN
	material_shininess_uniform = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");
    


    // *** vertices, colors, shader attribs, vbo, vao initializations ***
    var pyramidVertices=new Float32Array([
                                       // Front face
										0.0, 1.0, 0.0,
										-1.0, -1.0, 1.0,
										1.0, -1.0, 1.0,
								
										//Back Face
										0.0, 1.0, 0.0,
										1.0, -1.0, -1.0,
										-1.0, -1.0, -1.0,
								
										//Right face
										0.0, 1.0, 0.0,
										1.0, -1.0, 1.0,
										1.0, -1.0, -1.0,
								
										//Left face
										0.0, 1.0, 0.0,
										-1.0, -1.0, -1.0,
										-1.0, -1.0, 1.0,
                                       ]);
    
    var pyramidNormals=new Float32Array([
                                     // Front face
										0.0, 0.447214, 0.894427,
										0.0, 0.447214, 0.894427,
										0.0, 0.447214, 0.894427,
								
										//Back Face
										0.0, 0.4472141, -0.894427,
										0.0, 0.4472141, -0.894427,
										0.0, 0.4472141, -0.894427,
								
										//Right face
										0.894427, 0.4472141, 0.0,
										0.894427, 0.4472141, 0.0,
										0.894427, 0.4472141, 0.0,
										//Left face
								
										-0.894427, 0.4472141, 0.0,
										-0.894427, 0.4472141, 0.0,
										-0.894427, 0.4472141, 0.0,
                                      ]);
    
    vao_pyramid=gl.createVertexArray();
    gl.bindVertexArray(vao_pyramid);
    
    vbo_pyramid_position = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER,vbo_pyramid_position);
    gl.bufferData(gl.ARRAY_BUFFER,pyramidVertices,gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_VERTEX,
                           3, // 3 is for X,Y,Z co-ordinates in our Vertices array
                           gl.FLOAT,
                           false,0,0);
    gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_VERTEX);
    gl.bindBuffer(gl.ARRAY_BUFFER,null);
    
    vbo_pyramid_normal = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER,vbo_pyramid_normal);
    gl.bufferData(gl.ARRAY_BUFFER,pyramidNormals,gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_NORMAL,
                           3, // 3 is for X,Y,Z co-ordinates in our Normals array
                           gl.FLOAT,
                           false,0,0);
    gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_NORMAL);
    gl.bindBuffer(gl.ARRAY_BUFFER,null);

    gl.bindVertexArray(null);

    // set clear color
    gl.clearColor(0.0, 0.0, 0.0, 1.0); // black
    
    // Depth test will always be enabled
    gl.enable(gl.DEPTH_TEST);

    // depth test to do
    gl.depthFunc(gl.LEQUAL);
    
    // We will always cull back faces for better performance
    gl.enable(gl.CULL_FACE);

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

function draw()
{
    // code
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    
    gl.useProgram(shaderProgramObject);
    
    if(bLKeyPressed == true)
    {
		
		// set 'u_lighting_enabled' uniform
		gl.uniform1i(L_KeyPressed_uniform, 1);

		// setting light's properties
		gl.uniform3f(La1_uniform,0.0,0.0,0.0,1.0);
		gl.uniform3f(Ld1_uniform,1.0,0.0,0.0,0.0);
		gl.uniform3f(Ls1_uniform,0.0,0.0,1.0,0.0);
		gl.uniform4f(light_position1_uniform, -2.0,1.0,1.0,0.0);


		// setting light's properties
		gl.uniform3f(La2_uniform, 0.0,0.0,0.0,0.0);
		gl.uniform3f(Ld2_uniform,0.0,0.0,1.0,0.0);
		gl.uniform3f(Ls2_uniform,  1.0,0.0,0.0,0.0 );
		gl.uniform4f(light_position2_uniform, 2.0,1.0,1.0,0.0);

		
		// setting material's properties
		gl.uniform3f(Ka_uniform, 0.0,0.0,0.0,1.0);
		gl.uniform3f(Kd_uniform, 1.0,1.0,1.0,1.0);
		gl.uniform3f(Ks_uniform, 1.0,1.0,1.0,1.0);
		gl.uniform1f(material_shininess_uniform, 50.0);
    }
    else
    {
        gl.uniform1i(L_KeyPressed_uniform, 0);
    }
    
    var modelMatrix=mat4.create(); // itself creates identity matrix
	var viewMatrix=mat4.create(); // itself creates identity matrix
    
    mat4.translate(modelMatrix, modelMatrix, [0.0,0.0,-5.0]);    
    
    mat4.rotateY(modelMatrix ,modelMatrix, degToRad(anglePyramid));

    gl.uniformMatrix4fv(model_matrix_uniform,false,modelMatrix);
	gl.uniformMatrix4fv(view_matrix_uniform,false,viewMatrix);
    gl.uniformMatrix4fv(projection_matrix_uniform,false,perspectiveProjectionMatrix);

    gl.bindVertexArray(vao_pyramid);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    // actually 2 triangles make 1 cube, so there should be 6 vertices,
    // but as 2 tringles while making square meet each other at diagonal,
    // 2 of 6 vertices are common to both triangles, and hence 6-2=4
    gl.drawArrays(gl.TRIANGLES,0,12);
    
    gl.bindVertexArray(null);
    
    gl.useProgram(null);
    
	
    anglePyramid=anglePyramid+1.0;
    if(anglePyramid>=360.0)
        anglePyramid=anglePyramid-360.0;
    
    // animation loop
    requestAnimationFrame(draw, canvas);
}

function uninitialize()
{
    // code
    if(vao_pyramid)
    {
        gl.deleteVertexArray(vao_pyramid);
        vao_pyramid=null;
    }
    
    if(vbo_pyramid_normal)
    {
        gl.deleteBuffer(vbo_pyramid_normal);
        vbo_pyramid_normal=null;
    }
    
    if(vbo_pyramid_position)
    {
        gl.deleteBuffer(vbo_pyramid_position);
        vbo_pyramid_position=null;
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
            if(bLKeyPressed==false)
                bLKeyPressed=true;
            else
                bLKeyPressed=false;
            break;
        case 70: // for 'F' or 'f'
            toggleFullScreen();
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
