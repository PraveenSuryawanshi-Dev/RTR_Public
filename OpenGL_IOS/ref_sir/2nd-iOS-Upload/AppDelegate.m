//
//  AppDelegate.m
//  Window-BlueColor
//
//  Created by ASTROMEDICOMP on 30/05/18.
//

#import "AppDelegate.h"

#import "ViewController.h"

#import "GLESView.h"

@implementation AppDelegate
{
@private
    UIWindow *mainWindow;
    ViewController *mainViewController;
    GLESView *myView;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // get screen bounds for fullscreen
    CGRect screenBounds=[[UIScreen mainScreen]bounds];
    
    // initialize window variable corresponding to screen bounds
    mainWindow=[[UIWindow alloc]initWithFrame:screenBounds];
    
    mainViewController=[[ViewController alloc]init];
    
    [mainWindow setRootViewController:mainViewController];
    
    // initialize view variable corresponding to screen bounds
    myView=[[GLESView alloc]initWithFrame:screenBounds];
    
    [mainViewController setView:myView];
    
    [myView release];
    
    // add the ViewController's view as subview to the window
    [mainWindow addSubview:[mainViewController view]];
    
    // make window key window and visible
    [mainWindow makeKeyAndVisible];
    
    [myView startAnimation];
    
    return(YES);
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // code
    [myView stopAnimation];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{

}

- (void)applicationWillEnterForeground:(UIApplication *)application
{

}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // code
    [myView startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // code
    [myView stopAnimation];
}

- (void)dealloc
{
    // code
    [myView release];
    
    [mainViewController release];
    
    [mainWindow release];
    
    [super dealloc];
}

@end
