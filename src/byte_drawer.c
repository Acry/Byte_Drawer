//BEGIN HEAD
//BEGIN DESCRIPTION
/*
 * Flipping Bits on a real char and visualize that.
 * 
 * This is WORK IN PROGRESS
 * 
 * The code is at a fork state.
 * 
 * fork 1 is an educational program to learn about Bit-order, Byte Order
 * 	  aka endianess and Data Representation and C BinOps.
 * 
 * fork 2 is a pixel drawer to make monochromatic assets for classic VCS.
 * 
 * fork 3 could be a fully fledged Pixel/Icon Editor.
 * 
 */

/* DEFINED PROGRESS GOALS
 * 
 * For trunk:
 * DONE read and save byte
 * 
 * TODO Check Big Endian usability
 * DONE Window resizable
 * DONE Scale Window content
 * 
 * For B1 - educational program:
 * TODO Switch sign/unsigned char
 * TODO Add Window ASCII TABLE
 * TODO Show dec/oct/hex-Values
 * TODO substitute endian png's with runtime pictures
 * TODO switch data signed/unsigned and type (int/float)
 * TODO Text Input for Value
 * 
 * For B2 - pixel drawer:
 * TODO Add Line
 * TODO Del Line
 * TODO Toggle Line
 * TODO Clone Line
 * TODO vertical Drawing
 * TODO A viewer
 * 
 * For B3 - Icon Editor:
 * TODO Multicolor
 *
 */
//END   DESCRIPTION

//BEGIN INCLUDES
//local header
#include "helper.h"
//END   INCLUDES

//BEGIN CPP DEFINITIONS
#define RES_H 340
#define RES_W 500

#define WHITE 255, 255, 255, 255
#define BLACK 0  ,   0,   0, 255
#define BLUE  0  ,   0, 255, 255
//END   CPP DEFINITIONS

//BEGIN DATASTRUCTURES
struct vec2{
	float x;
	float y;
};

struct rect{
	struct vec2 pos;
	struct vec2 size;
};

struct _canvas{
	SDL_Texture *Texture;
	struct rect frac;
	SDL_Rect dst;
	struct rect frac_last;
};

struct area{
	SDL_Texture *Texture;
	struct rect frac;
	SDL_Rect dst;
	struct rect frac_last;
};
//END	DATASTRUCTURES

//BEGIN GLOBALS
int IARH;  						//Image Aspect Ratio horizontal
int IARW;  						//Image Aspect Ratio vertical
struct vec2 current_scale;
int WW 				= RES_W;		//Window Width
int WH				= RES_H;		//Window Height
struct _canvas action_area;

SDL_Point mouse;
struct area bit[8];
SDL_bool  fill[8];

unsigned char    line;

struct area byte_numbers;
TTF_Font  	*font;
SDL_Color 	 font_color = {BLUE};

SDL_Surface 	*help_surf  = NULL;

struct area le;			//little endian
struct area be;			//big endian
struct area ns;			//number system

//DEBUG HELP
char dump=1;
//END   GLOBALS

//BEGIN FUNCTION PROTOTYPES
void check_col		(void);
void check_line		(void);
void render_text	(void);
void dump_value		(void);
void read_file		(void);
void write_file		(void);

int  gcd		(int, int);
void scale_all		(void);
void init_canvas	(void);
void scale_canvas	(struct _canvas *s_entity);
void center_canvas	(void);
void scale_area		(struct area    *entity);
void rect_copy		(struct area 	*rect);
//END	FUNCTION PROTOTYPES

//END HEAD

