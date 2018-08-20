mkdir -p Window.app/Contents/MacOS

Clang -o Window.app/Contents/MacOS/Window AppDelegate.m MyView.m Window.m -framework Cocoa
