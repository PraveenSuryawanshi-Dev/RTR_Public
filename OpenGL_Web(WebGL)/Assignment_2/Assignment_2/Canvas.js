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


var vao_quad;
var vbo_position;
var vbo_texture;
var smiley_texture=0;

var mvpUniform;
var perspectiveProjectionMatrix;
var uniform_texture0_sampler;



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
    "in vec4 vPosition;"+
    "in vec2 vTexture0_Coord;"+
    "out vec2 out_texture0_coord;"+
    "uniform mat4 u_mvp_matrix;"+
    "void main(void)"+
    "{"+
		"gl_Position = u_mvp_matrix * vPosition;"+
		"out_texture0_coord = vTexture0_Coord;"+
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
    "in vec2 out_texture0_coord;"+
    "uniform highp sampler2D u_texture0_sampler;"+
    "out vec4 FragColor;"+
    "void main(void)"+
    "{"+
		"FragColor = texture(u_texture0_sampler, out_texture0_coord);"+
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
    gl.bindAttribLocation(shaderProgramObject,WebGLMacros.VDG_ATTRIBUTE_TEXTURE0,"vTexture0_Coord");

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

    // Load smiley Textures
	smiley_texture = loadGLTexture("smiley.png");
    
    
    // get MVP uniform location
    mvpUniform=gl.getUniformLocation(shaderProgramObject,"u_mvp_matrix");
    // get texture0_sampler uniform location
    uniform_texture0_sampler=gl.getUniformLocation(shaderProgramObject,"u_texture0_sampler");

    // *** vertices, colors, shader attribs, vbo, vao initializations ***
    var quadVertices=new Float32Array([
                                        0.8, 0.8, 0.0,	
		
										-0.8, 0.8, 0.0,
		
										-0.8, -0.8, 0.0,
		
										0.8, -0.8, 0.0
                                         ]);

    var quadTexcoords=new Float32Array([
                                            1.0, 1.0,
											
											0.0, 1.0,
											
											0.0, 0.0,
											
											1.0, 0.0
                                           ]);

    // pyramid code
    vao_quad=gl.createVertexArray();
    gl.bindVertexArray(vao_quad);
    
    vbo_position = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER,vbo_position);
    gl.bufferData(gl.ARRAY_BUFFER,quadVertices,gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_VERTEX,
                           3, // 3 is for X,Y,Z co-ordinates in our quadVertices array
                           gl.FLOAT,
                           false,0,0);
    gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_VERTEX);
    gl.bindBuffer(gl.ARRAY_BUFFER,null);
    
    vbo_texture = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER,vbo_texture);
    gl.bufferData(gl.ARRAY_BUFFER,quadTexcoords,gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_TEXTURE0,
                           2, // 2 is for S and T co-ordinates in our quadTexcoords array
                           gl.FLOAT,
                           false,0,0);
    gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_TEXTURE0);
    gl.bindBuffer(gl.ARRAY_BUFFER,null);
    
    gl.bindVertexArray(null);

    
    // set clear color
    gl.clearColor(0.0, 0.0, 0.0, 1.0); // black
    
    // Depth test will always be enabled
    gl.enable(gl.DEPTH_TEST);
    
    // We will always cull back faces for better performance
    gl.enable(gl.CULL_FACE);
    
    // initialize projection matrix
    perspectiveProjectionMatrix=mat4.create();
}


function loadGLTexture(path)
{
	var texture;
	texture = gl.createTexture();
    texture.image = new Image();
    texture.image.src=path;
    texture.image.onload = function ()
    {
        gl.bindTexture(gl.TEXTURE_2D, texture);
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, texture.image);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
        gl.bindTexture(gl.TEXTURE_2D, null);
    }
	
	return texture;
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
    
    var modelViewMatrix=mat4.create(); // itself creates identity matrix
    var modelViewProjectionMatrix=mat4.create(); // itself creates identity matrix
    
    // Quad
	
    mat4.identity(modelViewMatrix); // reset to identity matrix
    mat4.identity(modelViewProjectionMatrix); // reset to identity matrix
    
    mat4.translate(modelViewMatrix, modelViewMatrix, [0.0,0.0,-3.0]);
    
    mat4.multiply(modelViewProjectionMatrix,perspectiveProjectionMatrix,modelViewMatrix);
    gl.uniformMatrix4fv(mvpUniform,false,modelViewProjectionMatrix);
    
    // bind with texture
    gl.bindTexture(gl.TEXTURE_2D,smiley_texture);
    gl.uniform1i(uniform_texture0_sampler, 0);
    
    gl.bindVertexArray(vao_quad);
    
    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    // actually 2 triangles make 1 cube, so there should be 6 vertices,
    // but as 2 tringles while making square meet each other at diagonal,
    // 2 of 6 vertices are common to both triangles, and hence 6-2=4
    gl.drawArrays(gl.TRIANGLE_FAN,0,4);
    
    gl.bindVertexArray(null);

    gl.useProgram(null);
    
	Update();
    // animation loop
    requestAnimationFrame(draw, canvas);
}

function uninitialize()
{
    // code
    if(smiley_texture)
    {
        gl.deleteTexture(smiley_texture);
        smiley_texture=0;
    }
    
    
    if(vao_quad)
    {
        gl.deleteVertexArray(vao_quad);
        vao_quad=null;
    }
   
    if(vbo_texture)
    {
        gl.deleteBuffer(vbo_texture);
        vbo_texture=null;
    }
    
    if(vbo_position)
    {
        gl.deleteBuffer(vbo_position);
        vbo_position=null;
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


function Update()
{   
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

function degToRad(degrees)
{
    // code
    return(degrees * Math.PI / 180);
}
