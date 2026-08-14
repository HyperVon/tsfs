

/////////////////////////////////////////////////////////////////////////
// Globals
///////////////

CAMERA camera;
matrix_4x4 camera_matrix;
unsigned char *mirror_buffer;
POLYGON *master_facet_list[MAX_POLYS_PER_FRAME];
POINT_3D lightsource;
Fixed32 clip_far_z = int_to_fixed(200),clip_near_z = float_to_fixed(0);
Fixed32 fov_width=float_to_fixed((160./250.)),fov_height=float_to_fixed( (100./200.)*(320./200.) );
Fixed32 fov_width_mirror=float_to_fixed((128./256.)),fov_height_mirror=float_to_fixed((128./256.));
long num_polys;
Fixed32 sine[360], cosine[360],
	_333333=float_to_fixed(.333333),
	_128=int_to_fixed(128),
        _7=float_to_fixed(.7111),
        __7 = (.7111)*64.0,
	_63=int_to_fixed(63),
        _200 = 200<<16,
        _150 = 150<<16,
        _250 = 250<<16,
        _300 = 300<<16,
        __63=(63<<6);


Fixed32 arccos266[512];
Fixed32 arccos[512];
Fixed32 AMBIENCE266=(8<<6);
int AMBIENCE=8;

Fixed32 camx1,camy1,camz1,
        camx2,camy2,camz2,
        camx3,camy3,camz3,
        cam1,cam2,cam3;




void normalize_fixed(POINT_3D *v)
{
 Fixed32 length;

 length = fixedsqrt(fixedsquare(v->x) +
                    fixedsquare(v->y) +
                    fixedsquare(v->z));
 v->x = fixeddiv(v->x,length);
 v->y = fixeddiv(v->y,length);
 v->z = fixeddiv(v->z,length);
}



//normal modules
/*
#include "flat.c"
#include "gouraud.c"    
#include "texture.c"    
#include "tmapflat.c"   
#include "tmapgour.c"   
*/

#ifdef MIRROR
//mirror modules
#include "gouraudm.c"   
#include "texturem.c"   
#include "mapflatm.c"   
#include "mapgourm.c"
#endif

	

/////////////////////////////////////////////////////////////////////////
// Matrix Code  
///////////////

void cross_product_fixed(VECTOR_3D v, VECTOR_3D w, VECTOR_3D *pl)
{
 Fixed32 length;
 
 //cross product
 pl->x = fixedmul(v.y,w.z) - fixedmul(w.y,v.z);
 pl->y = -( fixedmul(v.x,w.z) - fixedmul(w.x,v.z) );
 pl->z = fixedmul(v.x,w.y) - fixedmul(w.x,v.y);

 //normalize
 length = fixedsqrt(fixedsquare(pl->x)+
		    fixedsquare(pl->y)+
		    fixedsquare(pl->z));
 pl->x = fixeddiv(pl->x,length);
 pl->y = fixeddiv(pl->y,length);
 pl->z = fixeddiv(pl->z,length);
}

void cross_product_float(VECTOR_3D v, VECTOR_3D w, VECTOR_3D *pl)
{
 float length;
 
 //cross product
 pl->x = fixedmul(v.y,w.z) - fixedmul(w.y,v.z);
 pl->y = -( fixedmul(v.x,w.z) - fixedmul(w.x,v.z) );
 pl->z = fixedmul(v.x,w.y) - fixedmul(w.x,v.y);

 //normalize
 length = sqrt(pl->x/65536.0 * pl->x/65536.0 +
          pl->y/65536.0 * pl->y/65536.0 +
          pl->z/65536.0 * pl->z/65536.0);

 pl->x = ((pl->x/65536.0)/length)*65536.0;
 pl->y = ((pl->y/65536.0)/length)*65536.0;
 pl->z = ((pl->z/65536.0)/length)*65536.0;
}


void mat_mul3x3(matrix_3x3 A, matrix_3x3 B, matrix_3x3 ans)
{
 int i,j,k;
 Fixed32 sum;

 for(i=0;i<3;i++)
   for(j=0;j<3;j++)
   {
     sum = 0;

     for(k=0;k<3;k++)
      sum += fixedmul(A[i][k],B[k][j]);

     ans[i][j] = sum;
   }

}

void build_rot_matrix(int x_ang, int y_ang, int z_ang,matrix_3x3 result)
{
 matrix_3x3 A,B,C;

 //set matrix A to the idenity matrix
 memset(A,0,sizeof(Fixed32)*9);
 A[0][0] = A[1][1] = A[2][2] = 65536;

 memset(B,0,sizeof(Fixed32)*9);
 B[0][0] = B[1][1] = B[2][2] = 65536;

 //build x matrix first
 A[1][1] = cosine[x_ang];
 A[1][2] = sine[x_ang];
 A[2][1] = -sine[x_ang];
 A[2][2] = cosine[x_ang];

 //build y matrix next
 B[0][0] = cosine[y_ang];
 B[2][0] = -sine[y_ang];
 B[0][2] = sine[y_ang];
 B[2][2] = cosine[y_ang];

 //concatenate A,B together into C
 mat_mul3x3(A,B,C);

 //reset matrix A so we can use it again
 memset(A,0,sizeof(Fixed32)*9);
 A[0][0] = A[1][1] = A[2][2] = 65536;

 //build z matrix using A
 A[0][0] = cosine[z_ang];
 A[0][1] = sine[z_ang];
 A[1][0] = -sine[z_ang];
 A[1][1] = cosine[z_ang];

 //concatenate A,C together into res
 mat_mul3x3(A,C,result);
}


#define rotate_about_y(a,b,theta)\
 (b).x = fixedmul((a).x,cosine[(theta)]) - fixedmul((a).z,sine[(theta)]);\
 (b).y = (a).y;\
 (b).z = fixedmul((a).x,sine[(theta)]) + fixedmul((a).z,cosine[(theta)])

#define rotate_about_z(a,b, theta)\
 (b).x = fixedmul((a).x,cosine[theta]) - fixedmul((a).y,sine[theta]);\
 (b).y = fixedmul((a).x,sine[theta]) + fixedmul((a).y,cosine[theta]);\
 (b).z = (a).z

#define rotate_about_x(a,b,theta)\
 (b).x = (a).x;\
 (b).y = fixedmul((a).y,cosine[(theta)]) - fixedmul((a).z,sine[(theta)]);\
 (b).z = fixedmul((a).y,sine[(theta)]) + fixedmul((a).z,cosine[(theta)])



void animate_object(OBJECT *object)
{
 int i;
 POINT_3D temp1,temp2;

 for(i=0;i<object->num_vertices;i++)
 {
      temp1.x = object->local_vertices[i].x;
      temp1.y = object->local_vertices[i].y;
      temp1.z = object->local_vertices[i].z;

      rotate_about_x(temp1,temp2,object->x_ang);
      rotate_about_y(temp2,temp1,object->y_ang);
      rotate_about_z(temp1,temp2,object->z_ang);


      object->vertices_rot[i].x = temp2.x;
      object->vertices_rot[i].y = temp2.y;
      object->vertices_rot[i].z = temp2.z;
 }

 if(object->shade_type == GOURAUD ||
    object->shade_type == GOURTEX ||
    object->shade_type == ENVMAP_GOUR  ||
    object->shade_type == ENVMAP)
 for(i=0;i<object->num_vertices;i++)
 {
      temp1.x = object->vertex_normal[i].x;
      temp1.y = object->vertex_normal[i].y;
      temp1.z = object->vertex_normal[i].z;

      rotate_about_x(temp1,temp2,object->x_ang);
      rotate_about_y(temp2,temp1,object->y_ang);
      rotate_about_z(temp1,temp2,object->z_ang);


      object->vertex_normal_rot[i].x = temp2.x;
      object->vertex_normal_rot[i].y = temp2.y;
      object->vertex_normal_rot[i].z = temp2.z;
 }

 for(i=0;i<object->num_polygons;i++)
 {
      temp1.x = object->polys[i].normal.x;
      temp1.y = object->polys[i].normal.y;
      temp1.z = object->polys[i].normal.z;

      rotate_about_x(temp1,temp2,object->x_ang);
      rotate_about_y(temp2,temp1,object->y_ang);
      rotate_about_z(temp1,temp2,object->z_ang);
      
      object->polys[i].normal_rot.x = temp2.x;
      object->polys[i].normal_rot.y = temp2.y;
      object->polys[i].normal_rot.z = temp2.z;
 }
}


