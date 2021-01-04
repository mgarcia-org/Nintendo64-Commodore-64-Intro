#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

// Based on libdragon test.c example
// c64 loading screen by mgarcia
// A ported of my Net Yaroze C64 demo, https://github.com/mgarcia-org/Net-Yaroze-Commodore-64-Intro/blob/master/C64Intro.c

static resolution_t res = RESOLUTION_512x480; //RESOLUTION_640x480; //RESOLUTION_320x240;
static bitdepth_t bit = DEPTH_32_BPP;
#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 480


//#define EMULATION
//#define DEBUG




#ifdef EMULATION

#define INC_TYPING 15
#define INC_SPEED 15
#define INC_LOADING 25

#else

#define INC_TYPING 1
#define INC_SPEED 1
#define INC_LOADING 1

#endif


//from Net Yaroze
typedef struct {
	unsigned long attribute;
	unsigned short   x, y;
	unsigned short w, h;
	unsigned char r, g, b;
} GsBOXF;



#define setRGB( xline, xr, xg, xb)  (xline).r = (xr); (xline).b = (xb); (xline).g = (xg)
#define C64LINES 20

#define TYPESPEED 25 // must be divisable by INC_TYPING

/*
   from http://www.godot64.de/german/epalet.htm
C64
Color    Red    Green    Blue
black    0x00    0x00    0x00    1
white    0xff    0xff    0xff    2
red    0x88    0x00    0x00    3
cyan    0xaa    0xff    0xee    4
purple    0xcc    0x44    0xcc    5
green    0x00    0xcc    0x55    6
blue    0x00    0x00    0xaa    7
yellow    0xee    0xee    0x77    8
orange    0xdd    0x88    0x55    9
brown    0x66    0x44    0x00    10
l,red    0xff    0x77    0x77    11
d,gray    0x33    0x33    0x33    12
gray    0x77    0x77    0x77    13
lgreen    0xaa    0xff    0x66    14
l,blue    0x00    0x88    0xff    15
l,gray    0xbb    0xbb    0xbb    16
 */



unsigned char currentBuffer;

GsBOXF      myLines[C64LINES];
GsBOXF      fadeBox;

GsBOXF      c64InBox;
GsBOXF      c64OutBox;
int typingCounter;
int loadCounter;
int dispCounter;
char  loadString[SCREEN_HEIGHT*SCREEN_HEIGHT];


void initSystem(void);
void display(void);
void initBoxes(void);
void drawBoxes(display_context_t disp);

void c64Screen(display_context_t disp);

char tStr[256];


