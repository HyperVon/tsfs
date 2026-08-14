#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <dos.h>
#include "cotypes.h"
#include "fixed32.h"
#include "pcx.h"
#include "co3de.h"
#include "fonts.h"


#include "diff2c.c"

#define TIME_KEEPER_INT 0x1C

unsigned long time_keep=0;

void (_interrupt *Old_Isr)();

void _interrupt  Timer()
{
  time_keep++;
}


void main(int argc, char *argv[])
{
 POINT_3D temp,temp2,temp3,light_orig;
 static int done,i,j,k,l,m,n;
 static char text_buffer[200],filename[200];
 static unsigned char pal[768],RED;
 int shade,old_time,shade_type,num_objects;
 float start,end,frames;
 static OBJECT object[2],*object_ptr;
 POINT_3D *inc;
 VECTOR_3D *v_inc;



 texturemap = (unsigned char *) malloc (65536);
 load_pcx("texture.pcx",texturemap,pal);

 num_objects = 1;

 shade_type = 7;

 make_shade_lut(pal);


 //INIT OBJECT
 m=findRGB(32,32,32,pal);
 RED = findRGB(64,0,0,pal);

 object[0].use_truespace_pos = 1;
 load_cobFN("obj1.cob",&object[0],NULL);

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
 object[0].frustrum     = 0;
 object[0].world_pos.x =  object[0].world_pos.y =  object[0].world_pos.z=0;

 object[1].use_truespace_pos = 1;
 load_cobFN("obj2.cob",&object[1],NULL);

 for(j=0;j < object[1].num_polygons;j++)
 {
   object[1].polys[j].texturemap = texturemap;
   object[1].polys[j].mirror = 0;
   object[1].polys[j].shade_type = shade_type;
   object[1].polys[j].transparent = 0;
   object[1].polys[j].color = m;
 }
 object[1].shade_type = shade_type;
 object[1].cull_tag = 1;
 object[1].mirror = 0;
 object[1].texturemap = texturemap;
 object[1].x_ang = object[1].y_ang = object[1].z_ang = 0;
 object[1].animation    = NON_ANIMATED;
 object[1].frustrum     = 0;
 object[1].world_pos.x =  object[1].world_pos.y =  object[1].world_pos.z=0;

 inc = (POINT_3D *) malloc(sizeof(POINT_3D)*object[0].num_vertices);
 v_inc = (POINT_3D *) malloc(sizeof(POINT_3D)*object[0].num_polygons);

 //calc step values
 for(i=0;i<object[0].num_vertices;i++)
 {
  inc[i].x = fixeddiv((object[1].local_vertices[i].x -
             object[0].local_vertices[i].x),(626<<16));
  inc[i].y = fixeddiv((object[1].local_vertices[i].y -
             object[0].local_vertices[i].y),(626<<16));
  inc[i].z = fixeddiv((object[1].local_vertices[i].z -
             object[0].local_vertices[i].z),(626<<16));
 }

 for(i=0;i<object[0].num_polygons;i++)
 {
  v_inc[i].x = fixeddiv((object[1].polys[i].normal.x -
             object[0].polys[i].normal.x),(626<<16));
  v_inc[i].y = fixeddiv((object[1].polys[i].normal.y -
             object[0].polys[i].normal.y),(626<<16));
  v_inc[i].z = fixeddiv((object[1].polys[i].normal.z -
             object[0].polys[i].normal.z),(626<<16));

 }



 buffer=VIDEO_BUFFER;
 memset(buffer,0,VIDEO_STORAGE);

 load_fontFN("6x6.fnt",6,6);


/* initialize graphics mode */
 create_lookup_tables();
 Set_Video_Mode();

 load_palette(pal);
 

 temp.x = camera.viewpoint.x = 0;
 temp.y =camera.viewpoint.y = 0;
 temp.z =camera.viewpoint.z = -(5<<16);

 camera.lookat.x = 0;
 camera.lookat.y = 0;
 camera.lookat.z = 0;

 light_orig.x=lightsource.x = 0;
light_orig.y= lightsource.y = 0;
light_orig.z= lightsource.z = -65536;



 frames = j = done = 0;
 
//  Old_Isr = _dos_getvect(TIME_KEEPER_INT);
//  _dos_setvect(TIME_KEEPER_INT, Timer);

frames = done=0;
start = clock();
 time_keep=0;

while(!done)
 {
//  old_time = time_keep;
  if(kbhit())
   switch(getch())
   {
    case '4': camera.viewpoint.x-=65536/5;
              break;
    case '6': camera.viewpoint.x+=65536/5;
              break;
    case '8': camera.viewpoint.y+=65536/5;
              break;
    case '2': camera.viewpoint.y-=65536/5;
              break;
    case '+': camera.viewpoint.z+=65536/5;
              break;
    case '-': camera.viewpoint.z-=65536/5;
              break;
    case 'a': camera.lookat.x-=65536/5;
              break;
    case 's': camera.lookat.x+=65536/5;
              break;
    case 'w': camera.lookat.y+=65536/5;
              break;
    case 'z': camera.lookat.y-=65536/5;
              break;
    case '[': camera.lookat.z+=65536/5;
              break;
    case ']': camera.lookat.z-=65536/5;
              break;
    case '7': lightsource.x-=65536/5;
              normalize_fixed(&lightsource);
              break;
    case '9': lightsource.x+=65536/5;
              normalize_fixed(&lightsource);

              break;
    case '1': lightsource.y+=65536/5;
              normalize_fixed(&lightsource);

              break;
    case '3': lightsource.y-=65536/5;
              normalize_fixed(&lightsource);

              break;
    case '0': lightsource.z+=65536/5;
              normalize_fixed(&lightsource);

              break;
    case '.': lightsource.z-=65536/5;
              normalize_fixed(&lightsource);

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

  object[1].x_ang+=3;
  if(object[1].x_ang>=360) object[1].x_ang = 0;

  object[1].y_ang+=3;
  if(object[1].y_ang>=360) object[1].y_ang = 0;

  object[1].z_ang+=3;
  if(object[1].z_ang>=360) object[1].z_ang = 0;

 j+=3;
 if(j >= 360) j=0;

 rotate_about_y(temp,temp2,j);
 rotate_about_x(temp2,temp3,j);

 camera.viewpoint.x = temp3.x;
 camera.viewpoint.y = temp3.y;
 camera.viewpoint.z = temp3.z;

 rotate_about_y(light_orig,temp2,j);
 lightsource.x = temp2.x;
 lightsource.y = temp2.y;
 lightsource.z = temp2.z;




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

//  for(i=0;i<200;i++)
//   memcpy(buffer+i*320,texturemap+i*256,256);

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

  if(frames == 626)
  {
   getch();
   done = 1;
  }
  else
  {
  for(i=0;i<object[0].num_vertices;i++)
  {
   object[0].local_vertices[i].x += inc[i].x;
   object[0].local_vertices[i].y += inc[i].y;
   object[0].local_vertices[i].z += inc[i].z;
  }
  for(i=0;i<object[0].num_polygons;i++)
  {
   object[0].polys[i].normal.x += v_inc[i].x;
   object[0].polys[i].normal.y += v_inc[i].y;
   object[0].polys[i].normal.z += v_inc[i].z;
  }

  }


// Wait_For_Vsync();
 memcpy(video_buffer,buffer,VIDEO_STORAGE);
 memset(buffer,0,VIDEO_STORAGE);
 end = clock();
 frames++;
// while((time_keep-old_time)<1){}

}


  Set_Text_Mode();
}