void create_world_to_cam()
{
    static VECTOR_3D U,V,N;
    Fixed32 dot,length;

    if(camera.viewpoint.x == camera.lookat.x &&
       camera.viewpoint.y == camera.lookat.y &&
       camera.viewpoint.z == camera.lookat.z)
       {
	   printf("ERROR: Viewpoint is equal to lookat point:\n");
	   printf("VP: %f %f %f\n",fixed_to_float(camera.viewpoint.x),
	   			   fixed_to_float(camera.viewpoint.y),
				   fixed_to_float(camera.viewpoint.z));

	   printf("LAT: %f %f %f\n",fixed_to_float(camera.lookat.x),
	   			   fixed_to_float(camera.lookat.y),
				   fixed_to_float(camera.lookat.z));
	   exit(1);
       }

    N.x = camera.lookat.x - camera.viewpoint.x;
    N.y = camera.lookat.y - camera.viewpoint.y;
    N.z = camera.lookat.z - camera.viewpoint.z;
    length=fixedsqrt(fixedsquare(N.x) +
		     fixedsquare(N.y) +
		     fixedsquare(N.z));
    N.x=fixeddiv(N.x,length);
    N.y=fixeddiv(N.y,length);
    N.z=fixeddiv(N.z,length);


    V.x = camera.lookat.x - camera.viewpoint.x;
    V.y = camera.lookat.y + _1 - camera.viewpoint.y;
    V.z = camera.lookat.z - camera.viewpoint.z;

    dot = fixedmul(V.x,N.x) + fixedmul(V.y,N.y) + fixedmul(V.z,N.z);
    V.x -= fixedmul(dot,N.x);
    V.y -= fixedmul(dot,N.y);
    V.z -= fixedmul(dot,N.z);
    length = fixedsqrt(fixedsquare(V.x) +
		       fixedsquare(V.y) +
		       fixedsquare(V.z));

    V.x = fixeddiv(V.x,length);
    V.y = fixeddiv(V.y,length);
    V.z = fixeddiv(V.z,length);

    cross_product_fixed(V,N,&U);

/*
    camera_matrix[0][0] = U.x;
    camera_matrix[1][0] = U.y;
    camera_matrix[2][0] = U.z;
    camera_matrix[0][1] = V.x;
    camera_matrix[1][1] = V.y;
    camera_matrix[2][1] = V.z;
    camera_matrix[0][2] = N.x;
    camera_matrix[1][2] = N.y;
    camera_matrix[2][2] = N.z;
*/
   camx1 = U.x; camy1 = U.y; camz1 = U.z;
   camx2 = V.x; camy2 = V.y; camz2 = V.z;
   camx3 = N.x; camy3 = N.y; camz3 = N.z;

/*    camera_matrix[3][0]*/cam1 = -fixedmul(U.x,camera.viewpoint.x) -
			   fixedmul(U.y,camera.viewpoint.y) -
			   fixedmul(U.z,camera.viewpoint.z);

/*    camera_matrix[3][1]*/cam2 = -fixedmul(V.x,camera.viewpoint.x) -
			   fixedmul(V.y,camera.viewpoint.y) -
			   fixedmul(V.z,camera.viewpoint.z);

/*    camera_matrix[3][2]*/cam3 = -fixedmul(N.x,camera.viewpoint.x) -
			   fixedmul(N.y,camera.viewpoint.y) -
			   fixedmul(N.z,camera.viewpoint.z);

/*    camx1 = camera_matrix[0][0];
    camy1 = camera_matrix[1][0];
    camz1 = camera_matrix[2][0];
    cam1 = camera_matrix[3][0] - fixedmul(camx1,camy1);
*/
    cam1 -= fixedmul(camx1,camy1);

/*    camx2 = camera_matrix[0][1];
    camy2 = camera_matrix[1][1];
    camz2 = camera_matrix[2][1];
    cam2 = camera_matrix[3][1] - fixedmul(camx2,camy2);
*/
    cam2 -= fixedmul(camx2,camy2);

/*
    camx3 = camera_matrix[0][2];
    camy3 = camera_matrix[1][2];
    camz3 = camera_matrix[2][2];
    cam3 = camera_matrix[3][2] - fixedmul(camx3,camy3);
*/
    cam3 -= fixedmul(camx3,camy3);

}



void world_to_cam(OBJECT *object)
{
    int i;
    Fixed32 worldx,worldy,worldz,
	    tempx,tempy,tempz,xy;
    
    worldx = object->world_pos.x;
    worldy = object->world_pos.y;
    worldz = object->world_pos.z;

    
    if(object->animation == ANIMATED)
    for(i=0;i<object->num_vertices;i++)
	{
        tempx = object->vertices_rot[i].x = object->vertices_rot[i].x + worldx;
        tempy = object->vertices_rot[i].y = object->vertices_rot[i].y + worldy;
        tempz = object->vertices_rot[i].z = object->vertices_rot[i].z + worldz;
	
	xy = fixedmul(tempx,tempy);
	
	object->camera_vertices[i].x =
		fixedmul(tempx + camy1,tempy + camx1) +
		fixedmul(tempz, camz1) +
		cam1 - xy;

	object->camera_vertices[i].y =
		fixedmul(tempx + camy2,tempy + camx2) +
		fixedmul(tempz, camz2) +
		cam2 - xy;
		
	object->camera_vertices[i].z =
		fixedmul(tempx + camy3,tempy + camx3) +
		fixedmul(tempz, camz3) +
		cam3 - xy;
	}
    else
    for(i=0;i<object->num_vertices;i++)
	{
        tempx = object->vertices_rot[i].x = object->local_vertices[i].x + worldx;
        tempy = object->vertices_rot[i].y = object->local_vertices[i].y + worldy;
        tempz = object->vertices_rot[i].z = object->local_vertices[i].z + worldz;
	
	xy = fixedmul(tempx,tempy);
	
	object->camera_vertices[i].x =
		fixedmul(tempx + camy1,tempy + camx1) +
		fixedmul(tempz, camz1) +
		cam1 - xy;

	object->camera_vertices[i].y =
		fixedmul(tempx + camy2,tempy + camx2) +
		fixedmul(tempz, camz2) +
		cam2 - xy;
		
	object->camera_vertices[i].z =
		fixedmul(tempx + camy3,tempy + camx3) +
		fixedmul(tempz, camz3) +
		cam3 - xy;
	}

}

void world_to_cam_special(OBJECT *object)
{
    int i;
    Fixed32 tempx,tempy,tempz,xy;
    
    
    for(i=0;i<object->num_vertices;i++)
	{
        tempx = object->vertices_rot[i].x;
        tempy = object->vertices_rot[i].y;
        tempz = object->vertices_rot[i].z;
	
	xy = fixedmul(tempx,tempy);
	
	object->camera_vertices[i].x =
		fixedmul(tempx + camy1,tempy + camx1) +
		fixedmul(tempz, camz1) +
		cam1 - xy;

	object->camera_vertices[i].y =
		fixedmul(tempx + camy2,tempy + camx2) +
		fixedmul(tempz, camz2) +
		cam2 - xy;
		
	object->camera_vertices[i].z =
		fixedmul(tempx + camy3,tempy + camx3) +
		fixedmul(tempz, camz3) +
		cam3 - xy;
	}
}





