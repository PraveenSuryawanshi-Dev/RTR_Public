//ONLOAD FUNCTION

//DECLARE GLOBAL VARIABLES
var canvas = null;
var context = null;

function main()
{
	//STEP 1:
	//GET THE CANVAS FROM HTML 5 ELEMENT
	canvas = document.getElementById("Canvas");
	if(!canvas)
		console.log("Obtaining Canvas Failed !!!... \n");
	else
		console.log("Obtaining Canvas Succeeded ...\n");

	//PRINT THE CANVAS WIDTH AND HEIGHT.
	console.log("Canvas WIDTH:"+canvas.width+" Canvas HEIGHT:"+canvas.height+" \n");
	
	//STEP 2:
	//GET THE 2D CONTEXT FROM CANVAS.
	context = canvas.getContext("2d");
	if(!context)
		console.log("Obtaining 2D Context Failed !!!... \n");
	else
		console.log("Obtaining 2D Context Succeeded...\n");
	
	
	//STEP 3:
	//SET THE BACKGROUND COLOR TO BLACK.
	context.fillStyle = "black";
	context.fillRect(0,0,canvas.width,canvas.height);
	
	
	//STEP 4:
	//DRAW TEXT AT THE CENTER OF SCREEN.
	DrawText("Toggle Screen..");
	
	//STEP 5:
	//REGISTER KEYBOARD AND MOUSE EVENT LISTENER
	window.addEventListener("keydown",OnKeyDown,false);
	window.addEventListener("click",OnClick,false);
}


function DrawText(msg)
{
	context.font ="48px sans-serif";
	context.textAlign ="center";
	context.textBaseline="middle";
	context.fillStyle="white";
	context.fillText(msg,canvas.width/2,canvas.height/2);
}


function OnKeyDown(event)
{
	switch(event.keyCode)
	{
		case 70: // for 'F' or 'f'
				console.log("F Key Pressed \n");
				ToggleFullScreen();
				//Repaint
				DrawText("Toggle Screen..");
				break;
	}
}

function OnClick()
{
	//code
}

function ToggleFullScreen()
{
var fullscreen_element = document.fullscreenElement || document.webkitFullscreenElement ||
						document.mozFullScreenElement || document.msFullscreenElement || null;
						
						
	//if not full screen
	if(fullscreen_element == null)
	{
		if(canvas.requestFullscreen)
			canvas.requestFullScreen();
		else 
		if(canvas.mozRequestFullScreen)
			canvas.mozRequestFullScreen();
		else
		if(canvas.webkitRequestFullScreen)
			canvas.webkitRequestFullScreen();
		else
		if(canvas.msRequestFullScreen)
			canvas.msRequestFullScreen();
	}
	else
	{
		if(document.exitFullscreen)
			document.exitFullscreen();
		else
		if(document.mozCancelFullScreen)
			document.mozCancelFullScreen();
		else
		if(document.webkitExitFullscreen)
			document.webkitExitFullscreen();
		else
		if(document.msExitFullscreen)
			document.msExitFullscreen();
		
	}
	
}