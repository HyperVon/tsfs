#include <stdio.h>
#include <conio.h>
#include "cotypes.h"
#include "pcx.h"
#include "co3de.h"

unsigned char pic[65536];

//center x,y /bitmap /scale
put_sprite(int x, int y, unsigned char *bmap, float scale_factor)
{
 int u,v,sx,sy,r,s;
 float scaled_width,scaled_height;
 float bmap_x,bmap_y,y_map_inc,x_map_inc;

 scaled_width  = (float)((float)256/(float)scale_factor);
 scaled_height = (float)((float)256/(float)scale_factor);

 bmap_x = bmap_y = 0;
 y_map_inc = 256.0/scaled_height;
 x_map_inc = 256.0/scaled_width;

 for(sy = y - scaled_height/2;sy < y + scaled_height/2;sy++)
 {
  if(sy >=0 && sy <= 199)
  for(sx = x - scaled_width/2;sx < x + scaled_width/2;sx++)
  {
   if(sx >= 0 && sx <= 319)
   r = bmap_x;
   s = bmap_y;
   if(bmap[s*256+r]!=bmap[0])
    buffer[(sy<<6)+(sy<<8)+sx] = bmap[s*256+r];
   bmap_x += x_map_inc;
  }
  bmap_x=0;
  bmap_y+=y_map_inc;
 }
}

main()
{
 unsigned char pal[768];
 float scale_factor;

 load_pcx("litebal2.pcx",pic,pal);

 Set_Video_Mode();
 load_palette(pal);

 memset(buffer,25,64000);
 put_sprite(224,79,pic,4.78);

 getch();
 Set_Text_Mode();
}
                