//BEGIN MAIN FUNCTION
int main(int argc, char *argv[])
{

(void)argc;
(void)argv;

//BEGIN INIT
init();
init_canvas();
//BEGIN LOAD STUFF AND CREATE TEXTURES
font=TTF_OpenFont("./assets/FiraMono-Medium.ttf", 43);
byte_numbers.dst.x=54;
byte_numbers.dst.y=150;

int w,h;
int i;
//LE
help_surf = IMG_Load( "./assets/l_e.png" );

le.Texture = SDL_CreateTextureFromSurface( Renderer, help_surf );
SDL_QueryTexture(le.Texture, NULL, NULL, &w, &h);
le.dst.w=w/2;
le.dst.h=h/2;
le.dst.x=50;

if (SDL_BYTEORDER==SDL_LIL_ENDIAN)
	le.dst.y=10;
else
	le.dst.y=250;
rect_copy(&le);
//BE
help_surf = IMG_Load( "./assets/b_e.png" );
be.Texture = SDL_CreateTextureFromSurface( Renderer, help_surf );
SDL_QueryTexture(be.Texture, NULL, NULL, &w, &h);
be.dst.w=w/2;
be.dst.h=h/2;
be.dst.x=50;

if (SDL_BYTEORDER==SDL_BIG_ENDIAN)
	be.dst.y=10;
else
	be.dst.y=250;
rect_copy(&be);

for (i=0; i<8; i++){
	bit[i].dst.w=50;
	bit[i].dst.h=50;
	bit[i].dst.x=50+(i*50);
	bit[i].dst.y=100;
	rect_copy(&bit[i]);
}

for (i=0; i<8; i++){
	fill[i]=SDL_FALSE;
}
line=0;
render_text();
rect_copy(&byte_numbers);

//NS
help_surf=TTF_RenderText_Blended(font,"2", font_color);
ns.Texture = SDL_CreateTextureFromSurface( Renderer, help_surf );

SDL_QueryTexture(ns.Texture, NULL, NULL, &w, &h);
ns.dst.w=w/2;
ns.dst.h=h/2;
SDL_QueryTexture(byte_numbers.Texture, NULL, NULL, &w, &h);
ns.dst.x=w+byte_numbers.dst.x;
ns.dst.y=h+byte_numbers.dst.y-(ns.dst.h/2);

//END LOAD STUFF AND CREATE TEXTURES

SDL_SetWindowPosition(Window,0,0);
SDL_SetWindowSize(Window,WW,WH);
SDL_SetWindowTitle(Window, "Byte Drawer");
SDL_ShowWindow(Window);
scale_all();
SDL_Event event;
int running = 1;
//END   INIT

//BEGIN MAIN LOOP
while(running){
//BEGIN EVENT LOOP
	while(SDL_PollEvent(&event)){
		if(event.type == SDL_QUIT){
			running = 0;
		}
		if(event.type == SDL_WINDOWEVENT){
			if (event.window.event == SDL_WINDOWEVENT_RESIZED){
				WW = event.window.data1;
				WH = event.window.data2;
				scale_all();
			}
		}
		if(event.type == SDL_KEYDOWN ){
			switch(event.key.keysym.sym ){
				case SDLK_ESCAPE:
					running = 0;
					break;
					
				case SDLK_r:
					read_file();
					check_line();
					render_text();
					break;
					
				case SDLK_w:
					write_file();
					break;
					
				default:
					break;
			}
		}
		if(event.type == SDL_MOUSEBUTTONDOWN){
			SDL_GetMouseState(&mouse.x, &mouse.y);
			if(event.button.button == SDL_BUTTON_RIGHT){
				;
			}
			if(event.button.button == SDL_BUTTON_LEFT){
				check_col();
			}
		}
	}
//END   EVENT LOOP

//BEGIN RENDERING
SDL_SetRenderDrawColor(Renderer, WHITE);
SDL_RenderClear(Renderer);

for (i=0; i <8; i++){
	if (fill[i]==SDL_FALSE){
		SDL_SetRenderDrawColor(Renderer, BLACK);
		SDL_RenderDrawRect(Renderer, &bit[i].dst);
	}
	else{

		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(Renderer, &bit[i].dst);
		
		SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(Renderer, &bit[i].dst);
	}
}
	SDL_RenderCopy(Renderer, le.Texture, NULL, &le.dst);
	SDL_RenderCopy(Renderer, be.Texture, NULL, &be.dst);
	SDL_RenderCopy(Renderer, ns.Texture, NULL, &ns.dst);

	SDL_RenderCopy(Renderer, byte_numbers.Texture, NULL, &byte_numbers.dst);

SDL_RenderPresent(Renderer);
//END   RENDERING

}
//END   MAIN LOOP

SDL_FreeSurface(help_surf);
SDL_DestroyTexture(byte_numbers.Texture);
SDL_DestroyTexture(ns.Texture);
SDL_DestroyTexture(le.Texture);
SDL_DestroyTexture(be.Texture);
TTF_CloseFont(font);

exit_();
return EXIT_SUCCESS;

}
//END   MAIN FUNCTION

