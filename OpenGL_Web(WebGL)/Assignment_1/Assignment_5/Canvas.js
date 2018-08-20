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

var triangle_vao;
var triangle_v_vbo;
var triangle_c_vbo;

var square_vao;
var square_v_vbo;
var triangle_c_vbo;

var mvpUniform;

var perspectiveProjectionMatrix;

var angle = 0.0;

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
	"in vec4 vColor;"+
	"out vec4 out_color;"+
	"uniform mat4 u_mvp_matrix;" +
	"void main(void)" +
	"{" +
		"gl_Position = u_mvp_matrix * vPosition;" +
		"out_color = vColor;"+
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
	"in vec4 out_color;"+
	"out vec4 FragColor;"+
	"void main(void)"+
	"{"+
		"FragColor = out_color;"+
	"}"
	
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
	gl.bindAttribLocation(shaderProgramObject,WebGLMacros.VDG_ATTRIBUTE_COLOR,"vColor");

    
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

    // get MVP uniform location
    mvpUniform=gl.getUniformLocation(shaderProgramObject,"u_mvp_matrix");
    
    // *** vertices, colors, shader attribs, vbo, vao initializations ***
    var triangleVertices=new Float32Array([
											0.0, 1.0, 0.0, // apex
											-1.0, 0.0, 0.0, // left -bottom
											1.0, 0.0 ,0.0, //right -bottm
                                           ]);
	
	var  triangleColor = new Float32Array([
		1.0, 0.0, 0.0, // red
		0.0, 1.0, 0.0, // blue
		0.0, 0.0, 1.0, // green
	]);

	
	var squareVertices=new Float32Array([
											
											1.0, 1.0, 0.0, // right top
											-1.0, 1.0, 0.0, // left top
											-1.0, -1.0 ,0.0, //left -bottm
											1.0, -1.0 ,0.0//right -bottm
                                           ]);
	var  squareColor = new Float32Array([
		0.39, 0.58, 0.93,
		0.39, 0.58, 0.93,
		0.39, 0.58, 0.93,
		0.39, 0.58, 0.93
	]);
										   
	

    triangle_vao = gl.createVertexArray();
    gl.bindVertexArray(triangle_vao);
    
    triangle_v_vbo = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER,triangle_v_vbo);
    gl.bufferData(gl.ARRAY_BUFFER,triangleVertices,gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_VERTEX,
                           3, // 3 is for X,Y,Z co-ordinates in our triangleVertices array
                           gl.FLOAT,
                           false,0,0);
    gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_VERTEX);
    gl.bindBuffer(gl.ARRAY_BUFFER,null);
	
	
	triangle_c_vbo = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER,triangle_c_vbo);
    gl.bufferData(gl.ARRAY_BUFFER,triangleColor,gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_COLOR,
                           3, // 3 is for X,Y,Z co-ordinates in our triangleVertices array
                           gl.FLOAT,
                           false,0,0);
    gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER,null);
	
	
    gl.bindVertexArray(null);

	
	
	square_vao = gl.createVertexArray();
    gl.bindVertexArray(square_vao);
    
    square_v_vbo = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER,square_v_vbo);
    gl.bufferData(gl.ARRAY_BUFFER,squareVertices,gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_VERTEX,
                           3, // 3 is for X,Y,Z co-ordinates in our triangleVertices array
                           gl.FLOAT,
                           false,0,0);
    gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_VERTEX);
    gl.bindBuffer(gl.ARRAY_BUFFER,null);
	
	
	square_c_vbo = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER,square_c_vbo);
    gl.bufferData(gl.ARRAY_BUFFER,squareColor,gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_COLOR,
                           3, // 3 is for X,Y,Z co-ordinates in our triangleVertices array
                           gl.FLOAT,
                           false,0,0);
    gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER,null);
	
    gl.bindVertexArray(null);

	
	
    // set clear color
    gl.clearColor(0.0, 0.0, 0.0, 1.0); // blue
    
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
    
    // Orthographic Projection => left,right,bottom,top,near,far
    /*if (canvas.width <= canvas.height)
        mat4.ortho(orthographicProjectionMatrix, -100.0, 100.0, (-100.0 * (canvas.height / canvas.width)), (100.0 * (canvas.height / canvas.width)), -100.0, 100.0);
    else
        mat4.ortho(orthographicProjectionMatrix, (-100.0 * (canvas.width / canvas.height)), (100.0 * (canvas.width / canvas.height)), -100.0, 100.0, -100.0, 100.0);*/
	
	mat4.perspective(perspectiveProjectionMatrix,45.05,canvas.width /canvas.height, 0.1, 100.0);
}

function draw()
{
    // code
    gl.clear(gl.COLOR_BUFFER_BIT);
    
    gl.useProgram(shaderProgramObject);
    
	/*start drawing triangle*/
    var modelViewMatrix=mat4.create();
    var modelViewProjectionMatrix=mat4.create();
    
	mat4.translate(modelViewMatrix,modelViewMatrix,[-1.0, -0.5, -3.0]);
	
	mat4.rotateY(modelViewMatrix,modelViewMatrix,degreeToRadian(angle));
	
	mat4.multiply(modelViewProjectionMatrix,perspectiveProjectionMatrix,modelViewMatrix);

    gl.uniformMatrix4fv(mvpUniform,false,modelViewProjectionMatrix);

    gl.bindVertexArray(triangle_vao);

    gl.drawArrays(gl.TRIANGLES,0,3);
    
    gl.bindVertexArray(null);
    /*end*/
	
	/*start drawing square*/
	//	modelviewmatrix = vmath::translate(1.5f, 0.0f, -6.0f);

	modelViewMatrix=mat4.create();
    modelViewProjectionMatrix=mat4.create();
    
	mat4.translate(modelViewMatrix,modelViewMatrix,[2.0, 0.0, -6.0]);
	
	mat4.rotateX(modelViewMatrix,modelViewMatrix,degreeToRadian(angle));
	
	mat4.multiply(modelViewProjectionMatrix,perspectiveProjectionMatrix,modelViewMatrix);

    gl.uniformMatrix4fv(mvpUniform,false,modelViewProjectionMatrix);

    gl.bindVertexArray(square_vao);

    gl.drawArrays(gl.TRIANGLE_FAN,0,4);
    
    gl.bindVertexArray(null);	

	/*end*/
    gl.useProgram(null);
    
    // animation loop
	
	update();
	
    requestAnimationFrame(draw, canvas);
}


function degreeToRadian(angle)
{
	return (angle * (Math.PI/180.0));
}
function update()
{

		if(angle >= 360.0)
			 angle = 0.0;
		
		angle = angle +  + 1.0;
		
}


function uninitialize()
{
    // code
    if(triangle_vao)
    {
        gl.deleteVertexArray(triangle_vao);
        triangle_vao=null;
    }
    if(triangle_v_vbo)
    {
        gl.deleteBuffer(triangle_v_vbo);
        triangle_v_vbo=null;
    }
	if(triangle_c_vbo)
    {
        gl.deleteBuffer(triangle_c_vbo);
        triangle_c_vbo=null;
    }
	
	
	if(square_vao)
    {
        gl.deleteBuffer(square_vao);
        square_vao=null;
    }
	if(square_v_vbo)
    {
        gl.deleteBuffer(square_v_vbo);
        square_v_vbo=null;
    }
	if(square_c_vbo)
    {
        gl.deleteBuffer(square_c_vbo);
        square_c_vbo=null;
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
        case 70: // for 'F' or 'f'
            toggleFullScreen();
            break;
    }
}

function mouseDown()
{
    // code
}
