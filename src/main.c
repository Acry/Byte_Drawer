//BEGIN HEAD
//BEGIN DESCRIPTION
/*
 * Flipping Bits on a real char and visualize that.
 * 
 * This is WORK IN PROGRESS
 * 
 * The code is nearly at a fork state.
 * 
 * fork 1 is an educational program to learn about Bit-order aka endianess.
 * fork 2 is a pixel drawer to make monochromatic assets for classic VCS.
 * fork 3 could be a fully fledged Pixel/Icon Editor.
 * 
 */

/* DEFINED PROGRESS GOALS
 * 
 * For trunk:
 * DONE read and save byte
 * 
 * AFTER CODE REVIEW:
 * TODO Window resizable
 * TODO Scale Window content
 * 
 * For B1 - educational program:
 * TODO Switch sign/unsigned char
 * TODO Add Window ASCII TABLE
 * TODO Show dec/oct/hex-Values
 * TODO substitute endian png's with runtime pictures
 * TODO switch data signed/unsigned and type (int/float)
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
#define WHITE 255, 255, 255, 255
#define BLACK 0  ,   0,   0, 255
#define BLUE  0  ,   0, 255, 255
//END   CPP DEFINITIONS

//BEGIN DATASTRUCTURES
//END	DATASTRUCTURES

//BEGIN GLOBALS

SDL_Point mouse;
SDL_Rect  block[8];
SDL_bool  fill[8];
char      line;

TTF_Font  	*font;
SDL_Color 	 font_color = {BLUE};
SDL_Surface 	*font_surf  = NULL;
SDL_Texture 	*font_Tex   = NULL;
SDL_Rect 	 font_dst;

SDL_Surface 	*help_surf  = NULL;
SDL_Texture 	*le_tex     = NULL;
SDL_Rect 	 le_dst;

SDL_Texture 	*be_tex     = NULL;
SDL_Rect 	 be_dst;

SDL_Texture 	*numb_sys_t = NULL;
SDL_Rect 	 numb_sys_r;

//DEBUG HELP
char dump=0;
//END   GLOBALS

//BEGIN FUNCTION PROTOTYPES
void check_col		(void);
void check_line		(void);
void render_text	(void);
void dump_value		(void);
void read_file		(void);
void write_file		(void);
//END	FUNCTION PROTOTYPES

//END HEAD

//BEGIN MAIN FUNCTION
int main(int argc, char *argv[])
{

(void)argc;
(void)argv;

//BEGIN INIT
init();

//BEGIN LOAD STUFF AND CREATE TEXTURES
font=TTF_OpenFont("./assets/FiraMono-Medium.ttf", 43);
font_dst.x=50;
font_dst.y=150;


int w,h;
int i;
help_surf = IMG_Load( "./assets/l_e.png" );
le_tex = SDL_CreateTextureFromSurface( Renderer, help_surf );
SDL_QueryTexture(le_tex, NULL, NULL, &w, &h);
le_dst.w=w/2;
le_dst.h=h/2;
le_dst.x=50;

if (SDL_BYTEORDER==SDL_LIL_ENDIAN)
	le_dst.y=10;
else
	le_dst.y=250;

help_surf = IMG_Load( "./assets/b_e.png" );
be_tex = SDL_CreateTextureFromSurface( Renderer, help_surf );
SDL_QueryTexture(be_tex, NULL, NULL, &w, &h);
be_dst.w=w/2;
be_dst.h=h/2;
be_dst.x=50;

if (SDL_BYTEORDER==SDL_BIG_ENDIAN)
	be_dst.y=10;
else
	be_dst.y=250;


SDL_FreeSurface( help_surf );

for (i=0; i<8; i++){
	block[i].w=50;
	block[i].h=50;
	block[i].x=(i*50)+50;
	block[i].y=100;
}

for (i=0; i<8; i++){
	fill[i]=SDL_FALSE;
}
line=0;
render_text();
font_surf=TTF_RenderText_Blended(font,"2", font_color);
numb_sys_t = SDL_CreateTextureFromSurface( Renderer, font_surf );

SDL_QueryTexture(numb_sys_t, NULL, NULL, &w, &h);
numb_sys_r.w=w/2;
numb_sys_r.h=h/2;
SDL_QueryTexture(font_Tex, NULL, NULL, &w, &h);
numb_sys_r.x=w+font_dst.x;
numb_sys_r.y=h+font_dst.y-(numb_sys_r.h/2);

//END LOAD STUFF AND CREATE TEXTURES

SDL_SetWindowPosition(Window,0,0);
SDL_SetWindowSize(Window,540,540);
SDL_SetWindowTitle(Window, "Byte Drawer");
SDL_ShowWindow(Window);
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
		if(event.type == SDL_KEYDOWN )
		{
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

for (i=7; 0 <= i; i--){
	if (fill[i]==SDL_FALSE){
		SDL_SetRenderDrawColor(Renderer, BLACK);
		SDL_RenderDrawRect(Renderer, &block[i]);
	}
	else{

		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(Renderer, &block[i]);
		
		SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(Renderer, &block[i]);
	}
}
	SDL_RenderCopy(Renderer, le_tex, NULL, &le_dst);
	SDL_RenderCopy(Renderer, be_tex, NULL, &be_dst);
	SDL_RenderCopy(Renderer, numb_sys_t, NULL, &numb_sys_r);

SDL_RenderCopy(Renderer, font_Tex, NULL, &font_dst);

SDL_RenderPresent(Renderer);
//END   RENDERING

}
//END   MAIN LOOP

SDL_FreeSurface(font_surf);
SDL_DestroyTexture(font_Tex);
SDL_DestroyTexture(numb_sys_t);
SDL_DestroyTexture(le_tex);
SDL_DestroyTexture(be_tex);
TTF_CloseFont(font);

exit_();
return EXIT_SUCCESS;

}
//END   MAIN FUNCTION

//BEGIN FUNCTIONS
void check_col(void)
{
	for (int i=0; i<8; i++){
		if (SDL_PointInRect(&mouse, &block[i])){
			fill[i]=fill[i]^1; //XOR - flip var
			line = line ^ (1 << i);
			render_text();
		}
	}
}

void check_line(void)
{
	int i;
	for (i = 0; i<8; i++) {
		if( (line >> i) & 1 )
			fill[i]=fill[i]^1; //XOR - flip var
	}
}

void render_text(void)
{
	char buffer[16];
	char temp[2];

	int i;
	for (i = 7; 0 <= i; i --) {
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

	font_surf=TTF_RenderText_Blended(font,buffer, font_color);
	font_Tex = SDL_CreateTextureFromSurface( Renderer, font_surf );
	SDL_QueryTexture(font_Tex, NULL, NULL, &font_dst.w, &font_dst.h);
	
	if (dump)
		dump_value();
}

void dump_value(void)
{
	
	SDL_Log("line %c is %d in decimal\n",line,line);
	int line2=line;
	SDL_Log("Octal value is: %o\n",line2);
	SDL_Log("Hexadecimal value is (Alphabet in small letters): %x\n",line2);
	SDL_Log("Hexadecimal value is (Alphabet in capital letters): %X\n",line2);

}

void read_file(void)
{
	FILE *file;
	file = fopen( "data", "rb" );
	if (file==NULL){
		SDL_Log("no file opened, try write one before. Press W to save your bit.");
	}else{
		line=(char)fgetc(file);
		fclose( file );
	}
}

void write_file	(void)
{
	FILE *file;
	file = fopen( "data", "wb" );
	if (file==NULL){
		SDL_Log("Sorry couldn't write your Byte. Check write permissions.");
	}else{
		fputs(&line, file);
		fclose( file );
		if (dump)
			dump_value();
	}
}
//END   FUNCTIONS