//BEGIN FUNCTIONS
void check_col(void)
{
	for (int i=0; i<8; i++){
		if (SDL_PointInRect(&mouse, &bit[i].dst)){
			
			fill[i]=fill[i]^1; //XOR - flip var
			if (line & 1 << i)
				line = line & ~(1 << i);
			else
				line = line | (1 << i);

			render_text();
		}
	}
}

void check_line(void)
{
	int i;
	for (i = 0; i<8; i++) {
		if (line & 1 << i)
			fill[i]=1;
		else
			fill[i]=0;
	}
}

void render_text(void)
{
	char buffer[16];
	char temp[2];

	for (int i = 7; 0 <= i; i--) {
		sprintf(temp, "%d", !!((line << i) & 0x80));
		if (i==7){
			strcpy(buffer,temp);
			strcat(buffer," ");
		}
		else{
			strcat(buffer,temp);
			strcat(buffer," ");
		}
	}

	help_surf=TTF_RenderText_Blended(font,buffer, font_color);
	byte_numbers.Texture = SDL_CreateTextureFromSurface( Renderer, help_surf );
	int w,h;
	SDL_QueryTexture(byte_numbers.Texture, NULL, NULL, &w, &h);
	if (WW!=RES_W){
		byte_numbers.frac.size.x=w*current_scale.x;
		byte_numbers.frac.size.y=h*current_scale.y;
		byte_numbers.dst.w=roundf(byte_numbers.frac.size.x);
		byte_numbers.dst.h=roundf(byte_numbers.frac.size.y);
	} else{
		byte_numbers.frac.size.x=w;
		byte_numbers.frac.size.y=h;
		byte_numbers.dst.w=roundf(byte_numbers.frac.size.x);
		byte_numbers.dst.h=roundf(byte_numbers.frac.size.y);
	}
	if (dump)
		dump_value();
}

void dump_value(void)
{

	char buffer[10];
	char temp[2];
	for (int i = 7; 0 <= i; i--) {
		sprintf(temp, "%d", !!((line << i) & 0x80));
		if (i==7){
			strcpy(buffer,temp);
		}
		else if (i==4){
			strcat(buffer,temp);
			strcat(buffer," ");
		}
		else{
			strcat(buffer,temp);
		}
	}
	strcat(buffer,"\0");
	SDL_Log("%s",buffer);
	
	SDL_Log("Byte represents %c as char and %d as decimal\n",line,line);
	int line2=line;
	SDL_Log("Octal value is: %o\n",line2);
	SDL_Log("Hexadecimal value is (Alphabet in small letters): %x\n",line2);
	SDL_Log("Hexadecimal value is (Alphabet in capital letters): %X\n",line2);
	SDL_Log("\n");

}

void read_file(void)
{
	FILE *file;
	file = fopen( "data", "rb" );
	if (file==NULL){
		SDL_Log("No file opened, try write one before. Press W to save your bit.");
	}else{
		line=(unsigned char)fgetc(file);
		fclose( file );
		SDL_Log("Read File success");
	}
	
}

void write_file	(void)
{
	FILE *file;
	file = fopen( "data", "wb" );
	if (file==NULL){
		SDL_Log("Sorry couldn't write your Byte. Check write permissions.");
	}else{
// 		fputc(line, file);
		fwrite( &line, sizeof line, 1, file );
		fclose( file );
		if (dump)
			dump_value();
		SDL_Log("Write File success");
	}
}

//BEGIN SCALING
int gcd (int a, int b)
{
	
	return (b == 0) ? a : gcd (b, a%b);
	
}

