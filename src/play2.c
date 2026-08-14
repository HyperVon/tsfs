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



int maxfps,runtime;
FILE *out;

unsigned char anti_aliased_buffer[64000];
OBJECT object[50],*object_ptr;

put_sprite(int x, int y, unsigned char *bmap, Fixed32 scale_factor)
{
 int sx,sy,c,s;
 int end_y,end_x,start_x,start_y;
 Fixed32 scale;
 Fixed32 bmap_x,bmap_y,map_inc;
 unsigned char *dest;

 if(scale_factor <= 32768)
  return;

 scale = fixeddiv(16777216,scale_factor);

 bmap_x = bmap_y = 0;
 map_inc = fixeddiv(16777216,scale);
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
    c = bmap[(s<<8)+(bmap_x>>16)]; 
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



int load_world(OBJECT *object, unsigned char *pal,char *name)
{
        int num_objects,tmp,i,m,shade_type,j;
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


generate_spline_path(PATH *path_ptr, float num_frames)
{
 float points_per_segment = num_frames/(float)path_ptr->num_points;
 int     i,n,count;
 float   j,t,t2,t3,frames;
 float   x,y,z;
 float   h[4];


 keys = (Key *) malloc (sizeof(Key)*path_ptr->num_points);

 printf("Points per segment: %f\n",points_per_segment);
 frames=0;
 count=0;
 for(i=0;i<path_ptr->num_points;i++)
 {

   // Frm.  X   Y   Z   t  c  b  et ef
   SetKey(&keys[i],frames,path_ptr->control_points[i].x/65536.0,
                          path_ptr->control_points[i].y/65536.0,
                          path_ptr->control_points[i].z/65536.0,
                          0, 0, 0, 0, 0 );
   frames+=points_per_segment;
 }
 MAX = path_ptr->num_points;

	if (MAX < 2)
        {
         printf("Only 2 (or less) points in control path. NOTHING TO DO!\n");
         exit(1);
        }
	if (MAX > 2)	{
		for (n = 1; n < MAX-1; n++ )	{
			CompDeriv( &keys[n-1],&keys[n],&keys[n+1] );
		}
	}
	CompDerivFirst( &keys[0],&keys[1],&keys[2] );
	CompDerivLast( &keys[MAX-3],&keys[MAX-2],&keys[MAX-1] );

        //do a quick pass to calc how many points in path
        for (n = 0; n < MAX-1; n++ )
        {
		frames = keys[n+1].frame - keys[n].frame;
                for (j = 0; j < frames; j++)
                 count++;
	}

        path_ptr->path = (POINT_3D *) malloc(sizeof(POINT_3D)*count);
        printf("Num points in path: %d\n",count);
        path_ptr->num_path_points = count;
        count=0;
	for (n = 0; n < MAX-1; n++ )	{
		frames = keys[n+1].frame - keys[n].frame;
		for (j = 0; j < frames; j++)	{
			t = j/frames;
			t = Ease( t,keys[n].easefrom,keys[n+1].easeto );
			t2 = t*t;												// t2 = t^2;
			t3 = t2*t;											// t3 = t^3;

			h[0] = 2*t3 - 3*t2 + 1;
			h[1] = -2*t3 + 3*t2;
			h[2] = t3 - 2*t2 + t;
			h[3] = t3 - t2;

             path_ptr->path[count].x = ((h[0]*keys[n].pos.x) + (h[1]*keys[n+1].pos.x) +
                                        (h[2]*keys[n].dd.x) + (h[3]*keys[n+1].ds.x))*65536.0;
             path_ptr->path[count].y = ((h[0]*keys[n].pos.y) + (h[1]*keys[n+1].pos.y) +
                                        (h[2]*keys[n].dd.y)+(h[3]*keys[n+1].ds.y))*65536.0;
             path_ptr->path[count].z = ((h[0]*keys[n].pos.z) + (h[1]*keys[n+1].pos.z) +
                                        (h[2]*keys[n].dd.z)+(h[3]*keys[n+1].ds.z))*65536.0;
             count++;
		}
	}

 free(keys);
}

generate_path(PATH *path_ptr, float num_frames)
{
    Fixed32 points_per_segment = (num_frames/(float)(path_ptr->num_points-1))*65536.0;
    int i,index,j;
    Fixed32 x_inc,y_inc,z_inc,x,y,z;

    path_ptr->path = (POINT_3D *) malloc (sizeof(POINT_3D)*num_frames);

    index=0;

    for(i=0;i<path_ptr->num_points-1;i++)
    {
	x_inc = fixeddiv(path_ptr->control_points[i+1].x - path_ptr->control_points[i].x,points_per_segment);
	y_inc = fixeddiv(path_ptr->control_points[i+1].y - path_ptr->control_points[i].y,points_per_segment);
	z_inc = fixeddiv(path_ptr->control_points[i+1].z - path_ptr->control_points[i].z,points_per_segment);

	x = path_ptr->control_points[i].x;
	y = path_ptr->control_points[i].y;
	z = path_ptr->control_points[i].z;
	

	for(j=1;j<points_per_segment/65536.0;j++)
	{
	    path_ptr->path[index].x = x;
	    path_ptr->path[index].y = y;
	    path_ptr->path[index].z = z;

	    x+=x_inc;
	    y+=y_inc;
	    z+=z_inc;
	    index++;
	}
    }
    printf("Num points in path: %d\n",index);
    path_ptr->num_path_points = index;
}

generate_path_light(PATH *path_ptr, float num_frames)
{
    Fixed32 points_per_segment = (num_frames/(float)(path_ptr->num_points-1))*65536.0;
    int i,index,j,mag;
    Fixed32 x_inc,y_inc,z_inc,x,y,z;

    path_ptr->path = (POINT_3D *) malloc (sizeof(POINT_3D)*num_frames);

    index=0;

    for(i=0;i<path_ptr->num_points-1;i++)
    {
	x_inc = fixeddiv(path_ptr->control_points[i+1].x - path_ptr->control_points[i].x,points_per_segment);
	y_inc = fixeddiv(path_ptr->control_points[i+1].y - path_ptr->control_points[i].y,points_per_segment);
	z_inc = fixeddiv(path_ptr->control_points[i+1].z - path_ptr->control_points[i].z,points_per_segment);

	x = path_ptr->control_points[i].x;
	y = path_ptr->control_points[i].y;
	z = path_ptr->control_points[i].z;
	

	for(j=1;j<points_per_segment/65536.0;j++)
	{
            mag = fixedsqrt(fixedmul(x,x)+fixedmul(y,y)+fixedmul(z,z));
            path_ptr->path[index].x = fixeddiv(x,mag);
            path_ptr->path[index].y = fixeddiv(y,mag);
            path_ptr->path[index].z = fixeddiv(z,mag);

	    x+=x_inc;
	    y+=y_inc;
	    z+=z_inc;
	    index++;
	}
    }
    printf("Num points in path: %d\n",index);
    path_ptr->num_path_points = index;
}


void copy_path(PATH *dest, PATH *source)
{
    int i;

    dest->control_points = (POINT_3D *) malloc (sizeof(POINT_3D)*source->num_points);
    dest->path   = (POINT_3D *) malloc (sizeof(POINT_3D)*source->num_path_points);
    dest->num_points = source->num_points;
    dest->num_path_points = source->num_path_points;

    for(i=0;i<source->num_points;i++)
    {
	dest->control_points[i].x = source->control_points[i].x;
	dest->control_points[i].y = source->control_points[i].y;
	dest->control_points[i].z = source->control_points[i].z;
    }

    for(i=0;i<source->num_path_points;i++)
    {
	dest->path[i].x = source->path[i].x;
	dest->path[i].y = source->path[i].y;
	dest->path[i].z = source->path[i].z;
    }
}


main(int argc, char *argv[])
{
 POINT_3D temp,temp2;
 POINT_3D_float lightsource_orgi;
 int done,i,j,k,l,m,num_objects;
 unsigned char pal[768],shade_type,*pix_ptr,rc,gc,bc;
 char t,text_buffer[80];
 int shade;
 float start,end,frames,s;
 FILE *path,*world,*fp;
 PATH flight_path,light_path,target_path;
 float r,g,b,rsum,gsum,bsum;
 int x,y;
 


 if(argc < 1)
 {
  printf("Usage: viewer <filename>\n");
  exit(1);
 }
 out = fopen("generic1.out","wt" );
 path = fopen(argv[2],"rb");
 world = fopen(argv[1],"rt");

 if (!world) { printf("File does not exist!\n");
               exit(1);
             }


 texturemap = (unsigned char *) malloc(5*65536*sizeof(unsigned char));

 num_objects=load_world(object,pal,argv[1]);

 fread(&i, sizeof(int), 1, path);

 flight_path.control_points = (POINT_3D *) malloc(sizeof(POINT_3D)*i);
 flight_path.num_points = i;

 target_path.control_points = (POINT_3D *) malloc(sizeof(POINT_3D)*i);
 target_path.num_points = i;

 light_path.control_points = (POINT_3D *) malloc(sizeof(POINT_3D)*i);
 light_path.num_points = i;


 for(i=0;i<target_path.num_points;i++)
 {
   fread(&flight_path.control_points[i], sizeof(POINT_3D), 1, path);
   fread(&target_path.control_points[i], sizeof(POINT_3D), 1, path);
   fread(&light_path.control_points[i], sizeof(POINT_3D), 1, path);
 }

 generate_spline_path(&flight_path,runtime*maxfps);
 generate_spline_path(&target_path,runtime*maxfps);
 generate_path_light(&light_path,runtime*maxfps);

 temp.x = 0;
 temp.y = 0;
 temp.z = 0;

/*
 set_sprite3d(&object[num_objects],
              flight_path.path[flight_path.num_path_points-75],
              temp,
              1,NON_ANIMATED);
 num_objects++;
*/
 fp = fopen("shade.lut","rb");
 fread(LUT,sizeof(unsigned char),16384,fp);
 fclose(fp);


 fp = fopen("transpar.lut","rb");
 fread(TRANSPAR,sizeof(unsigned char),65536,fp);
 fclose(fp);


 buffer=VIDEO_BUFFER;
 memset(buffer,0,VIDEO_STORAGE);


 load_fontFN("6x6.fnt",6,6);
 
 printf("Press any key...\n");
 getch();

/* initialize graphics mode */
 create_lookup_tables();

 Set_Video_Mode();

 load_palette(pal);

 camera.viewpoint.x = flight_path.path[0].x;
 camera.viewpoint.y = flight_path.path[0].y;
 camera.viewpoint.z = flight_path.path[0].z;

 camera.lookat.x = target_path.path[0].x;
 camera.lookat.y = target_path.path[0].y;
 camera.lookat.z = target_path.path[0].z;

 lightsource.x = light_path.path[0].x;
 lightsource.y = light_path.path[0].y;
 lightsource.z = light_path.path[0].z;

 frames = j = done = 0;
 
 

 start = clock();
 //must do this to get first value for end
 end = clock();
 
while(!done)
 {

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
       else if(shade_type == SPRITE3D)
           put_sprite(master_facet_list[i]->quad[0].x,
                      master_facet_list[i]->quad[0].y,
                      texturemap,
                      master_facet_list[i]->vertices[0].z);
  
  }
 frames++;
 sprintf(text_buffer,"fps: %2.2f NP: %d",frames/((end-start)/100.0),num_polys);
 putstr(0,0,text_buffer,LEFT,230);

 if(frames < flight_path.num_path_points)
 {
  camera.viewpoint.x = flight_path.path[(int)frames].x;
  camera.viewpoint.y = flight_path.path[(int)frames].y;
  camera.viewpoint.z = flight_path.path[(int)frames].z;

  camera.lookat.x = target_path.path[(int)frames].x;
  camera.lookat.y = target_path.path[(int)frames].y;
  camera.lookat.z = target_path.path[(int)frames].z;

  lightsource.x = light_path.path[(int)frames].x;
  lightsource.y = light_path.path[(int)frames].y;
  lightsource.z = light_path.path[(int)frames].z;


 }
 else
  done=1;
 
 //copy double buffer to the screen
// Wait_For_Vsync();

 memcpy(video_buffer,buffer,64000);
 memset(buffer,0,VIDEO_STORAGE);
// memset(anti_aliased_buffer,0,VIDEO_STORAGE);

 end = clock();
}
  Set_Text_Mode();
  fclose(out);
  fclose(path);
}
