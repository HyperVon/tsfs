#ifndef _CO3DE_H_
#define _CO3DE_H_

#include "cotypes.h"
//when remaking this lib uncomment the following line 
//#include "globals.h"
#include "fixed32.h"

extern unsigned char  *video_buffer;
extern unsigned char  *buffer;
extern unsigned char  *texturemap;
extern int intensity;
extern const int _319 , _199, _1;
extern int TEXTURE_WIDTH ;
extern int TEXTURE_HEIGHT;

extern unsigned char LUT[16384];
extern unsigned char TRANSPAR[65536];
extern float L[256][64];



void gouraud(TEXTURE * vtx,unsigned char color,unsigned char transparent);
void texture(TEXTURE * vtx,unsigned char transparent);
void tmapgour(TEXTURE * vtx,unsigned char transparent);
void tmapflat(TEXTURE * vtx,unsigned char transparent);
void flat(int x1, int y1, int x2, int y2, int x3, int y3,
        unsigned char color,unsigned char shade,unsigned char transparent);

void Set_Palette_Register(int index, RGB_color_ptr color);
void Get_Palette_Register(int index, RGB_color_ptr color);
void Create_Cool_Palette();
void load_palette(unsigned char palette[768]);
void save_palette(unsigned char palette[768]);
void fade_out(unsigned char pal[768]);
void clear_palette(unsigned char pal[768]);
void black_video_palette();
void fade_in(unsigned char pal[768]);
void set_rgb(int index, unsigned char r, unsigned char g, unsigned char b);
void line(int x1, int y1, int x2, int y2, unsigned char color);

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


#endif



