#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>


using namespace std;


bool gbFullScreen = false;

Display *gpDisplay =NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;

int giWindowWidth = 800;
int giWindowHeight = 600;

int main(void)
{

/*
FUNCTION PROTOTYPES
*/
void CreateWindow(void);
void ToggleFullScreen(void);
void UnInitialize(void);
/**/


int winWidth = giWindowWidth;
int winHeight = giWindowHeight;


/*INITIALIZE WINDOW*/
CreateWindow();
/**/


/*MESSAGE LOOP*/

XEvent event;
KeySym keysym;

while(1)
{
 XNextEvent(gpDisplay,&event);	
	switch(event.type)
	{
		case MapNotify:
		     break;

		case KeyPress:
		     keysym = XkbKeycodeToKeysym(gpDisplay , event.xkey.keycode,0,0);
		     switch(keysym)
		     {
			case XK_Escape:
			     UnInitialize();
			     exit(0);

			case XK_F:
			case XK_f:
				 if(gbFullScreen == false)
				   {
					ToggleFullScreen();
					gbFullScreen = true;
				   }
				 else
				   {
					ToggleFullScreen();
					gbFullScreen =false;
				   }
				 break;

			 default:
				 break;
		     }

		case ButtonPress:
		     switch(event.xbutton.button)
		     {
			case 1:
				break;
			
			case 2:
				break;

			case 3:
				break;
			default:
				break;
		     }
		     break; 

		case MotionNotify:
			break;

		case ConfigureNotify:
		     winWidth = event.xconfigure.width;
		     winHeight = event.xconfigure.height;
		     break;

		case Expose:
		     break;

		case DestroyNotify:
		     break;

		case 33:
		     UnInitialize();
		     exit(0);		
		     break;
	
		default:
		      break;
	}

}

/**/


UnInitialize();
return (0);
}



void CreateWindow(void)
{
/*FUNCTION PROTO TYPES*/
void UnInitialize(void);
/**/


XSetWindowAttributes winAttribs;
int defaultScreen;
int defaultDepth;
int styleMask;


gpDisplay = XOpenDisplay(NULL);
if(gpDisplay == NULL )
{
	printf("ERROR : Unable to open X Display. \nExitting Now...\n");
	UnInitialize();
	exit(0);
}


defaultScreen = XDefaultScreen(gpDisplay);

defaultDepth = DefaultDepth(gpDisplay,defaultScreen);

gpXVisualInfo = (XVisualInfo *) malloc(sizeof(XVisualInfo));
if(gpXVisualInfo == NULL)
{
	printf("ERROR : Unable to Allocate Memory for Visual Info. \nExitting Now... \n");
	UnInitialize();
	exit(0);
}


XMatchVisualInfo(gpDisplay, defaultScreen ,defaultDepth, TrueColor, gpXVisualInfo);
if(gpXVisualInfo == NULL)
{
	printf("ERROR : Unable to Open X Display. \nExitting Now... \n");
	UnInitialize();
	exit(0);

}


winAttribs.border_pixel = 0;
winAttribs.background_pixmap= 0;
winAttribs.colormap = XCreateColormap(gpDisplay, RootWindow(gpDisplay , gpXVisualInfo->screen),gpXVisualInfo->visual, AllocNone);

gColormap = winAttribs.colormap;

winAttribs.background_pixel= BlackPixel(gpDisplay,defaultScreen);
winAttribs.event_mask = ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;


styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;

gWindow = XCreateWindow(gpDisplay,
			RootWindow(gpDisplay, gpXVisualInfo->screen),
			0,
			0,
			giWindowWidth,
			giWindowHeight,
			0,
			gpXVisualInfo->depth,
			InputOutput,
			gpXVisualInfo->visual,
			styleMask,
			&winAttribs);
if(!gWindow)
{
	printf("ERROR :Failed To Create Main Window. \nExitting Now... \n");
	UnInitialize();
	exit(0);

}

XStoreName(gpDisplay,gWindow,"FIRST SIMPLE XWINDOW With FullScreen");

Atom windowManagerDelete = XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);
XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);

XMapWindow(gpDisplay, gWindow);
}


void ToggleFullScreen(void)
{

Atom wm_state;
Atom fullscreen;
XEvent xev = {0};


wm_state = XInternAtom(gpDisplay,"_NET_WM_STATE",False);
memset(&xev,0,sizeof(xev));

xev.type = ClientMessage;
xev.xclient.window = gWindow;
xev.xclient.message_type = wm_state;
xev.xclient.format=32;
xev.xclient.data.l[0] = gbFullScreen ? 0 : 1;


fullscreen = XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);

xev.xclient.data.l[1] = fullscreen;

XSendEvent(gpDisplay,
	   RootWindow(gpDisplay,gpXVisualInfo->screen),
	   False,
	   StructureNotifyMask,
	   &xev);

}



void UnInitialize()
{

if(gWindow)
{
	XDestroyWindow(gpDisplay,gWindow);
}

if(gColormap)
{
	XFreeColormap(gpDisplay,gColormap);
}


if(gpXVisualInfo)
{
	free(gpXVisualInfo);
	gpXVisualInfo = NULL;
}


if(gpDisplay)
{
	XCloseDisplay(gpDisplay);
	gpDisplay = NULL;
}

}
