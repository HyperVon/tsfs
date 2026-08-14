// INCLUDES //

#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <dos.h>

// DEFINES //

#define VGA_INPUT_STATUS_1 0x3DA
#define VGA_VSYNC_MASK 0x08
#define PALETTE_MASK 0x3C6
#define PALETTE_REGISTER_RD 0x3C7
#define PALETTE_REGISTER_WR 0x3C8
#define PALETTE_DATA 0x3C9
#define VIDEO_STORAGE 64000*sizeof(unsigned char)
#define PALETTE_STORAGE 768*sizeof(unsigned char)
#define PIC_PTR (unsigned char *)
#define VIDEO_BUFFER PIC_PTR malloc(VIDEO_STORAGE)
//#define plot_pixel(x,y,color) buffer[((y<<8) + (y<<6)) + x] = color
//#define get_pixel(x,y) buffer[((y<<8) + (y<<6)) + x]
#define Wait_For_Vsync()\
while(inp(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK);\
while(!(inp(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK))
#define ABS(x) (((x) < 0) ? -(x) : (x))
#define SGN(a) (((a) < 0) ? -1 : 1)
#define SWAP(a,b) {a^=b; b^=a; a^=b;}


// GLOBAL STUCTURES //


typedef struct RGB_color_typ
		{
			unsigned char red;
			unsigned char green;
			unsigned char blue;
		} RGB_color, *RGB_color_ptr;

// Prototypes //

extern void Set_Palette_Register(int index, RGB_color_ptr color);
extern void Get_Palette_Register(int index, RGB_color_ptr color);
extern void Create_Cool_Palette();
extern void load_palette(unsigned char palette[768]);
extern void save_palette(unsigned char palette[768]);
extern void fade_out(unsigned char pal[768]);
extern void clear_palette(unsigned char pal[768]);
extern void black_video_palette();
extern void fade_in(unsigned char pal[768]);
extern void set_rgb(int index, unsigned char r, unsigned char g, unsigned char b);
extern void line(int x1, int y1, int x2, int y2, unsigned char color);

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
