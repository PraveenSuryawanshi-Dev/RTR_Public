package com.rtr.universe;

import android.widget.TextView; /*NEED FOR CREATE TEXT LABEL*/
import android.graphics.Color; /*NEED FOR COLOR THE TEXT*/
import android.view.Gravity; /*NEED FOR SETTING TEXT TO CENTER*/
import android.content.Context; /*USED TO TYPECAST WINDOW OBJECT*/


public class MyTextView extends TextView
{
	MyTextView(Context context)
	{
		super(context);
		
		//SET THE TEXT COLOR TO GREEN
		setTextColor(Color.rgb(0,255,0));
	  
		//SET TEXT TO HELLO WORLD !!!
		setText("Hellow Universe !!!");
		
		//SET SIZE OF TEXT
		setTextSize(60);

		//SET THE LABEL TO CENTER OF SCREEN
		setGravity(Gravity.CENTER);
		
	}
	
}