void init_canvas(void)
{
	//BEGIN ACTION AREA
	action_area.frac.size.x=RES_W;
	action_area.frac.size.y=RES_H;
	action_area.frac.pos.x=0;
	action_area.frac.pos.y=0;
	
	action_area.dst.x=0;
	action_area.dst.y=0;
	action_area.dst.w=roundf(action_area.frac.size.x);
	action_area.dst.h=roundf(action_area.frac.size.y);
	//END ACTION AREA
	
	//BEGIN SCALING
	int gcd_;
	gcd_= gcd(action_area.frac.size.x,action_area.frac.size.y);
	IARH=action_area.frac.size.y/gcd_;
	IARW=action_area.frac.size.x/gcd_;
	SDL_Log("Aspect Ratio: %d:%d",IARW,IARH);
	//END SCALING
	
}

void scale_all(void)
{
	scale_canvas(&action_area);
	current_scale.x = action_area.frac.size.x / (float)RES_W;
	current_scale.y = action_area.frac.size.y / (float)RES_H;
	center_canvas();

	scale_area(&le);
	scale_area(&be);
	scale_area(&ns);
	scale_area(&byte_numbers);
	for (int i=0; i<8; i++){
		scale_area(&bit[i]);
	}

	
	
}

void scale_canvas(struct _canvas *s_entity)
{
	s_entity->frac_last.size.x=s_entity->frac.size.x;
	s_entity->frac_last.size.y=s_entity->frac.size.y;
	
	if ( (float)WH/(float)IARH  < (float)WW/(float)IARW ){
		s_entity->dst.h=WH;
		s_entity->frac.size.y=(float)WH;
		s_entity->frac.size.x=(s_entity->frac.size.y / (float)IARH ) * (float)IARW;
		s_entity->dst.w=roundf( s_entity->frac.size.x );
	}else{
		s_entity->dst.w=WW;
		s_entity->frac.size.x=(float)WW;
		s_entity->frac.size.y=(s_entity->frac.size.x / (float)IARW ) * (float)IARH;
		s_entity->dst.h=roundf( s_entity->frac.size.y );
	}
}

void center_canvas(void)
{
	
	action_area.frac_last.pos.x=action_area.frac.pos.x;
	action_area.frac_last.pos.y=action_area.frac.pos.y;
	
	if (action_area.dst.h<=WH){
		action_area.frac.pos.y = ( (float) WH / 2 ) - action_area.frac.size.y / 2;
		action_area.dst.y = roundf(action_area.frac.pos.y);
	}
	
	if (action_area.dst.w<=WW){
		action_area.frac.pos.x = ( (float) WW / 2 ) - action_area.frac.size.x / 2;
		action_area.dst.x = roundf(action_area.frac.pos.x);
	}
	
	SDL_RenderSetClipRect(Renderer, &action_area.dst);
	
}

void scale_area(struct area *s_entity)
{
	
	//SCALE SIZE
	s_entity->frac.size.y = action_area.frac.size.y * (s_entity->frac.size.y/action_area.frac_last.size.y);
	s_entity->frac.size.x = action_area.frac.size.x * (s_entity->frac.size.x/action_area.frac_last.size.x);
	
	s_entity->dst.h=roundf( s_entity->frac.size.y );
	s_entity->dst.w=roundf( s_entity->frac.size.x );
	
	//SCALE POSITION
	s_entity->frac.pos.x=( action_area.frac.size.x / ( action_area.frac_last.size.x / (s_entity->frac.pos.x-action_area.frac_last.pos.x) ) ) + action_area.frac.pos.x;
	s_entity->frac.pos.y=( action_area.frac.size.y / ( action_area.frac_last.size.y / (s_entity->frac.pos.y-action_area.frac_last.pos.y) ) ) + action_area.frac.pos.y;
	
	s_entity->dst.x=roundf( s_entity->frac.pos.x );
	s_entity->dst.y=roundf( s_entity->frac.pos.y );
	
}

void rect_copy(struct area *rect)
{
	rect->frac.size.x=(float)rect->dst.w;
	rect->frac.size.y=(float)rect->dst.h;
	rect->frac.pos.x =(float)rect->dst.x;
	rect->frac.pos.y =(float)rect->dst.y;
}
//END SCALING
//END   FUNCTIONS
