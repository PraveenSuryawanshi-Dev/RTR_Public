//ONLOAD Function

function main()
{
	//STEP 1:
	//GET THE CANVAS FROM HTML 5 ELEMENT
	var canvas = document.getElementById("Canvas");
	if(!canvas)
		console.log("Obtaining Canvas Failed !!!\n");
	else
		console.log("Obtaining Canvas Succeeded. \n");
	
	//PRINT THE CANVAS WIDTH AND HEIGHT
	console.log("Canvas width :"+canvas.width+"	Canvas height :"+canvas.height+"\n");
	
	
	
	//STEP 2:
	//GET THE 2D CONTEXT FROM CANVAS
	var context = canvas.getContext("2d");
	if(!context)
		console.log("Obtaining 2D Context Failed !!!\n");
	else
		console.log("Obtaining 2D Context Succeeded.\n");
	
	//STEP 3:
	//SET THE BACKGROUNF COLOR TO BLACK
	context.fillStyle="black";
	context.fillRect(0,0,canvas.width,canvas.height);
	
	
	//STEP 4:
	//DISPLAY TEXT AT THE CENTER AT SCREEN
	
	//SET THE TEXT AT CENTER OF SCREEN.
	
	//CENTER HORIZONTALLY
	context.textAlign="center";
	//CENTER VERTICALLY
	context.textBaseline ="middle";
	
	//SET TEXT FOREGROUND COLOR TO WHITE
	context.fillStyle ="white";
	//SET THE FONT OF TEXT
	context.font="48px sans-serif";

	
	var message ="Event Handling Window...";
	context.fillText(message,canvas.width/2 , canvas.height/2);
	
	
	//SETP 5:
	//ADDD EVENT LISTNER TO CANVAS
	
	window.addEventListener("keydown",OnKeyDown,false);
	window.addEventListener("click",OnClick,false);
}


function OnKeyDown(event)
{
	alert("YOU PRESSED THE KEY");
}



function OnClick()
{
	alert("YOU CLICK THE MOUSE");
}