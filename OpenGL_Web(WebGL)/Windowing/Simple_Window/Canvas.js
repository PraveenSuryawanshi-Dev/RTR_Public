//ONLOAD FUNCTION

function main()
{
	//STEP 1:
	//GET THE CANVAS ELEMENT FROM HTML 5
	var canvas = document.getElementById("Canvas");
	if(!canvas)
		console.log("Obtaining Canvas failed\n");
	else
		console.log("Obtaining Canvas succeeded\n");
	
	//PRINT THE CANVAS WIDTH AND HEIGHT.
	console.log("Canvas Width : "+canvas.width+"	Canvas Height :"+canvas.height+"\n");
	
	
	//STEP 2:
	//GET THE 2D CONTEXT FROM CANVAS.
	var context = canvas.getContext("2d");
	if(!context)
		console.log("Obtaining Context failed\n");
	else
		console.log("Obtaining Context succeeded\n");
	
	
	//STEP 3:
	//SET THE BACKGROUND COLOR TO BLACK
	context.fillStyle ="black";
	context.fillRect(0,0,canvas.width,canvas.height);
	//
	
	
	
	//STEP 4:
	//SET THE TEXT
	
	//CENTER HORIZONTALLY
	context.textAlign="center";
	//CENTER VERTICALLY
	context.textBaseline="middle";
	

	//SET FONT TO TEXT
	context.font ="48px sans-serif";
	//SET THE FOREGROUND COLOR OF TEXT
	context.fillStyle="white";
	
	
	//DISPLAY THE TEXT AT CENTER OF SCREEN.
	var message = "My First Canvas Window !!!";
	context.fillText(message,canvas.width/2,canvas.height/2);
	//
}