#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "c:\source\include\fixed32.h"
#include "c:\source\include\asmutil4.h"
#include "c:\source\include\pcx.h"
#include "c:\source\include\fonts.h"
#include "c:\source\include\hls.h"
#include "c:\source\include\luts.h"

int intensity;

#include "diff2c.c"

#define TIME_KEEPER_INT 0x1C

volatile static unsigned long time_keep=0;

void (_interrupt *Old_Isr)();

void _interrupt  Timer()
{
  time_keep++;
}

void Make_Fire_Palette(void)
{
	RGB_color color;
	int index;

	for (index=0; index<64; index++)
	{
		color.red=index;
		color.green=0;
		color.blue=0;
		Set_Palette_Register(index, (RGB_color_ptr)&color);
		color.red=63;
                color.green=index;
                color.blue=0;
		Set_Palette_Register(index+64, (RGB_color_ptr)&color);
                color.red=63;
                color.green=63;
                color.blue=index/2;
		Set_Palette_Register(index+128, (RGB_color_ptr)&color);
                color.red=63;
                color.green=63;
                color.blue=31+(index/2);
		Set_Palette_Register(index+192, (RGB_color_ptr)&color);
	}
	color.red = color.blue = color.green = 0;
	Set_Palette_Register(0,(RGB_color_ptr)&color);
}

void fire(void)
{
        int x,y,i,m;
        int color;
        static unsigned char *fire_buffer,first=1;
        unsigned char *src,*y_start_fire,*y_start_buffer;

        if(first)
        {
         fire_buffer = (unsigned char *) malloc(66*64);
         memset(fire_buffer,0,66*64);
         first=0;
        }

                for (y=64;y<66;y++)
                {
                 m=(y<<6);
                 for (x=0;x<64;x++)
                  fire_buffer[m+x] = rand() & 0x00FF;
                }
		
                for (y=1;y<66;y++)
		{
                    y_start_fire = fire_buffer + (y<<6);
                for (x=1;x<63;x++)
                {
		  src = y_start_fire + x;

                  color = (*src + *(src-64) + *(src+64) +
                                    *(src-1) + *(src+1))/5;
                  color -= rand()%6;
                  if (color<0) color=0;
                  *(src-64) = color;
                }
		}
		
                for(y=0;y<252;y+=4)
		{
                    y_start_fire = fire_buffer + (y<<4);
                    y_start_buffer = texturemap + (y<<8);
                for(x=0;x<252;x+=4)
		{
		    color = *(y_start_fire + (x>>2));
		    src = y_start_buffer  + x;
		    
		    *src = color;
		    *(src+1) = color;
		    *(src+2) = color;
		    *(src+3) = color;
		    
                    *(src+256) = color;
                    *(src+257) = color;
                    *(src+258) = color;
                    *(src+259) = color;

                    *(src+512) = color;
                    *(src+513) = color;
                    *(src+514) = color;
                    *(src+515) = color;

                    *(src+768) = color;
                    *(src+769) = color;
                    *(src+770) = color;
                    *(src+771) = color;
		}
		}
}


