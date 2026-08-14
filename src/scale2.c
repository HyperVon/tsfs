#include <stdio.h>
#include <conio.h>
#include "cotypes.h"
#include "pcx.h"
#include "co3de.h"

unsigned char pic[65536];
FILE *fp;

//center x,y /bitmap /scale
put_sprite(int x, int y, unsigned char *bmap, float scale_factor)
{
 int u,v,sx,sy,end_y,end_x,start_x;
 float scaled_width,scaled_height;
 float bmap_x,bmap_y,y_map_inc,x_map_inc;

 scaled_width= 256.0/scale_factor;
 scaled_height = 256.0/scale_factor;

 bmap_x = bmap_y = 0;
 y_map_inc = 256.0/scaled_height;
 x_map_inc = 256.0/scaled_width;
 end_y = y + scaled_height/2.0;
 end_x = x + scaled_width/2.0;
 start_x = x - scaled_width/2.0;

 for(sy = y - scaled_height/2.0;sy < end_y;sy++)
 {
  if(sy >=0 && sy <= 199)
  for(sx = start_x;sx < end_x;sx++)
  {
   if(sx >= 0 && sx <= 319)
   if(bmap[(int)(bmap_y*256)+(int)bmap_x])
    buffer[(sy<<6)+(sy<<8)+sx] = bmap[(int)(bmap_y*256)+(int)bmap_x];
   bmap_x += x_map_inc;
   fprintf(fp,"%f %f\n",bmap_x,bmap_y);
  }
  bmap_x=0;
  bmap_y+=y_map_inc;
 }
}

main()
{
 unsigned char pal[768];
 float scale_factor;
 int x,y;

 fp = fopen("generic2.out","wt");

 load_pcx("litebal2.pcx",pic,pal);

 Set_Video_Mode();
 load_palette(pal);
/*
 while(!feof(fp))
 {
  fscanf(fp,"%d %d %f",&x,&y,&scale_factor);
  put_sprite(x,y,pic,scale_factor);
  memset(buffer,0,64000);
 }
*/
 for(scale_factor=8.87;scale_factor>3.0;scale_factor-=1.0);
 {
  put_sprite(224,79,pic,9.87);
  getch();
  memset(buffer,0,64000);
 }

 getch();
 Set_Text_Mode();
 fclose(fp);
}
                