/////////////////////////////////////////////////////////////////////////
//  Vector 3d code
//////////////////
void cull_and_shade(OBJECT *object)
{
    int i,current_vert;
    Fixed32 temp1;
    VECTOR_3D viewvector;

    if(object->shade_type == SPRITE3D)
    {
     object->polys[0].visible=1;
     return;
    }

    //no cull if object is transparent
    if(object->animation == ANIMATED && !object->cull_tag)
    {
     if (object->shade_type == GOURAUD || object->shade_type == GOURTEX || object->shade_type == ENVMAP_GOUR)
	for(i=0;i<object->num_polygons;i++)
        {             
              object->polys[i].visible=1;
              current_vert = object->polys[i].a;
              if((temp1=(fixedmul(lightsource.x,object->vertex_normal_rot[current_vert].x) +
                         fixedmul(lightsource.y,object->vertex_normal_rot[current_vert].y) +
                         fixedmul(lightsource.z,object->vertex_normal_rot[current_vert].z))) > 0)
	      {
               object->polys[i].quad[0].i = arccos266[temp1>>7] + AMBIENCE266;
               if(object->polys[i].quad[0].i > __63) object->polys[i].quad[0].i = __63;
              }                         
	      else
               object->polys[i].quad[0].i = AMBIENCE266;
	
              current_vert = object->polys[i].b;
              if((temp1=(fixedmul(lightsource.x,object->vertex_normal_rot[current_vert].x) +
                         fixedmul(lightsource.y,object->vertex_normal_rot[current_vert].y) +
                         fixedmul(lightsource.z,object->vertex_normal_rot[current_vert].z))) > 0)
	      {
               object->polys[i].quad[1].i = arccos266[temp1>>7] + AMBIENCE266;
	       if(object->polys[i].quad[1].i > __63) object->polys[i].quad[1].i = __63;
	      }
	      else
               object->polys[i].quad[1].i = AMBIENCE266;
	
              current_vert = object->polys[i].c;
              if((temp1=(fixedmul(lightsource.x,object->vertex_normal_rot[current_vert].x) +
                         fixedmul(lightsource.y,object->vertex_normal_rot[current_vert].y) +
                         fixedmul(lightsource.z,object->vertex_normal_rot[current_vert].z))) > 0)
	      {
               object->polys[i].quad[2].i = arccos266[temp1>>7] + AMBIENCE266;
	       if(object->polys[i].quad[2].i > __63) object->polys[i].quad[2].i = __63;
	      }
	      else
               object->polys[i].quad[2].i = AMBIENCE266;
	}

       else if (object->shade_type == FLAT || object->shade_type == FLATTEX 
                || object->shade_type == CONSTANT)
	for(i=0;i<object->num_polygons;i++)
	{
	      object->polys[i].visible=1;
	      if((temp1=(fixedmul(lightsource.x,object->polys[i].normal_rot.x) +
		     fixedmul(lightsource.y,object->polys[i].normal_rot.y) +
		     fixedmul(lightsource.z,object->polys[i].normal_rot.z))) > 0)
	      {
               object->polys[i].shade =  arccos[temp1>>7] + AMBIENCE;
               if(object->polys[i].shade > 63) object->polys[i].shade = 63;
	      }
	      else
               object->polys[i].shade = AMBIENCE;
	}

       else if (object->shade_type == ENVMAP || object->shade_type == PHONG ||
                object->shade_type == TEXTUREMAP || object->shade_type == TEXTUREMAP2)
	for(i=0;i<object->num_polygons;i++)
		      object->polys[i].visible=1;

        return;
    }
    //else use object's non-animated info
    else if(object->animation == NON_ANIMATED && !object->cull_tag)
    {
     if (object->shade_type == GOURAUD || object->shade_type == GOURTEX || object->shade_type == ENVMAP_GOUR)
	for(i=0;i<object->num_polygons;i++)
	{
	      object->polys[i].visible=1;
              current_vert = object->polys[i].a;
              if((temp1=(fixedmul(lightsource.x,object->vertex_normal[current_vert].x) +
                         fixedmul(lightsource.y,object->vertex_normal[current_vert].y) +
                         fixedmul(lightsource.z,object->vertex_normal[current_vert].z))) > 0)
	      {
               object->polys[i].quad[0].i = arccos266[temp1>>7] + AMBIENCE266;
               if(object->polys[i].quad[0].i > __63) object->polys[i].quad[0].i = __63;
              }                         
	      else
               object->polys[i].quad[0].i = AMBIENCE266;

              current_vert = object->polys[i].b;
              if((temp1=(fixedmul(lightsource.x,object->vertex_normal[current_vert].x) +
                         fixedmul(lightsource.y,object->vertex_normal[current_vert].y) +
                         fixedmul(lightsource.z,object->vertex_normal[current_vert].z))) > 0)
	      {
               object->polys[i].quad[1].i = arccos266[temp1>>7] + AMBIENCE266;
	       if(object->polys[i].quad[1].i > __63) object->polys[i].quad[1].i = __63;
	      }
	      else
               object->polys[i].quad[1].i = AMBIENCE266;
	
              current_vert = object->polys[i].c;
              if((temp1=(fixedmul(lightsource.x,object->vertex_normal[current_vert].x) +
                     fixedmul(lightsource.y,object->vertex_normal[current_vert].y) +
                     fixedmul(lightsource.z,object->vertex_normal[current_vert].z))) > 0)
	      {
               object->polys[i].quad[2].i = arccos266[temp1>>7] + AMBIENCE266;
	       if(object->polys[i].quad[2].i > __63) object->polys[i].quad[2].i = __63;
	      }
	      else
               object->polys[i].quad[2].i = AMBIENCE266;
	}

       else if (object->shade_type == FLAT || object->shade_type == FLATTEX
                || object->shade_type == CONSTANT)
	for(i=0;i<object->num_polygons;i++)
	{
	      object->polys[i].visible=1;
              if((temp1=(fixedmul(lightsource.x,object->polys[i].normal.x) +
		     fixedmul(lightsource.y,object->polys[i].normal.y) +
		     fixedmul(lightsource.z,object->polys[i].normal.z))) > 0)
	      {
               object->polys[i].shade = arccos[temp1>>7] + AMBIENCE;
               if(object->polys[i].shade > 63) object->polys[i].shade = 63;
              }
	      else
               object->polys[i].shade = AMBIENCE;
	}

       else if (object->shade_type == ENVMAP || object->shade_type == PHONG ||
                object->shade_type == TEXTUREMAP || object->shade_type == TEXTUREMAP2)
	for(i=0;i<object->num_polygons;i++)
		      object->polys[i].visible=1;
      return; 
    }


    //else object isnt transparent
    if(object->animation == ANIMATED)
    {
     if (object->shade_type == GOURAUD || object->shade_type == GOURTEX || object->shade_type == ENVMAP_GOUR)
	for(i=0;i<object->num_polygons;i++)
        {             
             current_vert = object->polys[i].a;
             viewvector.x = camera.viewpoint.x - object->vertices_rot[current_vert].x;
             viewvector.y = camera.viewpoint.y - object->vertices_rot[current_vert].y;
             viewvector.z = camera.viewpoint.z - object->vertices_rot[current_vert].z;
	     object->polys[i].visible=0;
             if((fixedmul(viewvector.x,object->polys[i].normal_rot.x) +
                 fixedmul(viewvector.y,object->polys[i].normal_rot.y) +
                 fixedmul(viewvector.z,object->polys[i].normal_rot.z)) > 0 )
	     {
	      object->polys[i].visible=1;
              if((temp1=(fixedmul(lightsource.x,object->vertex_normal_rot[current_vert].x) +
                         fixedmul(lightsource.y,object->vertex_normal_rot[current_vert].y) +
                         fixedmul(lightsource.z,object->vertex_normal_rot[current_vert].z))) > 0)
	      {
               object->polys[i].quad[0].i = arccos266[temp1>>7] + AMBIENCE266;
               if(object->polys[i].quad[0].i > __63) object->polys[i].quad[0].i = __63;
              }                         
	      else
               object->polys[i].quad[0].i = AMBIENCE266;
	
              current_vert = object->polys[i].b;
              if((temp1=(fixedmul(lightsource.x,object->vertex_normal_rot[current_vert].x) +
                         fixedmul(lightsource.y,object->vertex_normal_rot[current_vert].y) +
                         fixedmul(lightsource.z,object->vertex_normal_rot[current_vert].z))) > 0)
	      {
               object->polys[i].quad[1].i = arccos266[temp1>>7] + AMBIENCE266;
	       if(object->polys[i].quad[1].i > __63) object->polys[i].quad[1].i = __63;
	      }
	      else
               object->polys[i].quad[1].i = AMBIENCE266;

              current_vert = object->polys[i].c;
              if((temp1=(fixedmul(lightsource.x,object->vertex_normal_rot[current_vert].x) +
                         fixedmul(lightsource.y,object->vertex_normal_rot[current_vert].y) +
                         fixedmul(lightsource.z,object->vertex_normal_rot[current_vert].z))) > 0)
	      {
               object->polys[i].quad[2].i = arccos266[temp1>>7] + AMBIENCE266;
	       if(object->polys[i].quad[2].i > __63) object->polys[i].quad[2].i = __63;
	      }
	      else
               object->polys[i].quad[2].i = AMBIENCE266;
	     }
	}

    


       else if (object->shade_type == FLAT || object->shade_type == FLATTEX
                || object->shade_type == CONSTANT)
	for(i=0;i<object->num_polygons;i++)
	{
             current_vert = object->polys[i].a;
             viewvector.x = camera.viewpoint.x - object->vertices_rot[current_vert].x;
             viewvector.y = camera.viewpoint.y - object->vertices_rot[current_vert].y;
             viewvector.z = camera.viewpoint.z - object->vertices_rot[current_vert].z;
	     object->polys[i].visible=0;
             if((fixedmul(viewvector.x,object->polys[i].normal_rot.x) +
                 fixedmul(viewvector.y,object->polys[i].normal_rot.y) +
                 fixedmul(viewvector.z,object->polys[i].normal_rot.z)) > 0 )
	     {
	      object->polys[i].visible=1;
	      if((temp1=(fixedmul(lightsource.x,object->polys[i].normal_rot.x) +
                         fixedmul(lightsource.y,object->polys[i].normal_rot.y) +
                         fixedmul(lightsource.z,object->polys[i].normal_rot.z))) > 0)
	      {
               object->polys[i].shade = arccos[temp1>>7] + AMBIENCE;
               if(object->polys[i].shade > 63) object->polys[i].shade = 63;
	      }
	      else
               object->polys[i].shade = AMBIENCE;
	     }
	}

       else if (object->shade_type == ENVMAP || object->shade_type == PHONG ||
                object->shade_type == TEXTUREMAP || object->shade_type == TEXTUREMAP2)
	for(i=0;i<object->num_polygons;i++)
	{
             current_vert = object->polys[i].a;
             viewvector.x = camera.viewpoint.x - object->vertices_rot[current_vert].x;
             viewvector.y = camera.viewpoint.y - object->vertices_rot[current_vert].y;
             viewvector.z = camera.viewpoint.z - object->vertices_rot[current_vert].z;
                
	     object->polys[i].visible=0;
             if((fixedmul(viewvector.x,object->polys[i].normal_rot.x) +
                 fixedmul(viewvector.y,object->polys[i].normal_rot.y) +
                 fixedmul(viewvector.z,object->polys[i].normal_rot.z)) > 0 )
		      object->polys[i].visible=1;
	}
    }
    //else use object's non-animated info
    else
    {
     if (object->shade_type == GOURAUD || object->shade_type == GOURTEX || object->shade_type == ENVMAP_GOUR)
	for(i=0;i<object->num_polygons;i++)
	{
             current_vert = object->polys[i].a;
             viewvector.x = camera.viewpoint.x - object->vertices_rot[current_vert].x;
             viewvector.y = camera.viewpoint.y - object->vertices_rot[current_vert].y;
             viewvector.z = camera.viewpoint.z - object->vertices_rot[current_vert].z;               
	     object->polys[i].visible=0;
             if((fixedmul(viewvector.x,object->polys[i].normal.x) +
                 fixedmul(viewvector.y,object->polys[i].normal.y) +
                 fixedmul(viewvector.z,object->polys[i].normal.z)) > 0 )
	     {
	      object->polys[i].visible=1;
              if((temp1=(fixedmul(lightsource.x,object->vertex_normal[current_vert].x) +
                         fixedmul(lightsource.y,object->vertex_normal[current_vert].y) +
                         fixedmul(lightsource.z,object->vertex_normal[current_vert].z))) > 0)
	      {
               object->polys[i].quad[0].i = arccos266[temp1>>7] + AMBIENCE266;
               if(object->polys[i].quad[0].i > __63) object->polys[i].quad[0].i = __63;
              }                         
	      else
               object->polys[i].quad[0].i = AMBIENCE266;
	
              current_vert = object->polys[i].b;
              if((temp1=(fixedmul(lightsource.x,object->vertex_normal[current_vert].x) +
                         fixedmul(lightsource.y,object->vertex_normal[current_vert].y) +
                         fixedmul(lightsource.z,object->vertex_normal[current_vert].z))) > 0)
              {     
               object->polys[i].quad[1].i = arccos266[temp1>>7] + AMBIENCE266;
	       if(object->polys[i].quad[1].i > __63) object->polys[i].quad[1].i = __63;
	      }
	      else
               object->polys[i].quad[1].i = AMBIENCE266;

              current_vert = object->polys[i].c;
              if((temp1=(fixedmul(lightsource.x,object->vertex_normal[current_vert].x) +
                         fixedmul(lightsource.y,object->vertex_normal[current_vert].y) +
                         fixedmul(lightsource.z,object->vertex_normal[current_vert].z))) > 0)
	      {
               object->polys[i].quad[2].i = arccos266[temp1>>7] + AMBIENCE266;
	       if(object->polys[i].quad[2].i > __63) object->polys[i].quad[2].i = __63;
	      }
	      else
               object->polys[i].quad[2].i = AMBIENCE266;
	     }
	}
    


       else if (object->shade_type == FLAT || object->shade_type == FLATTEX
                || object->shade_type == CONSTANT)
	for(i=0;i<object->num_polygons;i++)
	{
             current_vert = object->polys[i].a;
             viewvector.x = camera.viewpoint.x - object->vertices_rot[current_vert].x;
             viewvector.y = camera.viewpoint.y - object->vertices_rot[current_vert].y;
             viewvector.z = camera.viewpoint.z - object->vertices_rot[current_vert].z;               
                
	     object->polys[i].visible=0;
             if((fixedmul(viewvector.x,object->polys[i].normal.x) +
                 fixedmul(viewvector.y,object->polys[i].normal.y) +
                 fixedmul(viewvector.z,object->polys[i].normal.z)) > 0 )
	     {
	      object->polys[i].visible=1;
              if((temp1=(fixedmul(lightsource.x,object->polys[i].normal.x) +
                         fixedmul(lightsource.y,object->polys[i].normal.y) +
                         fixedmul(lightsource.z,object->polys[i].normal.z))) > 0)
	      {
               object->polys[i].shade = arccos[temp1>>7] + AMBIENCE;
               if(object->polys[i].shade > 63) object->polys[i].shade = 63;
              }
	      else
               object->polys[i].shade = AMBIENCE;
	     }
	}

       else if (object->shade_type == ENVMAP || object->shade_type == PHONG ||
                object->shade_type == TEXTUREMAP || object->shade_type == TEXTUREMAP2)
	for(i=0;i<object->num_polygons;i++)
	{
             current_vert = object->polys[i].a;
             viewvector.x = camera.viewpoint.x - object->vertices_rot[current_vert].x;
             viewvector.y = camera.viewpoint.y - object->vertices_rot[current_vert].y;
             viewvector.z = camera.viewpoint.z - object->vertices_rot[current_vert].z;               

             object->polys[i].visible=0;
             if((fixedmul(viewvector.x,object->polys[i].normal.x) +
                 fixedmul(viewvector.y,object->polys[i].normal.y) +
                 fixedmul(viewvector.z,object->polys[i].normal.z)) > 0 )
		      object->polys[i].visible=1;
	}
	
    }
    
}



