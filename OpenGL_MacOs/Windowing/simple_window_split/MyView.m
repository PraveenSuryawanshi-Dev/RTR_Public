#include "MyView.h"

@implementation MyView
{
	NSString *centralText;
}

-(id) initWithFrame:(NSRect) frame;
{
	self = [super initWithFrame:frame];

	if(self)
	{
		[[self window]setContentView:self];

		centralText =@"Hellow World !!!";
	}

	return (self);
}

-(void) drawRect:(NSRect) dirtyRect
{
	//code

	//black background

	NSColor *fillColor = [NSColor blackColor];

	[fillColor set];

	NSRectFill(dirtyRect);

	//dictionary with KVC

	 NSDictionary *dictionaryForTextAttributes=[NSDictionary dictionaryWithObjectsAndKeys:
                                               [NSFont fontWithName:@"Helvetica" size:32], NSFontAttributeName,
                                               [NSColor greenColor], NSForegroundColorAttributeName,
                                               nil];

    NSSize textSize=[centralText sizeWithAttributes:dictionaryForTextAttributes];
    
    NSPoint point;
    point.x=(dirtyRect.size.width/2)-(textSize.width/2);
    point.y=(dirtyRect.size.height/2)-(textSize.height/2)+12;

    [centralText drawAtPoint:point withAttributes:dictionaryForTextAttributes];
}


-(BOOL)acceptsFirstResponder
{
    // code
    [[self window]makeFirstResponder:self];
    return(YES);
}


-(void)keyDown:(NSEvent *)theEvent
{
    // code
    int key=(int)[[theEvent characters]characterAtIndex:0];
    switch(key)
    {
        case 27: // Esc key
            [ self release];
            [NSApp terminate:self];
            break;
        case 'F':
        case 'f':
            centralText=@"'F' or 'f' Key Is Pressed";
            [[self window]toggleFullScreen:self]; // repainting occurs automatically
            break;
        default:
            break;
    }
}



-(void)mouseDown:(NSEvent *)theEvent
{
    // code
    centralText=@"Left Mouse Button Is Clicked";
    [self setNeedsDisplay:YES]; // repainting
}


-(void)mouseDragged:(NSEvent *)theEvent
{
    // code
}


-(void)rightMouseDown:(NSEvent *)theEvent
{
    // code
    centralText=@"Right Mouse Button Is Clicked";
    [self setNeedsDisplay:YES]; // repainting
}


- (void) dealloc
{
    // code
    [super dealloc];
}
@end

