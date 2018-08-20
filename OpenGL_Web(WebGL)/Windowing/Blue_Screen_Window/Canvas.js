var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_orignal_width;
var canvas_orignal_height;


//To start animation : To have requestAnimationFrame() to be called "cross-browser" support

var requestAnimationFrame = window.requestAnimationFrame ||
							window.webkitRequestAnimationFrame ||
							window.mozRequestAnimationFrame ||
							window.oRequestAnimationFrame ||
							window.msRequestAnimationFrame;
							
							
							


function main()
{
	canvas = document.getElementById("Canvas");
	
	if(!canvas)
		console.log("Obtaining Canvas Failed\n");
	else
		console.log("Obtaining Canvas Succeed\n");
	
	canvas_orignal_width = canvas.width;
	canvas_orignal_height=canvas.height;
	
	window.addEventListener("keydown",keydown,false);
	window.addEventListener("click",mouseDown,false);
	window.addEventListener("resize",resize,false);
	
	init();
	
	//starting drawing here as warming-up;
	resize();
	draw();
}


function toggleFullScreen()
{
	console.log("toggleFullScreen");
	var fullscreen_element = document.fullscreenElement ||
							 document.webkitFullscreenElement ||
							 document.mozFullScreenElement ||
							 document.msFullscreenElement ||
							 null;
							 
							 
	if(fullscreen_element == null)
	{console.log("f if");
		if(canvas.requestFullscreen)
			canvas.requestFullscreen();
		else if(canvas.mozRequestFullScreen)
			canvas.mozRequestFullScreen();
		else if(canvas.webkitRequestFullscreen)
			canvas.webkitRequestFullscreen();
		else if(canvas.msRequestFullscreen)
			canvas.msRequestFullscreen();
		
		bFullscreen = true;
	}
	else
	{console.log("f else");
		if(document.exitFullscreen)
			document.exitFullscreen();
		
		else if(document.mozCancelFullScreen)
			document.mozCancelFullScreen();
		
		else if(document.webkitExitFullscreen)
			document.webkitExitFullscreen();
		
		else if(document.msExitFullscreen)
			document.msExitFullscreen();
		
		bFullscreen =false;
	}
}



function init()
{
	
	gl = canvas.getContext("webgl2");
	if(gl==null)
	{
		console.log("failed to get the rendering context for WEBGL");
		return;
	}
	
	
	gl.viewportWidth = canvas.width;
	gl.viewportHeight =canvas.height;
	
	gl.clearColor(0.0,0.0,1.0,1.0); //blue
}


function resize()
{
	if(bFullscreen == true)
	{
		canvas.width =window.innerWidth;
		canvas.height= window.innerHeight;
	}
	else
	{
		canvas.width = canvas_orignal_width;
		canvas.height =canvas_orignal_height;
	}
	
	gl.viewport(0,0,canvas.width,canvas.height);
}


function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT);
	
	requestAnimationFrame(draw,canvas);
}

function keydown(event)
{
	switch(event.keyCode)
	{
		case 70: //'f' or 'F'
		toggleFullScreen();
		break;
	}
}

function mouseDown()
{
	
}

