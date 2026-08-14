#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


unsigned char  *video_buffer = (char *)0x0a0000;
unsigned char  *buffer = (char *) 0x0a0000;
unsigned char  *texturemap;
int intensity;
const int _319 = 319<<16, _199 = 199<<16, _1 = 65536;
int TEXTURE_WIDTH;
int TEXTURE_HEIGHT;

#include "cotypes.h"
#include "fixed32.h"
#include "pcx.h"
#include "hls.h"
#include "luts.h"
#include "co3de.h"
#include "fonts.h"

//int intensity;


void main(int argc, char *argv[])
{
 POINT_3D temp,temp2;
 int done,i,j,k,l,m,n;
 char text_buffer[200],filename[200];
 unsigned char pal[768],RED;
 int shade,oldtime,shade_type,num_objects;
 float start,end,frames;
 OBJECT object[2],*object_ptr;



 texturemap = (unsigned char *) malloc (65536);
 load_pcx("texture.pcx",texturemap,pal);

 num_objects = 1;

 if(argc != 3)
 {
  printf("usage: generic1 object.cob render_type\n");
  exit(1);
 }

 shade_type = atoi(argv[2]);
 if(shade_type < 0 || shade_type > 7)
 {
  printf("render type must be 0-7\n");
  exit(1);
 }

 make_shade_lut(pal);

// make_trans_lut(pal,.75);

 //INIT OBJECT
 m=findRGB(32,32,32,pal);
 RED = findRGB(64,0,0,pal);
 object[0].use_truespace_pos = 1;

}
