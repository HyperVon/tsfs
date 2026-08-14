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
#include "spline_2.c"
#include "anti.c"
#include "fire.c"
#include "constant.c"

OBJECT cointro[21],scene1[33];
unsigned char secondary_buffer[64000],cross_fade_table[9][65536];
unsigned char scene1_lut[16384],scene1_trans[65536];
char cross_fade_filenames[9][12] ={
"cross1.lut",
"cross2.lut",
"cross3.lut",
"cross4.lut",
"cross5.lut",
"cross6.lut",
"cross7.lut",
"cross8.lut",
"cross9.lut"};



put_sprite64x64(int x, int y, unsigned char *bmap, Fixed32 scale_factor)
{
 int sx,sy,c,s;
 int end_y,end_x,start_x,start_y;
 Fixed32 scale;
 Fixed32 bmap_x,bmap_y,map_inc;
 unsigned char *dest;

 if(scale_factor <= 32768)
  return;

 scale = fixeddiv(4194304,scale_factor);

 bmap_x = bmap_y = 0;
 map_inc = fixeddiv(4194304,scale);
 scale >>= 17;
 end_y =  y + scale - 1;
 start_x = x - scale;
 start_y = y - scale;
 end_x = x + scale - 1;
 dest = buffer + (start_y<<6)+(start_y<<8);

 for(sy = start_y;sy < end_y;sy++)
 {
  s = (bmap_y>>16);
  if(sy >=0 && sy <= 199)
   for(sx = start_x;sx < end_x;sx++)
   {
    if(sx >= 0 && sx <= 319)
    {
    c = firelut[bmap[(s<<6)+(bmap_x>>16)]]; 
    if(c!=bmap[0])
     *(dest+sx) = TRANSPAR[(c<<8) + *(dest+sx)];
    }
    bmap_x += map_inc;
   }
  dest+=320;
  bmap_x=0;
  bmap_y += map_inc;
 }
}


