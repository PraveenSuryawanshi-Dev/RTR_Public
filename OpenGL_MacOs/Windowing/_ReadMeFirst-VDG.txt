01) After booting macOS, when you 'login' you will see the macOS desktop.

02) The top menubar should be showing 'Finder' application contextual menubar.
    'Finder' is macOS Explorer ( in Windows ) or 'File Manager' (in Linux).

03) In this 'Finder' menubar, select 'File > New Finder Window' 

04) The 'Finder' window should appear with your 'home' or ‘User’ like 
    directory contents.

05) Inside it Create a directory by right click and choose 'New Folder' 
    and name this new folder say MyOpenGL.

06) Copy the downloaded archive to this directory and extract it.

07) This will create a subdirectory holding your extracted files.
    Rename this ‘Archive’ subdirectory to some meaningful name.

    There are 3 files one is ‘Compile.sh’ second is ‘Window.m’ and
    third is the one that you are currently reading.

08) If <MacHD> icon ( Just like ‘My Computer’ icon in Windows ) is not 
    already visible on desktop, Go to top menubar and select 
    Finder > Preferences.
    
    In ‘Finder Preferences’ window, select ‘General’ tab and in it
    check ‘Hard disks’ checkbox. 

    In same above window, now select ‘Advanced’ tab and check
    ’Show all filename extensions’ checkbox.

    You will see some icons on desktop. On of which will be <MacHD>

09) Navigate to <MacHD>/Applications/Utilities and double click 
    Terminal.app to open Terminal window.

    This is just like ‘command prompt’ or ‘cmd’ in Windows or
    terminal/console in Linux.

10) 'cd' to the directory where you extracted the zip in step 7

11) Run the script by :
    $ sh Compile.sh

12) An Application Package will be created with default icon on it.
    This will have .app Extension. By default extensions are not visible. 
    You are seeing them because of the setting you made in step 8.

    If default application icon is not visible or if some ‘error like’ icon 
    is visible, just refresh it by closing and reopening this ‘Finder' window
    or by clicking ‘<‘ and ‘>’ buttons at left-top of this Finder window.

13) Double click on this ‘Application Package’ and you will see our
    first window with green ’Hello World !!!’ text on black background.

14) Press F/f key to toggle fullscreen.
    Press Left and Right mouse button to see relevant messages on screen.

15) Press ‘Esc’ key to exit.
