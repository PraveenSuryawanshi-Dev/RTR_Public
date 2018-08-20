

#import "MyView.h"

@implementation MyView
{
	NSString *centerText;
}

-(id) initWithFrame:(CGRect)frameRect
{
   
    
	self = [super initWithFrame:frameRect];
	
	if(self)
	{
		//initialize the code here
		
		//set scene's background color		
		[self setBackgroundColor:[UIColor whiteColor]];
		
        centerText=@"Hello World !!!";
		
		//Gesture Recogination

		//Tap gesture code
		 
		UITapGestureRecognizer *singleTapGestureRecognizer= [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
		[singleTapGestureRecognizer setNumberOfTapsRequired:1];
		[singleTapGestureRecognizer setNumberOfTouchesRequired:1];
		[singleTapGestureRecognizer setDelegate:self];
		[self addGestureRecognizer:singleTapGestureRecognizer];
		
		
		UITapGestureRecognizer *doubleTapGestureRecognizer= [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
		[doubleTapGestureRecognizer setNumberOfTapsRequired:2];
		[doubleTapGestureRecognizer setNumberOfTouchesRequired:1];
		[doubleTapGestureRecognizer setDelegate:self];
		[self addGestureRecognizer:doubleTapGestureRecognizer];
		
		
		//this will allow to differentiate between single tap and double tap.
		[singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
		
		
		//swipe gesture
		UISwipeGestureRecognizer *swipeGestureRecognizer = [[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
		[self addGestureRecognizer:swipeGestureRecognizer];
		
		//long-press gesture
		UILongPressGestureRecognizer *longPressGestureRecognizer = [[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
		[self addGestureRecognizer:longPressGestureRecognizer];
		
		}
	return (self);
}



//only override drawRect: if you perform custom drawing.
//an empty impelementation adverserly affects performance during animation.

-(void) drawRect:(CGRect)rect
{
	//black background
	UIColor *fillcolor = [UIColor blackColor];
	[fillcolor set];
	UIRectFill(rect);
	
	
	 // dictionary with kvc
    NSDictionary *dictionaryForTextAttributes=[NSDictionary dictionaryWithObjectsAndKeys:
                                               [UIFont fontWithName:@"Helvetica" size:24], NSFontAttributeName,
                                               [UIColor greenColor], NSForegroundColorAttributeName,
                                               nil];
    
    CGSize textSize=[centerText sizeWithAttributes:dictionaryForTextAttributes];
    
    CGPoint point;
    point.x=(rect.size.width/2)-(textSize.width/2);
    point.y=(rect.size.height/2)-(textSize.height/2)+12;
    
    [centerText drawAtPoint:point withAttributes:dictionaryForTextAttributes];
}

// to become first responder
-(BOOL)acceptsFirstResponder
{
    // code
    return(YES);
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    // code
/*
    centralText = @"'touchesBegan' Event Occured";
    [self setNeedsDisplay]; // repainting
*/
}

-(void)onSingleTap:(UITapGestureRecognizer *)gr
{
    // code
    centerText = @"'onSingleTap' Event Occured";
    [self setNeedsDisplay]; // repainting
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr
{
    // code
    printf("double tap");
    centerText = @"'onDoubleTap' Event Occured";
    [self setNeedsDisplay]; // repainting
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr
{
    // code
    [self release];
    exit(0);
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gr
{
    // code
    centerText = @"'onLongPress' Event Occured";
    [self setNeedsDisplay]; // repainting
}

- (void)dealloc
{
    // code
    [super dealloc];
}

@end
