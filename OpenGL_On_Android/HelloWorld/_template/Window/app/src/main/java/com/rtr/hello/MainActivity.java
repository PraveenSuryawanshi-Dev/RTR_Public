package com.rtr.hello;

import android.app.Activity;
import android.os.Bundle;

import android.view.Window; /*WINDOW CLASS*/
import android.view.WindowManager; /*NEED FOR MAKING APPLICATION FULLSCREEN*/
import android.graphics.Color; /*NEED FOR COLOR THE TEXT*/
import android.widget.TextView; /*NEED FOR CREATE TEXT LABEL*/
import android.view.Gravity; /*NEED FOR SETTING TEXT TO CENTER*/
import android.content.pm.ActivityInfo; /*NEED FOR LANDSCAPE */

public class MainActivity extends Activity {

    @Override

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        /*setContentView(R.layout.activity_main);*/ /*REMOVE*/
		
		/*REMOVE THE ACTION BAR*/
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		
		/*MAKE THE APP FULLSCREEN*/
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		
		/*MAKE THE APPLICATION LANDSCAPE*/
		MainActivity.this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		
		/*SET BACKGROUND COLOR TO BLACK*/
		this.getWindow().getDecorView().setBackgroundColor(Color.BLACK);
		
		/*CREATE THE LABEL OBJECT*/
		TextView sayHelloWorld = new TextView(this);
		
		//SET THE TEXT COLOR TO GREEN
		sayHelloWorld.setTextColor(Color.rgb(0,255,0));
	  
		//SET TEXT TO HELLO WORLD !!!
		sayHelloWorld.setText("Hellow World !!!");
		
		//SET SIZE OF TEXT
		sayHelloWorld.setTextSize(60);

		//SET THE LABEL TO CENTER OF SCREEN
		sayHelloWorld.setGravity(Gravity.CENTER);
		
		/*SET CONTENTS*/
		setContentView(sayHelloWorld);	
				
    }
	
	
	@Override
	
	protected void onPause()
	{
		super.onPause();
	}
	
	@Override
	
	protected void onResume()
	{
		super.onResume();
	}
}