int main(void)
{
	/* enable interrupts (on the CPU) */
	init_interrupts();

	/* Initialize peripherals */
	display_init( res, bit, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );
	dfs_init( DFS_DEFAULT_LOCATION );

	//console_init();

	initBoxes();
	typingCounter = 0;
	loadCounter = 0;
	dispCounter = 0;
	srand(rand());




	/* Main loop test */
	while(1) 
	{


		static display_context_t disp = 0;

		/* Grab a render buffer */
		while( !(disp = display_lock()) );

		/*Fill the screen */
		graphics_fill_screen( disp, 0 );

#if 0

		sprintf(tStr, "Video mode: %lu\n", *((uint32_t *)0x80000300));
		graphics_draw_text( disp, 20, 20, tStr );
		sprintf(tStr, "Status: %08lX\n", *((uint32_t *)0xa4400000));
		graphics_draw_text( disp, 20, 30, tStr );

		/* Full bright color */
		graphics_draw_box(disp, 20, 40, 20, 20, graphics_make_color(255, 0, 0, 255));
		graphics_draw_box(disp, 50, 40, 20, 20, graphics_make_color(0, 255, 0, 255));
		graphics_draw_box(disp, 80, 40, 20, 20, graphics_make_color(0, 0, 255, 255));
		graphics_draw_box(disp, 110, 40, 20, 20, graphics_make_color(255, 255, 255, 255));

#endif



		//not static, recreat string every frame
		// char loadImg[] = "Nintendo 64  (Dec 2017)     .::::.\nCommodore 64 Intro        .::::::::.\nBy Mike Garcia mgarcia.org:::::::::::\n                          ':::::::::::..\n                           :::::::::::::::'\n                            ':::::::::::.\n                              .::::::::::::'\n                            .:::::::::::...\n                           ::::::::::::::'\n               .:::.       '::::::::''::::,\n             .::::::::.      ':::::'  ':::,\n            .::::':::::::.    :::::    ':::,\n          .:::::' ':::::::::. :::::      '::\n        .:::::'     ':::::::::.:::::      ':\n      .::::''         '::::::::::::::     \n     .::''              '::::::::::::     \n  ..::::                  ':::::::::'       \n..''''':'                    ':::::.'\nASCII Art by LGB from: chris.com";
		char loadImg[] = "\n\n\n\n\n\nNintendo 64                 .::::.\nCommodore 64 Intro        .::::::::.\n                          :::::::::::\n                          ':::::::::::..\n                           :::::::::::::::'\n                            ':::::::::::.\n                              .::::::::::::'\n                            .:::::::::::...\n                           ::::::::::::::'\n               .:::.       '::::::::''::::,\n             .::::::::.      ':::::'  ':::,\n            .::::':::::::.    :::::    ':::,\n          .:::::' ':::::::::. :::::      '::\n        .:::::'     ':::::::::.:::::      ':\n      .::::''         '::::::::::::::     \n     .::''              '::::::::::::     \n  ..::::                  ':::::::::'       \n..''''':'                    ':::::.'\n\nASCII Art by LGB from: chris.com\n\n\nHacked by Mike Garcia (mgarcia.org) \nDec 2017\n\nShout out to #N64DEV@efnet!! :D";


		//debug("loadCounter %d\n",loadCounter);

		if(loadCounter < 200)  //fade and load screen
		{
			c64Screen(disp);

		}
		else if(loadCounter < 300)
		{
			// drawBoxes();
			graphics_draw_box(disp, c64OutBox.x, c64OutBox.y, c64OutBox.w, c64OutBox.h, graphics_make_color(c64OutBox.r, c64OutBox.g, c64OutBox.b, 255));
			// GsSortBoxFill(&c64OutBox,&OT[currentBuffer],   4);



		}
		else if(loadCounter < 500)  //loading screen
		{
			c64Screen(disp);
			// printf("  OK\n\n  SEARCHING\n  FOUND PSX GAME!");
			graphics_draw_text( disp, 72, 170, " OK\n\n SEARCHING\n\n FOUND N64_C64_INTRO");

		}
		else if(loadCounter < 850) 
		{
			drawBoxes(disp);


		} 
		else if(loadCounter < 1000)
		{
			//GsSortBoxFill(&c64OutBox,&OT[currentBuffer],   4);
			graphics_draw_box(disp, c64OutBox.x, c64OutBox.y, c64OutBox.w, c64OutBox.h, graphics_make_color(c64OutBox.r, c64OutBox.g, c64OutBox.b, 155));			
		}
		else if(loadCounter < 3200)
		{
			drawBoxes(disp);

			// GsSortBoxFill(&c64InBox,&OT[currentBuffer],   4);
			setRGB( c64InBox, 0, 0, 0);
			graphics_draw_box(disp, c64InBox.x, c64InBox.y, c64InBox.w, c64InBox.h, graphics_make_color(c64InBox.r, c64InBox.g, c64InBox.b, 155));

			loadCounter+=INC_LOADING;



#if INC_TYPING == 1 //normal 

			if(typingCounter % 2 == 0)
			{
				dispCounter++;



			}
			typingCounter++;
#else
			dispCounter+=INC_LOADING;
#endif
			loadImg[dispCounter]=0;	//set NULL to new position


			// FntPrint(loadImg);
			graphics_draw_text( disp, 80, 90, loadImg );


		}
		else if(loadCounter < 3400)
		{
			drawBoxes(disp);

		}
		else if(loadCounter < 3450)
		{

//blank screen before reset

		}
		else // restart
		{
			loadCounter =0;
			dispCounter= 0;
			typingCounter=0;
			strcpy(loadString,"  ");
			loadString[0]=0;
			setRGB( c64InBox, 53, 53, 192); // back to blue
		}


		
		loadCounter+=INC_LOADING;
		
#ifdef DEBUG
		sprintf(tStr, "loadCounter: %d dispCounter: %d typingCounter: %d", loadCounter, dispCounter, typingCounter);
		graphics_draw_text( disp, 0, 0, tStr );
#endif

		display_show(disp);

	}

}