void build_master_facet_list(OBJECT *object)
{
    int i,a,b,c;

          if(object->shade_type == SPRITE3D)
          {
              if(object->polys[0].visible)
               {
                  master_facet_list[num_polys] = &object->polys[0];

                  master_facet_list[num_polys]->vertices[0].x =
                    object->camera_vertices[0].x;
                        
                  master_facet_list[num_polys]->vertices[0].y =
                    object->camera_vertices[0].y;
                        
                  master_facet_list[num_polys]->vertices[0].z =
                    object->camera_vertices[0].z;

                  master_facet_list[num_polys]->vertices[1].z =
                    object->camera_vertices[0].z;

                  master_facet_list[num_polys]->vertices[2].z =
                    object->camera_vertices[0].z;


                  num_polys++;
               }
          }
          //make polygon list using camera coords
          else if(object->shade_type != PHONG && object->shade_type != ENVMAP
                  && object->shade_type != ENVMAP_GOUR)
	   for(i=0;i<object->num_polygons;i++)
	   {
	      if(object->polys[i].visible)
	      {
		  a = object->polys[i].a;
		  b = object->polys[i].b;
		  c = object->polys[i].c;

                  master_facet_list[num_polys] = &object->polys[i];

                  master_facet_list[num_polys]->vertices[0].x =
                    object->camera_vertices[a].x;
                        
                  master_facet_list[num_polys]->vertices[0].y =
                    object->camera_vertices[a].y;
                        
                  master_facet_list[num_polys]->vertices[0].z =
                    object->camera_vertices[a].z;

                  master_facet_list[num_polys]->vertices[1].x =
                    object->camera_vertices[b].x;
                        
                  master_facet_list[num_polys]->vertices[1].y =
                    object->camera_vertices[b].y;
                        
                  master_facet_list[num_polys]->vertices[1].z =
                    object->camera_vertices[b].z;

                  master_facet_list[num_polys]->vertices[2].x =
                    object->camera_vertices[c].x;
                        
                  master_facet_list[num_polys]->vertices[2].y =
                    object->camera_vertices[c].y;
                        
                  master_facet_list[num_polys]->vertices[2].z =
                    object->camera_vertices[c].z;

		  num_polys++;
	      }
	   }

          else if(object->shade_type == ENVMAP && object->animation==ANIMATED)
	   for(i=0;i<object->num_polygons;i++)
	   {
	      if(object->polys[i].visible)
	      {
		  a = object->polys[i].a;
		  b = object->polys[i].b;
		  c = object->polys[i].c;

                  master_facet_list[num_polys] = &object->polys[i];

                  master_facet_list[num_polys]->vertices[0].x =
                    object->camera_vertices[a].x;
                        
                  master_facet_list[num_polys]->vertices[0].y =
                    object->camera_vertices[a].y;
                        
                  master_facet_list[num_polys]->vertices[0].z =
                    object->camera_vertices[a].z;

                  master_facet_list[num_polys]->vertices[1].x =
                    object->camera_vertices[b].x;
                        
                  master_facet_list[num_polys]->vertices[1].y =
                    object->camera_vertices[b].y;
                        
                  master_facet_list[num_polys]->vertices[1].z =
                    object->camera_vertices[b].z;

                  master_facet_list[num_polys]->vertices[2].x =
                    object->camera_vertices[c].x;
                        
                  master_facet_list[num_polys]->vertices[2].y =
                    object->camera_vertices[c].y;
                        
                  master_facet_list[num_polys]->vertices[2].z =
                    object->camera_vertices[c].z;


                  master_facet_list[num_polys]->quad[0].tx =
                        ((((object->vertex_normal_rot[a].x<<6))+_128)>>10);
  
                  master_facet_list[num_polys]->quad[0].ty =
                        ((((object->vertex_normal_rot[a].y<<6))+_128)>>10);
                                                        
                  master_facet_list[num_polys]->quad[1].tx =
                        ((((object->vertex_normal_rot[b].x<<6))+_128)>>10);

                  master_facet_list[num_polys]->quad[1].ty =
                        ((((object->vertex_normal_rot[b].y<<6))+_128)>>10);
			

                  master_facet_list[num_polys]->quad[2].tx =
                        ((((object->vertex_normal_rot[c].x<<6))+_128)>>10);

                  master_facet_list[num_polys]->quad[2].ty =
                        ((((object->vertex_normal_rot[c].y<<6))+_128)>>10);

		  num_polys++;
	      }
	   }

	  else if(object->shade_type == ENVMAP && object->animation==NON_ANIMATED)
	   for(i=0;i<object->num_polygons;i++)
	   {
	      if(object->polys[i].visible)
	      {
		  a = object->polys[i].a;
		  b = object->polys[i].b;
		  c = object->polys[i].c;

                  master_facet_list[num_polys] = &object->polys[i];

                  master_facet_list[num_polys]->vertices[0].x =
                    object->camera_vertices[a].x;
                        
                  master_facet_list[num_polys]->vertices[0].y =
                    object->camera_vertices[a].y;
                        
                  master_facet_list[num_polys]->vertices[0].z =
                    object->camera_vertices[a].z;

                  master_facet_list[num_polys]->vertices[1].x =
                    object->camera_vertices[b].x;
                        
                  master_facet_list[num_polys]->vertices[1].y =
                    object->camera_vertices[b].y;
                        
                  master_facet_list[num_polys]->vertices[1].z =
                    object->camera_vertices[b].z;

                  master_facet_list[num_polys]->vertices[2].x =
                    object->camera_vertices[c].x;
                        
                  master_facet_list[num_polys]->vertices[2].y =
                    object->camera_vertices[c].y;
                        
                  master_facet_list[num_polys]->vertices[2].z =
                    object->camera_vertices[c].z;


                  master_facet_list[num_polys]->quad[0].tx =
                        ((((object->vertex_normal[a].x<<6))+_128)>>10);
  
                  master_facet_list[num_polys]->quad[0].ty =
                        ((((object->vertex_normal[a].y<<6))+_128)>>10);
                                                        
                  master_facet_list[num_polys]->quad[1].tx =
                        ((((object->vertex_normal[b].x<<6))+_128)>>10);

                  master_facet_list[num_polys]->quad[1].ty =
                        ((((object->vertex_normal[b].y<<6))+_128)>>10);
			

                  master_facet_list[num_polys]->quad[2].tx =
                        ((((object->vertex_normal[c].x<<6))+_128)>>10);

                  master_facet_list[num_polys]->quad[2].ty =
                        ((((object->vertex_normal[c].y<<6))+_128)>>10);

                  num_polys++;
	      }
	   }
          else if(object->shade_type == ENVMAP_GOUR && object->animation==ANIMATED)
	   for(i=0;i<object->num_polygons;i++)
	   {
	      if(object->polys[i].visible)
	      {
		  a = object->polys[i].a;
		  b = object->polys[i].b;
		  c = object->polys[i].c;

                  master_facet_list[num_polys] = &object->polys[i];

                  master_facet_list[num_polys]->vertices[0].x =
                    object->camera_vertices[a].x;
                        
                  master_facet_list[num_polys]->vertices[0].y =
                    object->camera_vertices[a].y;
                        
                  master_facet_list[num_polys]->vertices[0].z =
                    object->camera_vertices[a].z;

                  master_facet_list[num_polys]->vertices[1].x =
                    object->camera_vertices[b].x;
                        
                  master_facet_list[num_polys]->vertices[1].y =
                    object->camera_vertices[b].y;
                        
                  master_facet_list[num_polys]->vertices[1].z =
                    object->camera_vertices[b].z;

                  master_facet_list[num_polys]->vertices[2].x =
                    object->camera_vertices[c].x;
                        
                  master_facet_list[num_polys]->vertices[2].y =
                    object->camera_vertices[c].y;
                        
                  master_facet_list[num_polys]->vertices[2].z =
                    object->camera_vertices[c].z;


                  master_facet_list[num_polys]->quad[0].tx =
                        ((((object->vertex_normal_rot[a].x<<6))+_128)>>10);
  
                  master_facet_list[num_polys]->quad[0].ty =
                        ((((object->vertex_normal_rot[a].y<<6))+_128)>>10);
                                                        
                  master_facet_list[num_polys]->quad[1].tx =
                        ((((object->vertex_normal_rot[b].x<<6))+_128)>>10);

                  master_facet_list[num_polys]->quad[1].ty =
                        ((((object->vertex_normal_rot[b].y<<6))+_128)>>10);
			

                  master_facet_list[num_polys]->quad[2].tx =
                        ((((object->vertex_normal_rot[c].x<<6))+_128)>>10);

                  master_facet_list[num_polys]->quad[2].ty =
                        ((((object->vertex_normal_rot[c].y<<6))+_128)>>10);

		  num_polys++;
	      }
	   }

          else if(object->shade_type == ENVMAP_GOUR && object->animation==NON_ANIMATED)
	   for(i=0;i<object->num_polygons;i++)
	   {
	      if(object->polys[i].visible)
	      {
		  a = object->polys[i].a;
		  b = object->polys[i].b;
		  c = object->polys[i].c;

                  master_facet_list[num_polys] = &object->polys[i];

                  master_facet_list[num_polys]->vertices[0].x =
                    object->camera_vertices[a].x;
                        
                  master_facet_list[num_polys]->vertices[0].y =
                    object->camera_vertices[a].y;
                        
                  master_facet_list[num_polys]->vertices[0].z =
                    object->camera_vertices[a].z;

                  master_facet_list[num_polys]->vertices[1].x =
                    object->camera_vertices[b].x;
                        
                  master_facet_list[num_polys]->vertices[1].y =
                    object->camera_vertices[b].y;
                        
                  master_facet_list[num_polys]->vertices[1].z =
                    object->camera_vertices[b].z;

                  master_facet_list[num_polys]->vertices[2].x =
                    object->camera_vertices[c].x;
                        
                  master_facet_list[num_polys]->vertices[2].y =
                    object->camera_vertices[c].y;
                        
                  master_facet_list[num_polys]->vertices[2].z =
                    object->camera_vertices[c].z;


                  master_facet_list[num_polys]->quad[0].tx =
                        ((((object->vertex_normal[a].x<<6))+_128)>>10);
  
                  master_facet_list[num_polys]->quad[0].ty =
                        ((((object->vertex_normal[a].y<<6))+_128)>>10);
                                                        
                  master_facet_list[num_polys]->quad[1].tx =
                        ((((object->vertex_normal[b].x<<6))+_128)>>10);

                  master_facet_list[num_polys]->quad[1].ty =
                        ((((object->vertex_normal[b].y<<6))+_128)>>10);
			

                  master_facet_list[num_polys]->quad[2].tx =
                        ((((object->vertex_normal[c].x<<6))+_128)>>10);

                  master_facet_list[num_polys]->quad[2].ty =
                        ((((object->vertex_normal[c].y<<6))+_128)>>10);

                  num_polys++;
	      }
	   }

	  else if(object->shade_type == PHONG && object->animation==ANIMATED)
	   for(i=0;i<object->num_polygons;i++)
	   {
	      if(object->polys[i].visible)
	      {
		  a = object->polys[i].a;
		  b = object->polys[i].b;
		  c = object->polys[i].c;
/*

                  master_facet_list[num_polys].vertices[0].x =
                    object->camera_vertices[a].x;
                        
		  master_facet_list[num_polys].vertices[0].y =
                    object->camera_vertices[a].y;
                        
		  master_facet_list[num_polys].vertices[0].z =
                    object->camera_vertices[a].z;

                  master_facet_list[num_polys].vertices[1].x =
                    object->camera_vertices[b].x;
                        
                  master_facet_list[num_polys].vertices[1].y =
                    object->camera_vertices[b].y;
                        
                  master_facet_list[num_polys].vertices[1].z =
                    object->camera_vertices[b].z;

                  master_facet_list[num_polys].vertices[2].x =
                    object->camera_vertices[c].x;
                        
                  master_facet_list[num_polys].vertices[2].y =
                    object->camera_vertices[c].y;
                        
                  master_facet_list[num_polys].vertices[2].z =
                    object->camera_vertices[c].z;
                  master_facet_list[num_polys].mirror = 0;

                                     
                  master_facet_list[num_polys].shade_type = object->shade_type;
                  master_facet_list[num_polys].texturemap = object->texturemap;
                  master_facet_list[num_polys].transparent = object->transparent;


		  master_facet_list[num_polys].v1.x=(object->vertex_normal_rot[a].x);
		  master_facet_list[num_polys].v1.y=(object->vertex_normal_rot[a].y);
		  master_facet_list[num_polys].v1.z=(object->vertex_normal_rot[a].z);

		  master_facet_list[num_polys].v2.x=(object->vertex_normal_rot[b].x);
		  master_facet_list[num_polys].v2.y=(object->vertex_normal_rot[b].y);
		  master_facet_list[num_polys].v2.z=(object->vertex_normal_rot[b].z);

		  master_facet_list[num_polys].v3.x=(object->vertex_normal_rot[c].x);
		  master_facet_list[num_polys].v3.y=(object->vertex_normal_rot[c].y);
		  master_facet_list[num_polys].v3.z=(object->vertex_normal_rot[c].z);
*/                   
		  num_polys++;
	      }
	   }

	  else if(object->shade_type == PHONG && object->animation==NON_ANIMATED)
	   for(i=0;i<object->num_polygons;i++)
	   {
	      if(object->polys[i].visible)
	      {
		  a = object->polys[i].a;
		  b = object->polys[i].b;
		  c = object->polys[i].c;

/*
                  master_facet_list[num_polys].vertices[0].x =
                    object->camera_vertices[a].x;
                        
		  master_facet_list[num_polys].vertices[0].y =
                    object->camera_vertices[a].y;
                        
		  master_facet_list[num_polys].vertices[0].z =
                    object->camera_vertices[a].z;

                  master_facet_list[num_polys].vertices[1].x =
                    object->camera_vertices[b].x;
                        
                  master_facet_list[num_polys].vertices[1].y =
                    object->camera_vertices[b].y;
                        
                  master_facet_list[num_polys].vertices[1].z =
                    object->camera_vertices[b].z;

                  master_facet_list[num_polys].vertices[2].x =
                    object->camera_vertices[c].x;
                        
                  master_facet_list[num_polys].vertices[2].y =
                    object->camera_vertices[c].y;
                        
                  master_facet_list[num_polys].vertices[2].z =
                    object->camera_vertices[c].z;

                  master_facet_list[num_polys].mirror = 0;
                                     
                  master_facet_list[num_polys].shade_type = object->shade_type;
                  master_facet_list[num_polys].texturemap = object->texturemap;
                  master_facet_list[num_polys].transparent = object->transparent;


		  master_facet_list[num_polys].v1.x=(object->vertex_normal[a].x);
		  master_facet_list[num_polys].v1.y=(object->vertex_normal[a].y);
		  master_facet_list[num_polys].v1.z=(object->vertex_normal[a].z);
		  
		  master_facet_list[num_polys].v2.x=(object->vertex_normal[b].x);
		  master_facet_list[num_polys].v2.y=(object->vertex_normal[b].y);
		  master_facet_list[num_polys].v2.z=(object->vertex_normal[b].z);

		  master_facet_list[num_polys].v3.x=(object->vertex_normal[c].x);
		  master_facet_list[num_polys].v3.y=(object->vertex_normal[c].y);
		  master_facet_list[num_polys].v3.z=(object->vertex_normal[c].z);
 */                  
		  num_polys++;
	      }
	   }

}


