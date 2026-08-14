#include <stdio.h>
#include <math.h>


#include "pcx.h"

extern unsigned char  *video_buffer;
extern unsigned char  *buffer;
extern unsigned char  *texturemap;
typedef struct RGB_color_typ
		{
			unsigned char red;
			unsigned char green;
			unsigned char blue;
		} RGB_color, *RGB_color_ptr;

void Set_Palette_Register(int index, RGB_color_ptr color);
void load_palette(unsigned char palette[768]);
void set_rgb(int index, unsigned char r, unsigned char g, unsigned char b);

extern unsigned char TRANSPAR[65536];
#include "texture2.c"

void Set_Video_Mode(void);
#pragma aux Set_Video_Mode = \ 
	"mov eax,013h"\
	"int 10h" \
	modify [eax];

void Set_Text_Mode(void);
#pragma aux Set_Text_Mode = \
	"mov eax,003h"       \
	"int 10h " \
	modify [eax];




main()
{
 TEXTURE tri[3];
 unsigned char pal[768];

 texturemap = (unsigned char *) malloc(65536);

 tri[0].x = 0;
 tri[0].y = 0;
 tri[0].tx = 0;
 tri[0].ty = 0;

 tri[1].x = 0;
 tri[1].y = 199;
 tri[1].tx = 0;
 tri[1].ty = 255;

 tri[2].x = 319;
 tri[2].y = 199;
 tri[2].tx = 255;
 tri[2].ty = 255;


 load_pcx("texture.pcx",texturemap,pal);

 Set_Video_Mode();
 load_palette(pal);

 texture(tri,0);

 getch();

 Set_Text_Mode();
}
