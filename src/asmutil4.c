#include "c:\source\include\globals.h"
#include "c:\source\include\asmutil4.h"

//extern unsigned char  *video_buffer = (char *)0x0a0000;
//unsigned char  *buffer = (char *) 0x0a0000;


void Set_Palette_Register(int index, RGB_color_ptr color)
{
	outp(PALETTE_MASK,0xff);
	outp(PALETTE_REGISTER_WR,index);
	outp(PALETTE_DATA,color->red);
	outp(PALETTE_DATA,color->green);
	outp(PALETTE_DATA,color->blue);
}

void set_rgb(int index, unsigned char r, unsigned char g, unsigned char b)
{
	outp(PALETTE_MASK,0xff);
	outp(PALETTE_REGISTER_WR,index);
	outp(PALETTE_DATA,r);
	outp(PALETTE_DATA,g);
	outp(PALETTE_DATA,b);
} 

void Get_Palette_Register(int index, RGB_color_ptr color)
{

// this function gets the data out of a color lookup regsiter and places it
// into color

// set the palette mask register

outp(PALETTE_MASK,0xff);

// tell vga card which register we will be reading

outp(PALETTE_REGISTER_RD, index);

// now extract the data

color->red   = inp(PALETTE_DATA);
color->green = inp(PALETTE_DATA);
color->blue  = inp(PALETTE_DATA);

} // end Get_Palette_Color

void Create_Cool_Palette(void)
{
	RGB_color color;
	int index;
	for (index=0; index<64; index++)
	{
                color.red=index;
                color.green=index;
                color.blue=index;
		Set_Palette_Register(index, (RGB_color_ptr)&color);
                color.red=index;
                color.green=index;
                color.blue=-index;
		Set_Palette_Register(index+64, (RGB_color_ptr)&color);
		color.red=63-index;
		color.green=63-index;
		color.blue=0;
		Set_Palette_Register(index+128, (RGB_color_ptr)&color);
		color.red=63-index;
		color.green=0;
		color.blue=0;
		Set_Palette_Register(index+192, (RGB_color_ptr)&color);
	}
	color.red = color.blue = color.green = 0;
	Set_Palette_Register(0,(RGB_color_ptr)&color);
}


void load_palette(unsigned char palette[768])
{
  RGB_color pal;
  int i;
  for(i=0;i<768;i+=3)
  {
   pal.red = palette[i];  pal.green = palette[i+1]; pal.blue = palette[i+2];
   Set_Palette_Register(i/3,(RGB_color_ptr)&pal);
  }
}

void save_palette(unsigned char palette[768])
{
  int i;
  RGB_color pal;
  for(i=0;i<768;i+=3)
  {
   Get_Palette_Register(i/3,(RGB_color_ptr)&pal);
   palette[i] = pal.red; palette[i+1] = pal.green; palette[i+2]=pal.blue;
  }
}

void fade_out(unsigned char pal[768])
{
	int i,j;
	int count=0;
        static unsigned char pal2[768];
	
	for(i=0;i<768;i++)
	 pal2[i] = pal[i];

	for (j=0; j<256 && count!=768; j++)
	{ count=0;
	for (i=0; i<768; i++)
	{
		if (pal2[i]>0) pal2[i]-=1;
		if (pal2[i]==0) count++;
		if (i%256==0)
		{
			Wait_For_Vsync();
			load_palette(pal2);
		}
	}
	}
}

void clear_palette(unsigned char pal[768])
{
	int i;
	for (i=0; i<768; i++) pal[i]=0;
}

void black_video_palette(void)
{
	int i;
        static unsigned char pal[768];
	for (i=0; i<768; i++) pal[i]=0;
	load_palette(pal);
}

void fade_in(unsigned char pal[768])
{
	int i,j;
	int count=0;
        static unsigned char pal2[768];

	clear_palette(pal2);
	for (j=0; j<256 && count!=768; j++)
	{ count=0;
	for (i=0; i<768; i++)
	{
		if (pal2[i]<pal[i]) pal2[i]+=1;
		if (pal2[i]==pal[i]) count++;
		if (i%256==0)
		{
			Wait_For_Vsync();
			load_palette(pal2);
		}
	}
	}
}
/*
void line(int x1, int y1, int x2, int y2, unsigned char color)
{
 int d,x,y,ax,ay,sx,sy,dx,dy;

 dx = x2-x1;  ax=ABS(dx)<<1;
 dy = y2-y1;  ay=ABS(dy)<<1;

 sx=SGN(dx);
 sy=SGN(dy);

 x=x1;
 y=y1;
 if(ax > ay)
 {
  d = ay-(ax>>1);
  for(;;)
  {
   buffer[(y<<6)+(y<<8)+x] = color;
   if(x==x2) return;
   if(d >= 0)
   {
    y+=sy;
    d-=ax;
   }
   x+=sx;
   d+=ay;
  }
 }
 else
 {
  d=ax-(ay>>1);
  for(;;)
  {
   buffer[(y<<6)+(y<<8)+x] = color;
   if(y==y2) return;
   if(d>=0)
   {
    x+=sx;
    d-=ay;
   }
   y+=sy;
   d+=ax;
  }
 }
}
*/