int load_world(OBJECT *object, unsigned char *pal,char *name,int *maxfps,int *runtime)
{
        int num_objects,tmp,i,m,shade_type,j;
        int transparent,R,G,B,cull_tag;
        static char filename[100];
        FILE *fp,*world;

        world = fopen(name,"rb");

        fscanf(world,"%d",runtime);
        fscanf(world,"%d",maxfps);
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



main(int argc, char *argv[])
{
 POINT_3D temp,temp2;
 int done,i,j,k,l,m;
 unsigned char shade_type,*sprite,*dest;
 char t,text_buffer[80];
 int shade,sprite_count;
 float start,end,frames;
 FILE *path,*fp;
 int x,y,sx,sy,sprite_intensity,sprite_intensity_inc;
 OBJECT *object_ptr;
 int cointro_fps,cointro_runtime,cointro_num_objects;
 unsigned char pal_cointro[768],*cointro_tmaps;
 PATH cointro_flight_path,cointro_light_path,cointro_target_path;

 int scene1_fps,scene1_runtime,scene1_num_objects;
 unsigned char pal_scene1[768],*scene1_tmaps,*save_buffer_ptr;
 PATH scene1_flight_path,scene1_light_path,scene1_target_path;
 PATH secondary_flight_path,secondary_target_path;
 int cross_fade_count,save_num_target_path_points,save_num_flight_path_points;
 POINT_3D *save_flight_path_ptr,*save_target_path_ptr;
 int cross_fade_table_index,cross_fade;

 /******  COINTRO STUFF *****/

 texturemap = (unsigned char *) malloc(4*65536*sizeof(unsigned char));
 cointro_num_objects=load_world(cointro,pal_cointro,"cointro.wld",&cointro_fps,&cointro_runtime);
 cointro_tmaps=texturemap;
 sprite = cointro_tmaps+3*65536;

 path = fopen("cointro.pth","rb");
 fread(&i, sizeof(int), 1, path);

 cointro_flight_path.control_points = (POINT_3D *) malloc(sizeof(POINT_3D)*i);
 cointro_flight_path.num_points = i;

 cointro_target_path.control_points = (POINT_3D *) malloc(sizeof(POINT_3D)*i);
 cointro_target_path.num_points = i;

 cointro_light_path.control_points = (POINT_3D *) malloc(sizeof(POINT_3D)*i);
 cointro_light_path.num_points = i;

 for(i=0;i<cointro_target_path.num_points;i++)
 {
   fread(&cointro_flight_path.control_points[i], sizeof(POINT_3D), 1, path);
   fread(&cointro_target_path.control_points[i], sizeof(POINT_3D), 1, path);
   fread(&cointro_light_path.control_points[i], sizeof(POINT_3D), 1, path);
 }

 generate_spline_path(&cointro_flight_path,cointro_runtime*cointro_fps);
 generate_spline_path(&cointro_target_path,cointro_runtime*cointro_fps);
 generate_path_light(&cointro_light_path,cointro_runtime*cointro_fps);
 fclose(path);

 temp2.x = 7*65536;
 temp2.y = 2.1*65536;
 temp2.z = -8*65536;

 temp.x = 0;
 temp.y = 0;
 temp.z = 0;

 set_sprite3d(&cointro[cointro_num_objects],
              temp,
              temp2,
              0,NON_ANIMATED);
 cointro[cointro_num_objects].polys[0].texturemap = firemap;
 cointro_num_objects++;

 temp2.x = -3*65536;
 temp2.y = 2.1*65536;
 temp2.z = -8*65536;

 set_sprite3d(&cointro[cointro_num_objects],
              temp,
              temp2,
              0,NON_ANIMATED);
 cointro[cointro_num_objects].polys[0].texturemap = firemap;
 cointro_num_objects++;

 fp = fopen("shade.lut","rb");
 fread(LUT,sizeof(unsigned char),16384,fp);
 fclose(fp);


 fp = fopen("transpar.lut","rb");
 fread(TRANSPAR,sizeof(unsigned char),65536,fp);
 fclose(fp);

 Make_Fire_Palette(pal);

 /******* COINTRO INIT DONE *****/

 /******* SCENE1 STUFF ******/
 texturemap = (unsigned char *) malloc(4*65536*sizeof(unsigned char));
 scene1_num_objects=load_world(scene1,pal_scene1,"scene1.wld",&scene1_fps,&scene1_runtime);
 scene1_tmaps = texturemap;
 path = fopen("scene1a.pth","rb");
 fread(&i, sizeof(int), 1, path);

 scene1_flight_path.control_points = (POINT_3D *) malloc(sizeof(POINT_3D)*i);
 scene1_flight_path.num_points = i;

 scene1_target_path.control_points = (POINT_3D *) malloc(sizeof(POINT_3D)*i);
 scene1_target_path.num_points = i;

 scene1_light_path.control_points = (POINT_3D *) malloc(sizeof(POINT_3D)*i);
 scene1_light_path.num_points = i;


 for(i=0;i<scene1_target_path.num_points;i++)
 {
   fread(&scene1_flight_path.control_points[i], sizeof(POINT_3D), 1, path);
   fread(&scene1_target_path.control_points[i], sizeof(POINT_3D), 1, path);
   fread(&scene1_light_path.control_points[i], sizeof(POINT_3D), 1, path);
 }
 fclose(path);
 generate_spline_path(&scene1_flight_path,scene1_runtime*scene1_fps);
 generate_spline_path(&scene1_target_path,scene1_runtime*scene1_fps);
 generate_path_light(&scene1_light_path,scene1_runtime*scene1_fps);

 path = fopen("scene1b.pth","rb");

 fread(&i, sizeof(int), 1, path);

 secondary_flight_path.control_points = (POINT_3D *) malloc(sizeof(POINT_3D)*i);
 secondary_flight_path.num_points = i;

 secondary_target_path.control_points = (POINT_3D *) malloc(sizeof(POINT_3D)*i);
 secondary_target_path.num_points = i;


 //read in only the flight path from the second path on command line
 for(i=0;i<secondary_flight_path.num_points;i++)
 {
   fread(&secondary_flight_path.control_points[i], sizeof(POINT_3D), 1, path);
   fread(&secondary_target_path.control_points[i], sizeof(POINT_3D), 1, path);
   fread(NULL, sizeof(POINT_3D), 1, path);
 }
 fclose(path);
 generate_spline_path(&secondary_flight_path,scene1_runtime*scene1_fps);
 generate_spline_path(&secondary_target_path,scene1_runtime*scene1_fps);

 fp = fopen("shade1.lut","rb");
 fread(scene1_lut,sizeof(unsigned char),16384,fp);
 fclose(fp);


 fp = fopen("transpa1.lut","rb");
 fread(scene1_trans,sizeof(unsigned char),65536,fp);
 fclose(fp);

 //load cross luts
 for(i=0;i<9;i++)
 {
  printf("Loading %s\n",cross_fade_filenames[i]);
  fp = fopen(cross_fade_filenames[i],"rb");
  fread(cross_fade_table[i],sizeof(unsigned char),65536,fp);
  fclose(fp);
 }
 //init fixed32 x_angs' inc using z_ang as the var.. u shouldnt do this :)
 for(i=8;i<scene1_num_objects;i++)
 {
  scene1[i].z_ang = fixedmul(fixeddiv(65536,((rand()%4)<<16)),(5<<16));
  printf("%f\n",scene1[i].z_ang/65536.0);
 }
 save_flight_path_ptr = scene1_flight_path.path;
 save_target_path_ptr = scene1_target_path.path;
 save_num_flight_path_points = scene1_flight_path.num_path_points;
 save_num_target_path_points = scene1_target_path.num_path_points;

 /******* SCENE1 INIT DONE *****/

 buffer=VIDEO_BUFFER;
 memset(buffer,0,VIDEO_STORAGE);


 load_fontFN("6x6.fnt",6,6);
 
 printf("Press any key...\n");
 getch();

/* initialize graphics mode */
 create_lookup_tables();

 Set_Video_Mode();
 load_palette(pal);


 camera.viewpoint.x = cointro_flight_path.path[0].x;
 camera.viewpoint.y = cointro_flight_path.path[0].y;
 camera.viewpoint.z = cointro_flight_path.path[0].z;

 camera.lookat.x = cointro_target_path.path[0].x;
 camera.lookat.y = cointro_target_path.path[0].y;
 camera.lookat.z = cointro_target_path.path[0].z;

 lightsource.x = cointro_light_path.path[0].x;
 lightsource.y = cointro_light_path.path[0].y;
 lightsource.z = cointro_light_path.path[0].z;

 frames = j = done = 0;
 sprite_intensity = 0;
 sprite_intensity_inc = 2;
 sprite_count=0;

 start = clock();
 //must do this to get first value for end
 end = clock();

 
while(!done)
 {
 fire();
/* for(k=0;k<num_objects;k++)
 {
  cointro[k].x_ang+=1;
  if(cointro[k].x_ang>=180) cointro[k].x_ang = 0;

//  cointro[k].y_ang+=1;
//  if(cointro[k].y_ang>=180) cointro[k].y_ang = 0;

  cointro[k].z_ang+=1;
  if(cointro[k].z_ang>=90) cointro[k].z_ang = 0;
 }
 */
 for (k=0;k < cointro_num_objects; k++)
  if(cointro[k].animation == ANIMATED)
   animate_object(&cointro[k]);

  //reset the polygon list to 0 polys
  num_polys=0;

  create_world_to_cam();

  //put the floor first which is object[0]
//  for(k=0;k<cointro_num_objects;k++)
k=0;
  if(!remove_object(&cointro[k]))
  {
    object_ptr = &cointro[k];

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
       else if(shade_type == SPRITE3D)
           put_sprite(master_facet_list[i]->quad[0].x,
                      master_facet_list[i]->quad[0].y,
                      texturemap,
                      master_facet_list[i]->vertices[0].z);
  
  }
  num_polys=0;
  for(k=1;k<cointro_num_objects;k++)
  if(!remove_object(&cointro[k]))
  {
    object_ptr = &cointro[k];

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
       else if(shade_type == SPRITE3D)
           put_sprite64x64(master_facet_list[i]->quad[0].x,
                      master_facet_list[i]->quad[0].y,
                      fire_buffer,
                      master_facet_list[i]->vertices[0].z);
       else if (shade_type == CONSTANT)
           constant(master_facet_list[i]->quad[0].x<<16,
                master_facet_list[i]->quad[0].y<<16,
                master_facet_list[i]->quad[1].x<<16,
                master_facet_list[i]->quad[1].y<<16,
                master_facet_list[i]->quad[2].x<<16,
                master_facet_list[i]->quad[2].y<<16,
                master_facet_list[i]->color,
                master_facet_list[i]->transparent);
  }

 frames++;
 sprintf(text_buffer,"fps: %2.2f NP: %d",frames/((end-start)/100.0),num_polys);
 putstr(0,0,text_buffer,LEFT,230);

 if(frames < cointro_flight_path.num_path_points)
 {
  camera.viewpoint.x = cointro_flight_path.path[(int)frames].x;
  camera.viewpoint.y = cointro_flight_path.path[(int)frames].y;
  camera.viewpoint.z = cointro_flight_path.path[(int)frames].z;

  camera.lookat.x = cointro_target_path.path[(int)frames].x;
  camera.lookat.y = cointro_target_path.path[(int)frames].y;
  camera.lookat.z = cointro_target_path.path[(int)frames].z;

  lightsource.x = cointro_light_path.path[(int)frames].x;
  lightsource.y = cointro_light_path.path[(int)frames].y;
  lightsource.z = cointro_light_path.path[(int)frames].z;


 }
 else
  done=1;

 if(sprite_count != 3)
 {
 dest = buffer + 24032;
 for(sy=75;sy<125;sy++)
 {
  y=((sy-75)<<8)-32;
  for(sx=32;sx<288;sx++)
  if(sprite[y+sx]!=sprite[0])
    *dest++ = TRANSPAR[(LUT[(sprite[y+sx]<<6)+sprite_intensity]<<8) + *dest];
   else
    dest++;
  dest+=64;
 }
 sprite_intensity+=sprite_intensity_inc;
 if(sprite_intensity >= 63)
 {
  sprite_intensity_inc = -2;
  sprite_intensity = 63;
 }
 if(sprite_intensity <= 0)
 {
  sprite+=12800;
  sprite_intensity=0;
  sprite_intensity_inc = 2;
  sprite_count++;
 }
 }
 
 memcpy(video_buffer,buffer,64000);
 memset(buffer,0,64000);


 end = clock();
}
 frames = j = done = 0;
// texturemap = scene1_tmaps;
 memcpy(TRANSPAR,scene1_trans,65536);
 memcpy(LUT,scene1_lut,16384);
 load_palette(scene1_pal);
 cross_fade_table_index = cross_fade=0;
 cross_fade_count=0;

while(!done)
 {
  if(kbhit())
   if(getch() == 'c')
     cross_fade = 1;

 for(k=0;k<scene1_num_objects-25;k++)
 {
  scene1[k].x_ang+=3;
  if(scene1[k].x_ang>=360) scene1[k].x_ang = 0;

  scene1[k].y_ang+=3;
  if(scene1[k].y_ang>=360) scene1[k].y_ang = 0;

  scene1[k].z_ang+=3;
  if(scene1[k].z_ang>=360) scene1[k].z_ang = 0;
 }


 for(k=8;k<scene1_num_objects;k++)
 {
  scene1[k].world_pos.x = sine[(scene1[k].x_ang>>16)];
  scene1[k].x_ang+=scene1[k].z_ang;
  if(scene1[k].x_ang>=(360<<16)) scene1[k].x_ang = 0;
 }


 for (k=0;k < scene1_num_objects; k++)
  if(scene1[k].animation == ANIMATED)
   animate_object(&scene1[k]);

  //reset the polygon list to 0 polys
  num_polys=0;

  create_world_to_cam();

  //normal render from first path
  for(k=0;k<scene1_num_objects;k++)
  if(!remove_object(&scene1[k]))
  {
    object_ptr = &scene1[k];

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
       else if(shade_type == SPRITE3D)
           put_sprite(master_facet_list[i]->quad[0].x,
                      master_facet_list[i]->quad[0].y,
                      texturemap,
                      master_facet_list[i]->vertices[0].z);
  
  }
  //if cross_fade is on then do another render this time to secondary_buffer

  if(cross_fade)
  {
   num_polys=0;
   save_buffer_ptr = buffer;
   buffer = secondary_buffer;
   //get next vp and lat from secondary_path
  if(cross_fade_count % 2 ==0)
  {
  camera.viewpoint.x = secondary_flight_path.path[(int)frames].x;
  camera.viewpoint.y = secondary_flight_path.path[(int)frames].y;
  camera.viewpoint.z = secondary_flight_path.path[(int)frames].z;

  camera.lookat.x = secondary_target_path.path[(int)frames].x;
  camera.lookat.y = secondary_target_path.path[(int)frames].y;
  camera.lookat.z = secondary_target_path.path[(int)frames].z;
  }
  else
  {
   camera.viewpoint.x = save_flight_path_ptr[(int)frames].x;
   camera.viewpoint.y = save_flight_path_ptr[(int)frames].y;
   camera.viewpoint.z = save_flight_path_ptr[(int)frames].z;

   camera.lookat.x = save_target_path_ptr[(int)frames].x;
   camera.lookat.y = save_target_path_ptr[(int)frames].y;
   camera.lookat.z = save_target_path_ptr[(int)frames].z;
  }
  create_world_to_cam();

  //normal render from first path
  for(k=0;k<scene1_num_objects;k++)
  if(!remove_object(&scene1[k]))
  {
    object_ptr = &scene1[k];

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
       else if(shade_type == SPRITE3D)
           put_sprite(master_facet_list[i]->quad[0].x,
                      master_facet_list[i]->quad[0].y,
                      texturemap,
                      master_facet_list[i]->vertices[0].z);
  
  }
  buffer = save_buffer_ptr;
  i = cross_fade_table_index>>1;
  for(k=0;k<64000;k++)
   buffer[k] = cross_fade_table[i][(buffer[k]<<8)+secondary_buffer[k]];
  memset(secondary_buffer,0,64000);
  cross_fade_table_index++;
  if(cross_fade_table_index == 18)
  {
   cross_fade = 0;
   cross_fade_table_index = 0;


   if(cross_fade_count % 2 == 0)
   {
    scene1_flight_path.path = secondary_flight_path.path;
    scene1_flight_path.num_path_points = secondary_flight_path.num_path_points;
    scene1_target_path.path = secondary_target_path.path;
    scene1_target_path.num_path_points = secondary_target_path.num_path_points;
   }
   else
   {
    scene1_flight_path.path = save_flight_path_ptr;
    scene1_flight_path.num_path_points = save_num_flight_path_points;
    scene1_target_path.path = save_target_path_ptr;
    scene1_target_path.num_path_points = save_num_target_path_points;
   }
   cross_fade_count++;
  }
 }//end if cross_fade

 frames++;
 sprintf(text_buffer,"fps: %2.2f NP: %d",frames/((end-start)/100.0),num_polys);
 putstr(0,0,text_buffer,LEFT,230);

 if(frames < flight_path.num_path_points)
 {
  camera.viewpoint.x = scene1_flight_path.path[(int)frames].x;
  camera.viewpoint.y = scene1_flight_path.path[(int)frames].y;
  camera.viewpoint.z = scene1_flight_path.path[(int)frames].z;

  camera.lookat.x = scene1_target_path.path[(int)frames].x;
  camera.lookat.y = scene1_target_path.path[(int)frames].y;
  camera.lookat.z = scene1_target_path.path[(int)frames].z;

  lightsource.x = scene1_light_path.path[(int)frames].x;
  lightsource.y = scene1_light_path.path[(int)frames].y;
  lightsource.z = scene1_light_path.path[(int)frames].z;
 }
 else
  done=1;
 
 //copy double buffer to the screen
// Wait_For_Vsync();

 memcpy(video_buffer,buffer,64000);
 if(!cross_fade)
  memset(buffer,0,VIDEO_STORAGE);

 end = clock();
}

  Set_Text_Mode();
}