void perspective_transform()
{
  int i;
  float x,y,z;


  //do the perspective transformation
  for(i=0;i<num_polys;i++)
  {
  
    x = fixedmul(master_facet_list[i]->vertices[0].x,_250)/65536.0;
    y = fixedmul(master_facet_list[i]->vertices[0].y,_200)/65536.0;
    z = master_facet_list[i]->vertices[0].z/65536.0;
    if( z<= 0) z =.5;


    master_facet_list[i]->quad[0].x =
          160 + x/z;
    master_facet_list[i]->quad[0].y =
          100 - y/z;

    x = fixedmul(master_facet_list[i]->vertices[1].x,_250)/65536.0;
    y = fixedmul(master_facet_list[i]->vertices[1].y,_200)/65536.0;
    z = master_facet_list[i]->vertices[1].z/65536.0;
    if( z<= 0) z =.5;


    master_facet_list[i]->quad[1].x =
          160 + x/z;
    master_facet_list[i]->quad[1].y =
          100 - y/z;

    x = fixedmul(master_facet_list[i]->vertices[2].x,_250)/65536.0;
    y = fixedmul(master_facet_list[i]->vertices[2].y,_200)/65536.0;
    z = master_facet_list[i]->vertices[2].z/65536.0;
    if( z<= 0) z =.5;


    master_facet_list[i]->quad[2].x =
          160 + x/z;
    master_facet_list[i]->quad[2].y =
          100 - y/z;
/*

    if(master_facet_list[i]->vertices[0].z <= 32768)
      master_facet_list[i]->vertices[0].z = 62768;
    if(master_facet_list[i]->vertices[1].z <= 32768)
      master_facet_list[i]->vertices[1].z = 62768;
    if(master_facet_list[i]->vertices[2].z <= 32768)
      master_facet_list[i]->vertices[2].z = 62768;

    master_facet_list[i]->quad[0].x =
          160 + fixed_to_int(fixeddiv(master_facet_list[i]->vertices[0].x<<8,
           master_facet_list[i]->vertices[0].z));
    master_facet_list[i]->quad[0].y =
          100 - fixed_to_int(fixeddiv(master_facet_list[i]->vertices[0].y<<8,
           master_facet_list[i]->vertices[0].z));

    master_facet_list[i]->quad[1].x =
          160 + fixed_to_int(fixeddiv(master_facet_list[i]->vertices[1].x<<8,
           master_facet_list[i]->vertices[1].z));
    master_facet_list[i]->quad[1].y =
          100 - fixed_to_int(fixeddiv(master_facet_list[i]->vertices[1].y<<8,
           master_facet_list[i]->vertices[1].z));

    master_facet_list[i]->quad[2].x =
          160 + fixed_to_int(fixeddiv(master_facet_list[i]->vertices[2].x<<8,
           master_facet_list[i]->vertices[2].z));
    master_facet_list[i]->quad[2].y =
          100 - fixed_to_int(fixeddiv(master_facet_list[i]->vertices[2].y<<8,
           master_facet_list[i]->vertices[2].z));
*/
  }
}