void c64Screen(display_context_t disp)
{

	//debug("typingCounter = %u\n", typingCounter);
	// GsSortBoxFill(&c64OutBox,&OT[currentBuffer],   4);
	//GsSortBoxFill(&c64InBox,&OT[currentBuffer],   1);

	graphics_draw_box(disp, c64OutBox.x, c64OutBox.y, c64OutBox.w, c64OutBox.h, graphics_make_color(c64OutBox.r, c64OutBox.g, c64OutBox.b, 155));
	graphics_draw_box(disp, c64InBox.x, c64InBox.y, c64InBox.w, c64InBox.h, graphics_make_color(c64InBox.r, c64InBox.g, c64InBox.b, 155));


	//To change the colour of FntPrint's output, insert the sequence "~crgb"
	//in your string, where r, g and b are colour values from '0' to '9'.
	graphics_draw_text( disp, 0, 90, "\t\t\t **** COMMODORE 64 BASIC V2 ****");
	graphics_draw_text( disp, 0, 100, "\t\t    64k RAM SYSTEM 38911 BASIC BYTES FREE");
	graphics_draw_text( disp, 0, 130, "\t\tREADY. ");


	//debug("string = %s\n",loadString);


	if(typingCounter == TYPESPEED)
	{

		strcpy(loadString," ");
		strcat(loadString,"L");


	}else if(typingCounter == TYPESPEED*2)
	{


		strcat(loadString,"O");


	}else if(typingCounter == TYPESPEED*3)
	{


		strcat(loadString,"A");


	}else if(typingCounter == TYPESPEED*4)
	{


		strcat(loadString,"D");

	}
	else if(typingCounter == TYPESPEED*5)
	{


		strcat(loadString,"\n\n");


	}
	else if(typingCounter == TYPESPEED*6)
	{

		strcat(loadString," PRESS PLAY ON TAPE");

	}

	typingCounter+= INC_TYPING;


	graphics_draw_text( disp, 72, 140, loadString);
	//debug("string = %s\n",loadString);


}



void initBoxes(void)
{
	int i;
	fadeBox.w=SCREEN_WIDTH+1;
	fadeBox.h=SCREEN_HEIGHT;
	fadeBox.x=0;
	fadeBox.y=0;
	fadeBox.attribute = 0;
	//    fadeBox.attribute += (1<<30) + (2<<28);
	//  setRGB( fadeBox, 0x0, 0x0, 0x0);


	c64OutBox.w=SCREEN_WIDTH;
	c64OutBox.h=SCREEN_HEIGHT;
	c64OutBox.x=0;
	c64OutBox.y=0;
	c64OutBox.attribute = 0;
	setRGB( c64OutBox, 136, 136, 214);


	c64InBox.w=SCREEN_WIDTH*.70;
	c64InBox.h=SCREEN_HEIGHT*.70;
	c64InBox.x=(SCREEN_WIDTH-c64InBox.w)/2;
	c64InBox.y=(SCREEN_HEIGHT-c64InBox.h)/2;
	c64InBox.attribute = 0;
	setRGB( c64InBox, 53, 53, 192);



	myLines[0].w=SCREEN_WIDTH+1;
	myLines[0].h=SCREEN_HEIGHT/C64LINES;
	myLines[0].x=0;
	myLines[0].y=0;

	for(i=1; i<C64LINES; i++)
	{
		myLines[i]=myLines[0];
		myLines[i].y=i*myLines[0].h;
	}
}




