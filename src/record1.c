#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cotypes.h"
#include "fixed32.h"
#include "pcx.h"
#include "co3de.h"
#include "fonts.h"

#include "diff2c.c"

FILE *out;

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




main(int argc, char *argv[])
{
 POINT_3D temp,temp2,temp1,temp3;
 POINT_3D_float lightsource_orgi;
 int current_object_lookat=0;
 int done,i,j,k,l,m,num_objects,num_points=0;
 unsigned char *buf1,pal[768],shade_type;
 char t,text_buffer[80];
 int shade;
 char load_filename[80];
 float start,end,frames;
 OBJECT object[10],*object_ptr;
 FILE *path,*world,*tmp;
 fpos_t filepos;

 if(argc < 1)
 {
  printf("Usage: viewer <filename>\n");
  exit(1);
 }
 path = fopen("temp.pth","wb");
 world = fopen(argv[1],"rt");

 if (!world) { printf("File does not exist!\n");
               exit(1);
             }

 texturemap = (unsigned char *) malloc(5*65536*sizeof(unsigned char));
 if(texturemap == NULL)
 {
     printf("no mem avail\n");
     exit(1);
 }

 num_objects=load_world(object,pal,argv[1]);

 printf("\nGenerate look up tables?(y/n): ");
 t = getch();

 while(t != 'y' && t != 'n' && t != 'N' && t != 'Y')
 {
   printf("Generate look up tables?(y/n): ");
   t = getch();
 }


 if(t == 'y' || t=='Y')
 {
  printf("Making shade lookup...\n");
  make_shade_lut(pal);

  printf("Making transparent lookup...\n");
  make_trans_lut(pal,.4);
 }
 else
 {
  printf("Enter shade lut filename: ");
  scanf("%s",text_buffer);

  printf("Loading shade lookup...\n");
  tmp = fopen(text_buffer,"rb");
  if(tmp == NULL)
  {
   printf("%s doesnt exist\n",text_buffer);
   exit(1);
  }
  fread(LUT,sizeof(unsigned char),16384,tmp);
  fclose(tmp);

  printf("Enter transpar lut filename: ");
  scanf("%s",text_buffer);
  tmp = fopen(text_buffer,"rb");
  if(tmp == NULL)
  {
   printf("%s doesnt exist\n",text_buffer);
   exit(1);
  }

  printf("Loading transparent lookup...\n");
  fread(TRANSPAR,sizeof(unsigned char),65536,tmp);
  fclose(tmp);
 }

 buf1=VIDEO_BUFFER;
 memset(buf1,0,VIDEO_STORAGE);
 buffer = buf1;

 load_fontFN("6x6.fnt",6,6);
 
 printf("Press any key...\n");
 getch();

/* initialize graphics mode */
 create_lookup_tables();
 
 Set_Video_Mode();

 load_palette(pal);

 camera.viewpoint.x = 0;
 camera.viewpoint.y = 0;
 camera.viewpoint.z = -int_to_fixed(100);

 camera.lookat.x = 0;
 camera.lookat.y = 0;
 camera.lookat.z = 0;

 lightsource_orgi.x = 0;
 lightsource_orgi.y = 0;
 lightsource_orgi.z = -1;

 lightsource.x = 0;
 lightsource.y = 0;
 lightsource.z = int_to_fixed(-1);

 frames = j = done = 0;
 
 start = clock();
 //must do this to get first value for end
 end = clock();
 
while(!done)
 {

  if(kbhit())
  {
      t = getch();
      switch(t)
      {
	  case '5':
		 camera.viewpoint.x = 0;
		 camera.viewpoint.y = 0;
		 camera.viewpoint.z = int_to_fixed(-10);

		 camera.lookat.x = 0;
		 camera.lookat.y = 0;
		 camera.lookat.z = 0;

		 lightsource_orgi.x = 0;
		 lightsource_orgi.y = 0;
		 lightsource_orgi.z = -1;

		 lightsource.x = 0;
		 lightsource.y = 0;
		 lightsource.z = fixed_to_int(-1);
		 AMBIENCE=5;
                 AMBIENCE266=(5<<6);
		 
		 break;
	  case '/': AMBIENCE--;
                    AMBIENCE266-=64;
	  	    if(AMBIENCE<0)
                    {
                     AMBIENCE=0;
                     AMBIENCE266=0;
                    }
		    break;
   	  case '*': AMBIENCE++;
                    AMBIENCE266+=64;
	  	    if(AMBIENCE>63)
                    {
                     AMBIENCE=63;
                     AMBIENCE266=(5<<6);
                    }
		    break;
	  case '7': lightsource_orgi.x-=.1;
	           break;
	  case '9': lightsource_orgi.x+=.1;
	  	   break;
	  case '1': lightsource_orgi.y-=.1;
	  	   break;
	  case '3': lightsource_orgi.y+=.1;
	  	   break;
	  case '0': lightsource_orgi.z-=.1;
	  	   break;
	  case '.': lightsource_orgi.z+=.1;
	  	   break;
	  case '8': camera.viewpoint.y+=int_to_fixed(1);
	  break;
          case '2': camera.viewpoint.y-=int_to_fixed(1);
	  break;
	  case '4': camera.viewpoint.x-=int_to_fixed(1);
	  break;
	  case '6': camera.viewpoint.x+=int_to_fixed(1);
	  break;
	  case '-': camera.viewpoint.z-=int_to_fixed(1);
	  break;
	  case '+': camera.viewpoint.z+=int_to_fixed(1);
	  break;
	  case 'd': done = 1;                    
	  break;
	  case 'a': camera.lookat.x-=_1;
	  break;
	  case 's': camera.lookat.x+=_1;
	  break;
	  case 'w': camera.lookat.y+=_1;
	  break;
	  case 'z': camera.lookat.y-=_1;
	  break;
          case 'e': camera.lookat.z+=_1;
          break;
          case 'c': camera.lookat.z-=_1;
          break;
          case 'r': fwrite(&camera.viewpoint, sizeof(POINT_3D), 1, path);
                    fwrite(&camera.lookat, sizeof(POINT_3D), 1, path);
                      normalize(&lightsource_orgi);

                      lightsource.x = float_to_fixed(lightsource_orgi.x);
                      lightsource.y = float_to_fixed(lightsource_orgi.y);
                      lightsource.z = float_to_fixed(lightsource_orgi.z);

                    fwrite(&lightsource, sizeof(POINT_3D), 1, path);

                    num_points++;
          break;

// SAVE PATH FILE

          case 'S': fclose(out);
                    fclose(path);

                    tmp = fopen("temp.pth","rb");
                    out = fopen(argv[2],"wb");

                fwrite(&num_points, sizeof(int), 1, out);

                for (i=0;i<num_points;i++)
                  {
                        fread(&temp1, sizeof(POINT_3D), 1, tmp);
                        fread(&temp2, sizeof(POINT_3D), 1, tmp);
                        fread(&temp3, sizeof(POINT_3D), 1, tmp);
                        fwrite(&temp1, sizeof(POINT_3D), 1, out);
                        fwrite(&temp2, sizeof(POINT_3D), 1, out);
                        fwrite(&temp3, sizeof(POINT_3D), 1, out);
                  }

                  fclose(out);
                  fclose(tmp);


                  path = fopen("temp.pth","wb");
                  tmp = fopen(argv[2],"rb");

                  fread(&i, sizeof(int), 1, tmp);

                  for (i=0;i<num_points;i++)
                  {
                        fread(&temp1, sizeof(POINT_3D), 1, tmp);
                        fread(&temp2, sizeof(POINT_3D), 1, tmp);
                        fread(&temp3, sizeof(POINT_3D), 1, tmp);
                        fwrite(&temp1, sizeof(POINT_3D), 1, path);
                        fwrite(&temp2, sizeof(POINT_3D), 1, path);
                        fwrite(&temp3, sizeof(POINT_3D), 1, path);
                  }

                  fclose(tmp);

          break;

          case 'o': current_object_lookat++;
                    current_object_lookat%=num_objects;
                    camera.lookat.x=object[current_object_lookat].world_pos.x;
                    camera.lookat.y=object[current_object_lookat].world_pos.y;
                    camera.lookat.z=object[current_object_lookat].world_pos.z;
          break;

          case 'L': Set_Text_Mode();
                    printf("Filename: ");
                    scanf("%s",load_filename);

                    fclose(out);
                    fclose(tmp);
                    fclose(path);

                    tmp = fopen(load_filename,"rb");
                    out = fopen("temp.pth","wb");

                    fread(&num_points, sizeof(int), 1, tmp);

                    for(i=0; i<num_points; i++)
                    {
                        fread(&camera.viewpoint, sizeof(POINT_3D), 1, tmp);
                        fread(&camera.lookat, sizeof(POINT_3D), 1, tmp);
                        fread(&lightsource, sizeof(POINT_3D), 1, tmp);
                        fwrite(&camera.viewpoint, sizeof(POINT_3D), 1, out);
                        fwrite(&camera.lookat, sizeof(POINT_3D), 1, out);
                        fwrite(&lightsource, sizeof(POINT_3D), 1, out);
                   }
                        
                   fgetpos(out,&filepos);
                   fclose(out);
                   fclose(tmp);
                   
                   fsetpos(path, &filepos);

                   Set_Video_Mode();
                   load_palette(pal);

          break;
                    

      }
          normalize(&lightsource_orgi);

      lightsource.x = float_to_fixed(lightsource_orgi.x);
      lightsource.y = float_to_fixed(lightsource_orgi.y);
      lightsource.z = float_to_fixed(lightsource_orgi.z);
  }
  j+=3;
  if(j>=360) j=0;
  
//  rotate_about_y(lightsource_orgi,lightsource,j);
for (k=0;k < num_objects; k++)
 if(object[k].animation == ANIMATED)
  for(i=0;i<object[k].num_vertices;i++)
  {
      temp.x = object[k].local_vertices[i].x;
      temp.y = object[k].local_vertices[i].y;
      temp.z = object[k].local_vertices[i].z;

      rotate_about_x(temp,temp2,j);
      
      object[k].vertices_rot[i].x = temp2.x;
      object[k].vertices_rot[i].y = temp2.y;
      object[k].vertices_rot[i].z = temp2.z;

      temp.x = object[k].vertex_normal[i].x;
      temp.y = object[k].vertex_normal[i].y;
      temp.z = object[k].vertex_normal[i].z;
      
      rotate_about_x(temp,temp2,j);

      object[k].vertex_normal_rot[i].x = temp2.x;
      object[k].vertex_normal_rot[i].y = temp2.y;
      object[k].vertex_normal_rot[i].z = temp2.z;

   }

for(k=0; k< num_objects; k++)
 if(object[k].animation == ANIMATED)
   for(i=0;i<object[k].num_polygons;i++)
   {
      temp.x = object[k].polys[i].normal.x;
      temp.y = object[k].polys[i].normal.y;
      temp.z = object[k].polys[i].normal.z;
      
      rotate_about_x(temp,temp2,j);

      object[k].polys[i].normal_rot.x = temp2.x;
      object[k].polys[i].normal_rot.y = temp2.y;
      object[k].polys[i].normal_rot.z = temp2.z;

  }

  //reset the polygon list to 0 polys
  num_polys=0;

  create_world_to_cam();

  for(k=0;k<num_objects;k++)
  if(!remove_object(&object[k]))
  {
    object_ptr = &object[k];

    world_to_cam(object_ptr);

    cull_and_shade(object_ptr);

    for(m=0;m<object_ptr->num_polygons;m++)
        if(object_ptr->camera_vertices[object_ptr->polys[m].a].z < clip_near_z &&
           object_ptr->camera_vertices[object_ptr->polys[m].b].z < clip_near_z &&
           object_ptr->camera_vertices[object_ptr->polys[m].c].z < clip_near_z)
	   object_ptr->polys[m].visible=0;

    //add the visible polys from this object to the polygon list
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
       
       if(shade_type == FLAT)
        flat(master_facet_list[i]->quad[0].x<<16,
             master_facet_list[i]->quad[0].y<<16,
             master_facet_list[i]->quad[1].x<<16,
             master_facet_list[i]->quad[1].y<<16,
             master_facet_list[i]->quad[2].x<<16,
             master_facet_list[i]->quad[2].y<<16,
             master_facet_list[i]->color,
             master_facet_list[i]->shade,
             master_facet_list[i]->transparent);
       else if(shade_type == ENVMAP)
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

  }
 frames++;
 sprintf(text_buffer,"fps: %2.2f NP: %d",frames/((end-start)/100.0),num_polys);
 putstr(0,0,text_buffer,LEFT,230);
 
 //copy double buffer to the screen
 memcpy(video_buffer,buffer,VIDEO_STORAGE);
 memset(buf1,0,VIDEO_STORAGE);
 end = clock();
}
  Set_Text_Mode();
  fclose(out);
  fclose(path);
  fclose(tmp);

  path = fopen(argv[2],"wb");
  out = fopen("temp.pth","rb");
  fwrite(&num_points, sizeof(int), 1, path);
  for (i=0;i<num_points;i++)
  {
        fread(&camera.viewpoint, sizeof(POINT_3D), 1, out);
        fread(&camera.lookat, sizeof(POINT_3D), 1, out);
        fread(&lightsource, sizeof(POINT_3D), 1, out);
        fwrite(&camera.viewpoint, sizeof(POINT_3D), 1, path);
        fwrite(&camera.lookat, sizeof(POINT_3D), 1, path);
        fwrite(&lightsource, sizeof(POINT_3D), 1, path);

  }
  fclose(out);
  fclose(path);
  fclose(tmp);


}
