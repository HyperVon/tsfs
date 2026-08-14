#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "cotypes.h"
#include "fixed32.h"
#include "pcx.h"
#include "co3de.h"
#include "fonts.h"

#include "diff2c.c"

#define TIME_KEEPER_INT 0x1C

volatile static unsigned long time_keep=0;

void (_interrupt *Old_Isr)();

void _interrupt  Timer()
{
  time_keep++;
}

int load_world(OBJECT *object, unsigned char *pal,char *name)
{
        int num_objects,tmp,i,runtime,maxfps,m,shade_type,j;
        int transparent,R,G,B,cull_tag;
        static char filename[100];
        FILE *fp,*world;

        world = fopen(name,"rb");

        fscanf(world,"%d",&runtime);
        fscanf(world,"%d",&maxfps);
        fscanf(world,"%d",&num_objects);
        fscanf(world,"%s",filename);



        load_pcx(filename,texturemap,pal);



        for (i=0; i<num_objects; i++)
        {
                fscanf(world,"%s",filename);

                fscanf(world,"%d",&tmp);
                if (tmp) object[i].animation=ANIMATED;
                else object[i].animation=NON_ANIMATED;

                fscanf(world,"%d",&shade_type);
                object[i].shade_type=shade_type;

                fscanf(world,"%d",&object[i].use_truespace_pos);
                if (object[i].use_truespace_pos == 0)
                {
                        fscanf(world,"%d",&object[i].world_pos.x);
                        fscanf(world,"%d",&object[i].world_pos.y);
                        fscanf(world,"%d",&object[i].world_pos.z);
                        object[i].world_pos.x*=65536;
                        object[i].world_pos.y*=65536;
                        object[i].world_pos.z*=65536;
                }


                fscanf(world,"%d",&tmp);
                object[i].texturemap=texturemap+tmp*65536;

                fscanf(world,"%d",&object[i].frustrum);

                fscanf(world,"%d",&transparent);
                fscanf(world,"%d",&R);
                fscanf(world,"%d",&G);
                fscanf(world,"%d",&B);
                fscanf(world,"%d",&cull_tag);

                m=findRGB(R,G,B,pal);

                load_cobFN(filename,&object[i],NULL);

         for(j=0;j < object[i].num_polygons;j++)
         {
           object[i].polys[j].texturemap = texturemap+tmp*65536;
           object[i].polys[j].mirror = 0;
           object[i].polys[j].shade_type = shade_type;
           object[i].polys[j].transparent = transparent;
           object[i].polys[j].color = m;
         }
         object[i].shade_type = shade_type;
         object[i].cull_tag = cull_tag;
         object[i].mirror = 0;
         object[i].texturemap = texturemap+tmp*65536;
         object[i].x_ang = object[i].y_ang = object[i].z_ang = 0;
        }
        return(num_objects);
}



void main(int argc, char *argv[])
{
 POINT_3D temp,temp2;
 static int done,i,j,k,l,m,n;
 static char text_buffer[200],filename[200];
 unsigned char pal[768],RED;
 int shade,oldtime,shade_type,num_objects;
 float start,end,frames;
 static OBJECT object[16],*object_ptr;
 FILE *fp;



 texturemap = (unsigned char *) malloc (5*65536);

 if(argc != 2)
 {
  printf("usage: generic1  world\n");
  exit(1);
 }


 //INIT OBJECT
 RED = findRGB(64,0,0,pal);
 m = findRGB(0,0,32,pal);

 num_objects = load_world(object,pal,argv[1]);

 fp = fopen("bunch3s.lut","rb");
 fread(LUT,sizeof(unsigned char),16384,fp);
 fclose(fp);
// make_shade_lut(pal);

 fp = fopen("bunch3.lut","rb");
 fread(TRANSPAR,sizeof(unsigned char),65536,fp);
 fclose(fp);

 buffer=VIDEO_BUFFER;
 memset(buffer,0,VIDEO_STORAGE);

 load_fontFN("6x6.fnt",6,6);


/* initialize graphics mode */
 create_lookup_tables();
 Set_Video_Mode();

 load_palette(pal);
 

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

  if(kbhit())
   switch(getch())
   {
    case 'a': camera.viewpoint.x-=12000;
              break;
    case 's': camera.viewpoint.x+=12000;
              break;
    case 'w': camera.viewpoint.y+=12000;
              break;
    case 'z': camera.viewpoint.y-=12000;
              break;
    case '+': camera.viewpoint.z+=12000;
              break;
    case '-': camera.viewpoint.z-=12000;
              break;
    case 'd': done=1;
              break;
   }

 for(k=0;k<num_objects;k++)
 {
  object[k].x_ang+=3;
  if(object[k].x_ang>=360) object[k].x_ang = 0;

  object[0].y_ang+=3;
  if(object[0].y_ang>=360) object[k].y_ang = 0;

  object[k].z_ang+=3;
  if(object[k].z_ang>=360) object[k].z_ang = 0;
 }


 for (k=0;k < num_objects; k++)
  if(object[k].animation == ANIMATED)
   animate_object(&object[k]);

  //reset the polygon list to 0 polys
  num_polys=0;

  create_world_to_cam();

  //put the floor first which is object[0]
k=0;
//  for(k=0;k<num_objects;k++)
//  if(!remove_object(&object[k]))
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
                master_facet_list[i]->shade,
                master_facet_list[i]->transparent);
  }

  num_polys=0;

  for(k=1;k<num_objects;k++)
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
                master_facet_list[i]->shade,
                master_facet_list[i]->transparent);
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