void drawBoxes(display_context_t disp)
{
	int i;



	for(i=0; i<C64LINES; i++)
	{


		switch(rand()%16)
		{


		// white    0xff    0xff    0xff    2
		case 1:
			setRGB(myLines[i], 0xff, 0xff, 0xff);
			break;

			// red    0x88    0x00    0x00    3
		case 2:
			setRGB(myLines[i], 0x88,0,0);
			break;

			// cyan    0xaa    0xff    0xee    4
		case 3:
			setRGB(myLines[i], 0xaa,0xff,0xee);
			break;

			// purple    0xcc    0x44    0xcc    5
		case 4:
			setRGB(myLines[i], 0xcc,0x44,0xcc);
			break;

			// green    0x00    0xcc    0x55    6
		case 5:
			setRGB(myLines[i], 0,0xcc,0x55);
			break;

			// blue    0x00    0x00    0xaa    7
		case 6:
			setRGB(myLines[i], 0,0,0xaa);
			break;

			// yellow    0xee    0xee    0x77    8
		case 7:
			setRGB(myLines[i], 0xee,0xee,0x77);
			break;

			// orange    0xdd    0x88    0x55    9
		case 8:
			setRGB(myLines[i], 0xdd,0x88,0x55);
			break;

			// brown    0x66    0x44    0x00    10
		case 9:
			setRGB(myLines[i], 0x66,0x44,0);
			break;

			// l,red    0xff    0x77    0x77    11
		case 10:
			setRGB(myLines[i], 0xff,0x77,0x77);
			break;

			// d,gray    0x33    0x33    0x33    12
		case 11:
			setRGB(myLines[i], 0x33,0x33,0x33);
			break;

			// gray    0x77    0x77    0x77    13
		case 12:
			setRGB(myLines[i], 0x77,0x77,0x77);
			break;

			// lgreen    0xaa    0xff    0x66    14
		case 13:
			setRGB(myLines[i], 0xaa,0xff,0x66);
			break;

			// l,blue    0x00    0x88    0xff    15
		case 14:
			setRGB(myLines[i], 0,0x88,0xff);
			break;

			// l,gray    0xbb    0xbb    0xbb    16
		case 15:
			setRGB(myLines[i], 0xbb,0xbb,0xbb);
			break;


			// black    0x00    0x00    0x00    1
		default:
			setRGB( myLines[i], 0x0, 0x0, 0x0);
			break;



		}


		/*
		 * 
		 * void graphics_draw_box 	( 	display_context_t  	disp,
		int  	x,
		int  	y,
		int  	width,
		int  	height,
		uint32_t  	color	 
	) 			 

Draw a filled rectangle to a display context.

Note:
    This function does not support transparency for speed purposes. To draw a transparent or translucent box, use graphics_draw_box_trans.

Parameters:
    [in] 	disp 	The currently active display context.
    [in] 	x 	The x coordinate of the top left of the box.
    [in] 	y 	The y coordinate of the top left of the box.
    [in] 	width 	The width of the box in pixels.
    [in] 	height 	The height of the box in pixels.
    [in] 	color 	The 32-bit RGBA color to draw to the screen. Use graphics_convert_color or graphics_make_color to generate this value. 
		 */


		graphics_draw_box(disp, myLines[i].x, myLines[i].y, myLines[i].w, myLines[i].h, graphics_make_color(myLines[i].r, myLines[i].g, myLines[i].b, 155));
		//   GsSortBoxFill(&myLines[i],&OT[currentBuffer],   10);


	}
}
