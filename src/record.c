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
                fscanf(world,"%d",&object[i].clip_near_z);
                object[i].clip_near_z <<= 16;


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
 unsigned char pal[768],RED,GREEN;
 int shade,oldtime,shade_type,num_objects,info_screen=0,current_object=0,num_points=0;
 float start,end,frames;
 static OBJECT object[50],*object_ptr;
 FILE *fp,*path;


 texturemap = (unsigned char *) malloc (5*65536);

 if(argc != 3)
 {
  printf("usage: record  world pathfile\n");
  exit(1);
 }


 //INIT OBJECT
 RED = findRGB(32,0,0,pal);
 GREEN = findRGB(0,32,0,pal);

 m = findRGB(0,0,32,pal);

 num_objects = load_world(object,pal,argv[1]);

 fp = fopen("shade.lut","rb");
 fread(LUT,sizeof(unsigned char),16384,fp);
 fclose(fp);

 fp = fopen("transpar.lut","rb");
 fread(TRANSPAR,sizeof(unsigned char),65536,fp);
 fclose(fp);

 fp = fopen("temp.pth","wb");

 buffer=VIDEO_BUFFER;
 memset(buffer,0,VIDEO_STORAGE);

 load_fontFN("6x6.fnt",6,6);


/* initialize graphics mode */
 printf("Size of OBJECT: %d\n",sizeof(OBJECT));
 create_lookup_tables();
 printf("Press any key...\n");
 getch();
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
    case '4': camera.viewpoint.x-=65536/6;
              break;
    case '6': camera.viewpoint.x+=65536/6;
              break;
    case '8': camera.viewpoint.y+=65536/6;
              break;
    case '2': camera.viewpoint.y-=65536/6;
              break;
    case '+': camera.viewpoint.z+=65536/6;
              break;
    case '-': camera.viewpoint.z-=65536/6;
              break;
    case 'a': camera.lookat.x-=65536/6;
              break;
    case 's': camera.lookat.x+=65536/6;
              break;
    case 'w': camera.lookat.y+=65536/6;
              break;
    case 'z': camera.lookat.y-=65536/6;
              break;
    case '[': camera.lookat.z+=65536/6;
              break;
    case ']': camera.lookat.z-=65536/6;
              break;
    case '7': lightsource.x-=65536;
    normalize_fixed(&lightsource);

              break;
    case '9': lightsource.x+=65536;
    normalize_fixed(&lightsource);

              break;
    case '1': lightsource.y+=65536;
    normalize_fixed(&lightsource);

              break;
    case '3': lightsource.y-=65536;
    normalize_fixed(&lightsource);

              break;
    case '0': lightsource.z+=65536;
    normalize_fixed(&lightsource);

              break;
    case '.': lightsource.z-=65536;
    normalize_fixed(&lightsource);

              break;
    case 'i': if(info_screen == 0) info_screen=1;
              else info_screen = 0;
              break;
    case 'o': current_object++;
              if(current_object >= num_objects)
               current_object = 0;
              camera.lookat.x = object[current_object].world_pos.x;
              camera.lookat.y = object[current_object].world_pos.y;
              camera.lookat.z = object[current_object].world_pos.z;
              break;
   case 'r':  fwrite(&camera.viewpoint, sizeof(POINT_3D), 1, fp);
              fwrite(&camera.lookat, sizeof(POINT_3D), 1, fp);
              fwrite(&lightsource, sizeof(POINT_3D), 1, fp);
              num_points++;
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
                master_facet_list[i]->shade,
                master_facet_list[i]->transparent);
  }

  sprintf(text_buffer,"p: %d fps: %.2f",num_polys,(frames/((end-start)/100.0)));
  putstr(0,0,text_buffer,LEFT,RED);

  if(info_screen)
  {
   sprintf(text_buffer,"vp: <%.2f,%.2f,%.2f>",camera.viewpoint.x/65536.0,
                                              camera.viewpoint.y/65536.0,
                                              camera.viewpoint.z/65536.0);
   putstr(0,190,text_buffer,LEFT,GREEN);

   sprintf(text_buffer,"lat: <%.2f,%.2f,%.2f>",camera.lookat.x/65536.0,
                                               camera.lookat.y/65536.0,
                                               camera.lookat.z/65536.0);
   putstr(0,183,text_buffer,LEFT,GREEN);
  }

// Wait_For_Vsync();
 memcpy(video_buffer,buffer,VIDEO_STORAGE);
 memset(buffer,0,VIDEO_STORAGE);
 end = clock();
 frames++;
}


  Set_Text_Mode();
  fclose(fp);

  path = fopen(argv[2],"wb");
  fp = fopen("temp.pth","rb");
  fwrite(&num_points, sizeof(int), 1, path);
  for (i=0;i<num_points;i++)
  {
        fread(&camera.viewpoint, sizeof(POINT_3D), 1, fp);
        fread(&camera.lookat, sizeof(POINT_3D), 1, fp);
        fread(&lightsource, sizeof(POINT_3D), 1, fp);
        fwrite(&camera.viewpoint, sizeof(POINT_3D), 1, path);
        fwrite(&camera.lookat, sizeof(POINT_3D), 1, path);
        fwrite(&lightsource, sizeof(POINT_3D), 1, path);

  }
  fclose(fp);
  fclose(path);

}