void perspective_transform_mirror()
{
  int i;

  //do the perspective transformation
  for(i=0;i<num_polys;i++)
  {
    
    master_facet_list[i]->quad[0].x =
          128 + fixed_to_int(fixeddiv(master_facet_list[i]->vertices[0].x<<8,
           master_facet_list[i]->vertices[0].z));
    master_facet_list[i]->quad[0].y =
          128 - fixed_to_int(fixeddiv(master_facet_list[i]->vertices[0].y<<8,
           master_facet_list[i]->vertices[0].z));

    master_facet_list[i]->quad[1].x =
          128 + fixed_to_int(fixeddiv(master_facet_list[i]->vertices[1].x<<8,
           master_facet_list[i]->vertices[1].z));
    master_facet_list[i]->quad[1].y =
          128 - fixed_to_int(fixeddiv(master_facet_list[i]->vertices[1].y<<8,
           master_facet_list[i]->vertices[1].z));

    master_facet_list[i]->quad[2].x =
          128 + fixed_to_int(fixeddiv(master_facet_list[i]->vertices[2].x<<8,
           master_facet_list[i]->vertices[2].z));
    master_facet_list[i]->quad[2].y =
          128 - fixed_to_int(fixeddiv(master_facet_list[i]->vertices[2].y<<8,
           master_facet_list[i]->vertices[2].z));
  }
}


void create_lookup_tables(void)
{
 float i;
 int index = 0;

 for(i=0;i<360;i++)
  cosine[(int)i] = float_to_fixed((cos(i/180 * 3.14159)));
 
 for(i=0;i<360;i++)
  sine[(int)i] = float_to_fixed((sin(i/180 * 3.14159)));

 index = 0;
 for(i=0;i<512;i++)
 {
  arccos266[index] =  __63 - fixedmul266((acos(i/512.0)/3.14159 * 180)*64.0,__7);
//  arccos1616[index++] = fixedmul((acos(i/512.0)/3.14159 * 180)*65536.0,_7);
  arccos[index++] = 63 - ((acos(i/512.0)/3.14159 * 180) * 0.711111);
 }

}

int lgmine(const void **x,const void **y)
{
 POLYGON *temp,*temp2;
 temp = (POLYGON *) *x; 
 temp2 = (POLYGON *) *y;
 
 if(temp->avg_z < temp2->avg_z)
  return(1);
 else if(temp->avg_z > temp2->avg_z)
  return(-1);
 else
  return(0);
}

void sort_viewable_faces(void)
{
 int i;
 for(i=0;i<num_polys;i++)
  master_facet_list[i]->avg_z = fixedmul(_333333,(master_facet_list[i]->vertices[0].z +
                         master_facet_list[i]->vertices[1].z +
                         master_facet_list[i]->vertices[2].z));


 qsort(master_facet_list,num_polys,sizeof(POLYGON *),lgmine);
}



int remove_object(OBJECT *object)
{
    Fixed32 x_bsphere,y_bsphere,z_bsphere,x_compare,y_compare,radius;
    Fixed32 tempx,tempy,tempz,xy;

    if(object->frustrum == 0)
     return 0;
    
    tempx = object->world_pos.x;
    tempy = object->world_pos.y;
    tempz = object->world_pos.z;
	
    xy = fixedmul(tempx,tempy);
	
    x_bsphere =
		fixedmul(tempx + camy1,tempy + camx1) +
		fixedmul(tempz,camz1) +
		cam1 - xy;

    y_bsphere = 
		fixedmul(tempx + camy2,tempy + camx2) +
		fixedmul(tempz,camz2) +
		cam2 - xy;

    z_bsphere =
		fixedmul(tempx + camy3,tempy + camx3) +
		fixedmul(tempz,camz3) +
		cam3 - xy;

    radius=object->radius;

    if( ((z_bsphere-radius) > clip_far_z) ||
	((z_bsphere+radius) < clip_near_z))
	return 1;

    x_compare = fixedmul(z_bsphere,fov_width);
	  
    if( ((x_bsphere-radius) > x_compare) ||
	((x_bsphere+radius) < -x_compare))
	return 1;

    y_compare = fixedmul(z_bsphere,fov_height);
	
    if( ((y_bsphere-radius) > y_compare) ||
	((y_bsphere+radius) < -y_compare))
	return 1;

   return 0;
}