void main(int argc, char *argv[])
{
 POINT_3D temp,temp2;
 static int done,i,j,k,l,m,n;
 static char text_buffer[200],filename[200];
 static unsigned char pal[768],RED;
 int shade,oldtime,shade_type,num_objects;
 float start,end,frames;
 static OBJECT object[2],*object_ptr;



 texturemap = (unsigned char *) malloc (65536);
// load_pcx("texture.pcx",texturemap,pal);
 memset(texturemap,0,65536);

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

// make_shade_lut(pal);

// make_trans_lut(pal,.75);




 //INIT OBJECT
 m=findRGB(32,32,32,pal);
 RED = findRGB(64,0,0,pal);
 object[0].use_truespace_pos = 1;
 load_cobFN(argv[1],&object[0],NULL);

 for(j=0;j < object[0].num_polygons;j++)
 {
   object[0].polys[j].texturemap = texturemap;
   object[0].polys[j].mirror = 0;
   object[0].polys[j].shade_type = shade_type;
   object[0].polys[j].transparent = 0;
   object[0].polys[j].color = m;
 }
 object[0].shade_type = shade_type;
 object[0].cull_tag = 1;
 object[0].mirror = 0;
 object[0].texturemap = texturemap;
 object[0].x_ang = object[0].y_ang = object[0].z_ang = 0;
 object[0].animation    = NON_ANIMATED;
 object[0].frustrum     = 1;



 buffer=VIDEO_BUFFER;
 memset(buffer,0,VIDEO_STORAGE);

 load_fontFN("6x6.fnt",6,6);


/* initialize graphics mode */
 create_lookup_tables();
 Set_Video_Mode();

 Make_Fire_Palette();

// load_palette(pal);
 

 camera.viewpoint.x = 0;
 camera.viewpoint.y = 0;
 camera.viewpoint.z = -(20<<16);

 camera.lookat.x = 0;
 camera.lookat.y = 0;
 camera.lookat.z = 0;

 lightsource.x = 0;
 lightsource.y = 0;
 lightsource.z = -65536;

 frames = j = done = 0;
 
 

frames = done=0;
start = clock();

while(!done)
 {
  fire();

  if(kbhit())
   switch(getch())
   {
    case 'a': camera.viewpoint.x-=65536;
              break;
    case 's': camera.viewpoint.x+=65536;
              break;
    case 'w': camera.viewpoint.y+=65536;
              break;
    case 'z': camera.viewpoint.y-=65536;
              break;
    case '+': camera.viewpoint.z+=65536;
              break;
    case '-': camera.viewpoint.z-=65536;
              break;
    case 'd': done=1;
              break;
   }

  object[0].x_ang+=3;
  if(object[0].x_ang>=360) object[0].x_ang = 0;

  object[0].y_ang+=3;
  if(object[0].y_ang>=360) object[0].y_ang = 0;

  object[0].z_ang+=3;
  if(object[0].z_ang>=360) object[0].z_ang = 0;



 for (k=0;k < num_objects; k++)
  if(object[k].animation == ANIMATED)
   animate_object(&object[k]);

  //reset the polygon list to 0 polys
  num_polys=0;

  create_world_to_cam();

  for(k=0;k<num_objects;k++)
  if(!remove_object(&object[k]))
  {
    object_ptr = &object[k];

    world_to_cam(object_ptr);

    cull_and_shade(object_ptr);
  
    poly3d_frustrum_clip(object_ptr);

    build_master_facet_list(object_ptr);
  }


  //sort ONLY the facets that can be seen
  sort_viewable_faces();


  //put the camera 3d coords into screen coords
  perspective_transform();

  //draw the tris to the double buffer
  for(i=0;i<num_polys;i++)
  {
       intensity = master_facet_list[i]->shade;
       shade_type = master_facet_list[i]->shade_type;
       texturemap = master_facet_list[i]->texturemap;
       
       if(shade_type == ENVMAP)
         texture(master_facet_list[i]->quad,
          master_facet_list[i]->transparent);
       else if(shade_type == FLATTEX)
         tmapflat(master_facet_list[i]->quad,
                  master_facet_list[i]->transparent);
       else if(shade_type == GOURTEX)
         tmapgour(master_facet_list[i]->quad,
                  master_facet_list[i]->transparent);
       else if(shade_type == GOURAUD)
         gouraud(master_facet_list[i]->quad,
                 master_facet_list[i]->color,
                 master_facet_list[i]->transparent);
       else if (shade_type == TEXTUREMAP)
           texture(master_facet_list[i]->quad,
                   master_facet_list[i]->transparent);
       else if (shade_type == FLAT)
           flat(master_facet_list[i]->quad[0].x<<16,
                master_facet_list[i]->quad[0].y<<16,
                master_facet_list[i]->quad[1].x<<16,
                master_facet_list[i]->quad[1].y<<16,
                master_facet_list[i]->quad[2].x<<16,
                master_facet_list[i]->quad[2].y<<16,
                master_facet_list[i]->color,
                master_facet_list[i]->shade);
  }

  sprintf(text_buffer,"p: %d fps: %.2f",num_polys,(frames/((end-start)/100.0)));
  putstr(0,0,text_buffer,LEFT,RED);                     

// Wait_For_Vsync();
 memcpy(video_buffer,buffer,VIDEO_STORAGE);
 memset(buffer,0,VIDEO_STORAGE);
 end = clock();
 frames++;
}


  Set_Text_Mode();
}