int remove_object_mirror(OBJECT *object)
{
    Fixed32 x_bsphere,y_bsphere,z_bsphere,x_compare,y_compare,radius;
    Fixed32 tempx,tempy,tempz,xy;

    if(object->frustrum == 0)
     return 0;
    
    tempx = object->world_pos.x;
    tempy = object->world_pos.y;
    tempz = object->world_pos.z;
	
    xy = fixedmul(tempx,tempy);
	
    x_bsphere =
		fixedmul(tempx + camy1,tempy + camx1) +
		fixedmul(tempz,camz1) +
		cam1 - xy;

    y_bsphere = 
		fixedmul(tempx + camy2,tempy + camx2) +
		fixedmul(tempz,camz2) +
		cam2 - xy;

    z_bsphere =
		fixedmul(tempx + camy3,tempy + camx3) +
		fixedmul(tempz,camz3) +
		cam3 - xy;

    radius=object->radius;

    if( ((z_bsphere-radius) > clip_far_z) ||
	((z_bsphere+radius) < clip_near_z))
	return 1;

    x_compare = fixedmul(z_bsphere,fov_width_mirror);
	  
    if( ((x_bsphere-radius) > x_compare) ||
	((x_bsphere+radius) < -x_compare))
	return 1;

    y_compare = fixedmul(z_bsphere,fov_height_mirror);
	
    if( ((y_bsphere-radius) > y_compare) ||
	((y_bsphere+radius) < -y_compare))
	return 1;

   return 0;
}

void poly3d_frustrum_clip(OBJECT *object)
{
 long m;
 Fixed32 z1,z2,z3;

 for(m=0;m<object->num_polygons;m++)
    if(object->polys[m].visible)
    {
       z1 = object->camera_vertices[object->polys[m].a].z;
       z2 = object->camera_vertices[object->polys[m].b].z;
       z3 = object->camera_vertices[object->polys[m].c].z;

       //first do near z plane clipping

       if(z1 < clip_near_z && z2 < clip_near_z && z3 < clip_near_z)
           object->polys[m].visible=0;
    }
}


void load_cobFN(char *filename,OBJECT *object,PATH *path)
{
 int i,j,k;
 FILE *cobf;
 static CALIGARI_HEADER cob_header;
 static CALIGARI_CHUNK  cob_chunk;
 static char t,*name;
 //current position matrix is really a 3x4 but 4x4 is allocated
 //so the transpose can be taken easily
 static float local_axes[4][4],current_position[4][4];
 static float x,y,z,*u,*v,temp,mag,count;
 static short length,num_verts_in_face,material_index,dupecount;
 static long tv,index;
 static VECTOR_3D R,S;
 static VECTOR_3D_FLOAT *temp_normal;
 static Fixed32 sum,current_position_fixed[4][4],x_fixed,y_fixed,z_fixed,
 		local_axes_fixed[4][4];
 static short path_type,path_flags,num_frames;
 static float acc,path_t,c,b,path_time;

 name = (char *) malloc (sizeof(char)*80);

 cobf = fopen(filename,"rb");
 if(cobf == NULL)
 {
   printf("COB LOAD ERROR: File name: %s not found.\n",filename);
   exit(1);
 }
 
 //First test if this is a .COB File
 fread(&cob_header,sizeof(CALIGARI_HEADER),1,cobf);

 if(strncmp(cob_header.caligari_id,"Caligari ",9) != 0 )
 {
   printf("COB LOAD ERROR: File is not in the .cob format!\n");
   exit(1);
 }
 
 if(strncmp(cob_header.version,"V00.01",6) != 0)
 {
   printf("COB LOAD ERROR: File is not a Version 00.01 .cob !\n");
   exit(1);
 }

 if(cob_header.file_type != 'B')
 {
  printf("COB LOAD ERROR: File: %s is not in .cob binary format!\n",filename);
  exit(1);
 }

 fread(&cob_chunk,sizeof(CALIGARI_CHUNK),1,cobf);
 while(strncmp(cob_chunk.chunk_type,END_CHUNK,4) != 0)
 {
  if(strncmp(cob_chunk.chunk_type,GROUP_CHUNK,4) == 0)
  {
   //skip group chunks for now
   printf("Skipping Group Chunk...\n");
   for(i=0; i < cob_chunk.chunk_size; i++)
    fread(&t,sizeof(char),1,cobf);
  }
  else if(strncmp(cob_chunk.chunk_type,MATERIAL_CHUNK,4) == 0)
  {
   //skip material chunks for now
   printf("Skipping Material Chunk...\n");
   for(i=0; i < cob_chunk.chunk_size; i++)
    fread(&t,sizeof(char),1,cobf);
  }
  else if(strncmp(cob_chunk.chunk_type,PROC_TEXTURE_CHUNK,4) == 0)
  {
   //skip procedural texture chunks for now
   printf("Skipping material Chunk...\n");
   for(i=0; i < cob_chunk.chunk_size; i++)
    fread(&t,sizeof(char),1,cobf);
  }
  else if(strncmp(cob_chunk.chunk_type,UNIT_CHUNK,4) == 0)
  {
   //skip unit chunks for now
   printf("Skipping Unit Chunk...\n");
   for(i=0; i < cob_chunk.chunk_size; i++)
    fread(&t,sizeof(char),1,cobf);
  }
  else if(strncmp(cob_chunk.chunk_type,CHAN_CHUNK,4) == 0)
  {
   if(path == NULL)
     for(i=0; i < cob_chunk.chunk_size; i++)
      fread(&t,sizeof(char),1,cobf);
   else
   {
   //NO dupecount
   fread(&length,sizeof(short),1,cobf);
   
   //read in name string of length length
   fread(name,sizeof(char),length,cobf);
   name[length] = '\0';
   printf("Name: %s\n",name);
   
   fread(&path_type,sizeof(short),1,cobf);
   printf("Path type: %d\n",path_type);

   fread(&path_flags,sizeof(short),1,cobf);
   printf("Path flags: %d\n",path_flags);

   fread(&num_frames,sizeof(short),1,cobf);
   printf("Frames: %d\n",num_frames);

   path->control_points = (POINT_3D *) malloc(sizeof(POINT_3D)*num_frames);
   path->num_points = num_frames;

   for(i=0;i<num_frames;i++)
   {
       fread(&path_time,sizeof(float),1,cobf);
       fread(&acc,sizeof(float),1,cobf);
       fread(&path_t,sizeof(float),1,cobf);
       fread(&c,sizeof(float),1,cobf);
       fread(&b,sizeof(float),1,cobf);
       fread(&x,sizeof(float),1,cobf);
       fread(&y,sizeof(float),1,cobf);
       fread(&z,sizeof(float),1,cobf);

       path->control_points[i].x = x*65536.0;
       path->control_points[i].y = y*65536.0;
       path->control_points[i].z = z*65536.0;
   }
   }
  }
  else if(strncmp(cob_chunk.chunk_type,POLYGON_CHUNK,4) == 0)
  {
   if(cob_chunk.major_ver != 0 || cob_chunk.minor_ver != 2)
   {
       printf("COB LOAD ERROR: The PolH chunk in this .cob file is not "\
       			      " supported by this decoder\n");
       exit(1);
   }
   //read dupecount then length of string that follows
   fread(&dupecount,sizeof(short),1,cobf);
   fread(&length,sizeof(short),1,cobf);
   
   //read in name string of length length
   fread(name,sizeof(char),length,cobf);
   name[length] = '\0';
   printf("Object Name: %s\n",name);

   //read in local axes matrix 4x3
   fread(local_axes,sizeof(float),12,cobf);
   printf("\nLocal axes matrix:\n");
   for(i=0;i<4;i++)
   {
    printf("\n");
    for(j=0;j<3;j++)
    printf(" %.4f ",local_axes[i][j]);
   }

   //read in current position matrix 3x4
   fread(current_position,sizeof(float),12,cobf);
   printf("\nCurrent Position Matrix:\n");
   for(i=0;i<3;i++)
   {
    printf("\n");
    for(j=0;j<4;j++)
    printf(" %.4f ",current_position[i][j]);
   }
   printf("\n");

   fread(&(object->num_vertices),sizeof(long),1,cobf);
   
   object->local_vertices    = (POINT_3D *) malloc(sizeof(POINT_3D)*object->num_vertices);
   object->camera_vertices   = (POINT_3D *) malloc(sizeof(POINT_3D)*object->num_vertices);
   object->vertices_rot      = (POINT_3D *) malloc(sizeof(POINT_3D)*object->num_vertices);
   object->vertex_normal     = (VECTOR_3D *) malloc(sizeof(POINT_3D)*object->num_vertices);
   object->vertex_normal_rot = (VECTOR_3D *) malloc(sizeof(POINT_3D)*object->num_vertices);

   //Read in vertices
   for(i=0; i < object->num_vertices; i++)
   {
    fread(&x,sizeof(float),1,cobf);
    fread(&y,sizeof(float),1,cobf);
    fread(&z,sizeof(float),1,cobf);

    //Convert from float to fixed 16:16
    object->local_vertices[i].x = x * 65536.0;
    object->local_vertices[i].y = y * 65536.0;
    object->local_vertices[i].z = z * 65536.0;
   }

   //Read in Texture vertices
   fread(&(object->num_texture_vertices),sizeof(long),1,cobf);
   
   u = (float *) malloc (sizeof(float)*object->num_texture_vertices);
   v = (float *) malloc (sizeof(float)*object->num_texture_vertices);
   
   for(i=0; i < object->num_texture_vertices; i++)
   {
       fread(&u[i],sizeof(float),1,cobf);
       fread(&v[i],sizeof(float),1,cobf);
   }

   fread(&object->num_polygons,sizeof(long),1,cobf);
   object->polys = (POLYGON *) malloc (sizeof(POLYGON)*object->num_polygons);
   
   for(i=0;i<object->num_polygons;i++)
   {
       fread(&t,sizeof(char),1,cobf);
       if(t == F_HOLE)
       {
	   printf("COB LOAD ERROR: This object has holes in it. Not supported by this decoder!\n");
	   exit(1);
       }
       fread(&num_verts_in_face,sizeof(short),1,cobf);
       if(num_verts_in_face != 3)
       {
	   printf("COB LOAD ERROR: This object has polygons made up of more than 3 sides.\n");
	   exit(1);
       }
       //non-esential info.. but still must read it in
       fread(&material_index,sizeof(short),1,cobf);
       
       fread(&(object->polys[i].a),sizeof(long),1,cobf);
       fread(&tv,sizeof(long),1,cobf);
       object->polys[i].quad[0].tx = (u[tv] * (TEXTURE_WIDTH - 1))*64.0;
       object->polys[i].quad[0].ty = (v[tv] * (TEXTURE_HEIGHT - 1))*64.0;
	
       fread(&(object->polys[i].b),sizeof(long),1,cobf);
       fread(&tv,sizeof(long),1,cobf);
       object->polys[i].quad[1].tx = (u[tv] * (TEXTURE_WIDTH - 1))*64.0;
       object->polys[i].quad[1].ty = (v[tv] * (TEXTURE_HEIGHT - 1))*64.0;
	
       fread(&(object->polys[i].c),sizeof(long),1,cobf);
       fread(&tv,sizeof(long),1,cobf);
       object->polys[i].quad[2].tx = (u[tv] * (TEXTURE_WIDTH - 1))*64.0;
       object->polys[i].quad[2].ty = (v[tv] * (TEXTURE_HEIGHT - 1))*64.0;
   }
  }
  else
  {
      printf("Skipping unknown chunk: %s\n",cob_chunk.chunk_type);
      
      for(i=0; i < cob_chunk.chunk_size; i++)
       fread(&t,sizeof(char),1,cobf);
  }

 fread(&cob_chunk,sizeof(CALIGARI_CHUNK),1,cobf);
 }
  sum=0;
  //calculate average radius for bounding sphere used in frustrum clip
  for(i=0;i<object->num_vertices;i++)
	sum+=fixedsqrt(fixedsquare(object->local_vertices[i].x) +
		       fixedsquare(object->local_vertices[i].y) +
		       fixedsquare(object->local_vertices[i].z));

  object->radius = fixeddiv(sum,(object->num_vertices)<<16);

  object->face_type = num_verts_in_face;

  if(object->use_truespace_pos)
  {
     printf("Using truespace position...\n");
	 
     //take transpose of current_matrix
     for(i=0;i<4;i++)
      for(j=i+1;j<4;j++)
      {
	  temp = current_position[i][j];
	  current_position[i][j] = current_position[j][i];
	  current_position[j][i] = temp;
      }

     //swap 0 and 2 columns
     for(i=0;i<4;i++)
     {
	 temp = current_position[i][0];
	 current_position[i][0] = current_position[i][2];
         current_position[i][2] = temp;
     }

     //swap 1 and 2 columns
     for(i=0;i<4;i++)
     {
	 temp = current_position[i][1];
	 current_position[i][1] = current_position[i][2];
         current_position[i][2] = temp;
     }

     //convert the matrix to fixed point
     for(i=0;i<4;i++)
      for(j=0;j<4;j++)
	  current_position_fixed[i][j]= current_position[i][j]*65536.0;
     
	  
     printf("current position matrix:\n");
     for(i=0;i<4;i++)
     {
      printf("\n");
      for(j=0;j<4;j++)
       printf(" %.4f ",current_position[i][j]);
     }
     printf("\n");
      
      for(i=0;i<object->num_vertices;i++)
      {
	  x_fixed = fixedmul(object->local_vertices[i].x,
	   	       current_position_fixed[0][0])+
              fixedmul(object->local_vertices[i].y,
	               current_position_fixed[1][0])+
	      fixedmul(object->local_vertices[i].z,
	               current_position_fixed[2][0]);

	  y_fixed = fixedmul(object->local_vertices[i].x,
	   	       current_position_fixed[0][1])+
              fixedmul(object->local_vertices[i].y,
	               current_position_fixed[1][1])+
	      fixedmul(object->local_vertices[i].z,
	               current_position_fixed[2][1]);

	  z_fixed = fixedmul(object->local_vertices[i].x,
	   	       current_position_fixed[0][2])+
              fixedmul(object->local_vertices[i].y,
	               current_position_fixed[1][2])+
	      fixedmul(object->local_vertices[i].z,
	               current_position_fixed[2][2]);

          object->local_vertices[i].x = x_fixed;
	  object->local_vertices[i].y = y_fixed;
          object->local_vertices[i].z = z_fixed;
      }
      printf("pos x: %f\n",current_position_fixed[3][0]/65536.0);
      printf("pos y: %f\n",current_position_fixed[3][1]/65536.0);
      printf("pos z: %f\n",current_position_fixed[3][2]/65536.0);
      
      object->world_pos.x = current_position_fixed[3][0];
      object->world_pos.y = current_position_fixed[3][1];
      object->world_pos.z = current_position_fixed[3][2];
  }
  
  for(i=0;i<object->num_polygons;i++)
  {

       //Calculate surface normal
       R.x = object->local_vertices[object->polys[i].a].x -
             object->local_vertices[object->polys[i].b].x;

       R.y = object->local_vertices[object->polys[i].a].y -
             object->local_vertices[object->polys[i].b].y;

       R.z = object->local_vertices[object->polys[i].a].z -
             object->local_vertices[object->polys[i].b].z;

       S.x = object->local_vertices[object->polys[i].c].x -
             object->local_vertices[object->polys[i].b].x;

       S.y = object->local_vertices[object->polys[i].c].y -
             object->local_vertices[object->polys[i].b].y;

       S.z = object->local_vertices[object->polys[i].c].z -
             object->local_vertices[object->polys[i].b].z;

       cross_product_float(R,S,&(object->polys[i].normal));

  }

  temp_normal = (VECTOR_3D_FLOAT *) malloc(sizeof(VECTOR_3D_FLOAT)*object->num_vertices);
   for(i=0;i<object->num_vertices;i++)
   {
     printf("Vertex Normal Calc.: %2.2f%%\r",((float)i/(float)object->num_vertices)*100.0);
     index = 0;
     for(j=0;j<object->num_polygons;j++)
      if(object->polys[j].a == i || object->polys[j].b == i || object->polys[j].c == i) 
      {
        temp_normal[index].x=object->polys[j].normal.x/65536.0;
        temp_normal[index].y=object->polys[j].normal.y/65536.0;
        temp_normal[index].z=object->polys[j].normal.z/65536.0;
        index++;
      }


     for(j=0;j<index;j++)
      for(k=0;k<index;k++)
       if(temp_normal[j].x == temp_normal[k].x &&
          temp_normal[j].y == temp_normal[k].y &&
          temp_normal[j].z == temp_normal[k].z && k!=j)
         {
	       temp_normal[k].x = -1000;
	       temp_normal[k].y = -1000;
	       temp_normal[k].z = -1000;
         }

    x = y = z = count = 0;
    for(j=0;j<index;j++)
     if(temp_normal[j].x != -1000 && temp_normal[j].y != -1000 && temp_normal[j].z != -1000)
     {
	      x += temp_normal[j].x;
	      y += temp_normal[j].y;
	      z += temp_normal[j].z;
	      count++;
     }

	  
    x/=count;
    y/=count;
    z/=count;
    mag = sqrt(x * x + y * y + z * z);
    object->vertex_normal[i].x = float_to_fixed(x/mag);
    object->vertex_normal[i].y = float_to_fixed(y/mag);
    object->vertex_normal[i].z = float_to_fixed(z/mag);
   }
  printf("\n");
  printf("Face Type: %d\n",object->face_type);
  printf("Num vertices: %d\n",object->num_vertices);
  printf("Num polygons: %d\n",object->num_polygons);
  printf("Texture vertices: %d\n",object->num_texture_vertices);
  free(temp_normal);
  free(u);
  free(v);
  fclose(cobf);
}

set_sprite3d(OBJECT *object,POINT_3D p,POINT_3D w_pos,int tmap,int animation)
{
 object->num_vertices = 1;
 object->num_polygons = 1;
 object->local_vertices = (POINT_3D *)malloc(sizeof(POINT_3D));
 object->camera_vertices = (POINT_3D *)malloc(sizeof(POINT_3D));
 object->vertices_rot = (POINT_3D *)malloc(sizeof(POINT_3D));
 object->polys = (POLYGON *) malloc (sizeof(POLYGON));
 object->polys[0].a = 0;
 object->polys[0].b = 0;
 object->polys[0].c = 0;
 object->polys[0].shade_type = SPRITE3D;
 object->polys[0].texturemap = texturemap + tmap*65536;
 object->cull_tag=0;
 object->frustrum=1;
 object->clip_near_z=1;
 object->mirror=0;
 object->animation=animation;
 object->shade_type=SPRITE3D;
 object->x_ang = object->y_ang = object->z_ang = 0;
 object->world_pos.x = w_pos.x;
 object->world_pos.y = w_pos.y;
 object->world_pos.z = w_pos.z;
 object->texturemap = texturemap + tmap*65536;
 object->local_vertices[0].x = p.x;
 object->local_vertices[0].y = p.y;
 object->local_vertices[0].z = p.z;

}
