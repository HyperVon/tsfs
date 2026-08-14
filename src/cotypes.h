#ifndef _COTYPES_H_
#define _COTYPES_H_


#define UNDEFINED -1

#define ANIMATED 3
#define NON_ANIMATED 4
#define MAX_POLYS_PER_FRAME 100000

/* Macros for Type Conversion */
#define int_to_fixed(x) ((x) << 16)
#define fixed_to_int(x) ((x) >> 16)
#define float_to_fixed(x) ((Fixed32)((x) * 65536.0))
#define fixed_to_float(x) ((float)((x) / 65536.0))



#define FLAT 1
#define GOURAUD 2
#define PHONG 3
#define ENVMAP 6
#define FLATTEX 4
#define GOURTEX 5
#define TEXTUREMAP 7
#define TEXTUREMAP2 8
#define SPRITE3D 9
#define CONSTANT 10
#define ENVMAP_GOUR 11

#define TRI 3
#define QUAD 4

#define F_HOLE              0x08

#define GROUP_CHUNK        "Grou"
#define END_CHUNK          "END "
#define POLYGON_CHUNK      "PolH"
#define MATERIAL_CHUNK     "Mat1"
#define PROC_TEXTURE_CHUNK "PrTx"
#define UNIT_CHUNK         "Unit"
#define CHAN_CHUNK         "Chan"

#define VGA_INPUT_STATUS_1 0x3DA
#define VGA_VSYNC_MASK 0x08
#define PALETTE_MASK 0x3C6
#define PALETTE_REGISTER_RD 0x3C7
#define PALETTE_REGISTER_WR 0x3C8
#define PALETTE_DATA 0x3C9
#define VIDEO_STORAGE 64000*sizeof(unsigned char)
#define PALETTE_STORAGE 768*sizeof(unsigned char)
#define PIC_PTR (unsigned char *)
#define VIDEO_BUFFER PIC_PTR malloc(VIDEO_STORAGE)
#define plot_pixel(x,y,color) buffer[((y<<8) + (y<<6)) + x] = color
#define get_pixel(x,y) buffer[((y<<8) + (y<<6)) + x]
#define Wait_For_Vsync()\
while(inp(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK);\
while(!(inp(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK))
#define ABS(x) (((x) < 0) ? -(x) : (x))
#define SGN(a) (((a) < 0) ? -1 : 1)
#define SWAP(a,b) {a^=b; b^=a; a^=b;}

typedef struct RGB_color_typ
		{
			unsigned char red;
			unsigned char green;
			unsigned char blue;
		} RGB_color, *RGB_color_ptr;


typedef struct {
        char    caligari_id[9];
        char    version[6];
        char    file_type;
        char    endian_type[2];
        char    blanks[13];
        char    newline_char;
        }CALIGARI_HEADER;

typedef struct {
        char    chunk_type[4];
        short   major_ver;
        short   minor_ver;
        long    chunk_id;
        long    parent_id;
        long    chunk_size;
        }CALIGARI_CHUNK;

typedef long Fixed32;          // 16.16 FixedPoint
typedef Fixed32 matrix_4x4[4][4];
typedef Fixed32 matrix_3x3[3][3];


typedef struct {
   Fixed32 x,y;           // screen coords
   Fixed32 tx,ty;         // texture coords
   Fixed32 i;             // intensity 
}TEXTURE;

typedef struct {
    Fixed32 x,y,z;        //a 3d point fixed 16:16 point
}POINT_3D;

typedef struct {
    float x,y,z;          //a 3d point floating point
}POINT_3D_float;

typedef struct {
    Fixed32 x,y,z;        //a 3d vector fixed 16:16 point
}VECTOR_3D;

typedef struct {
    float x,y,z;          //a 3d vector floating point
}VECTOR_3D_FLOAT;


typedef struct {
    POINT_3D viewpoint;   //view point
    POINT_3D lookat;      //lookat point
    }CAMERA;

typedef struct {
    unsigned short a,b,c;          //the three vertices that make up this triangle

    VECTOR_3D normal;     //surface normal

    VECTOR_3D normal_rot; //rotated surface normal

    TEXTURE quad[3];

    unsigned char visible,color,shade,shade_type,mirror,transparent;

    unsigned char *texturemap;

    POINT_3D vertices[3];

    Fixed32 avg_z;

//    VECTOR_3D v1,v2,v3;

}POLYGON;

typedef struct {
    POINT_3D *control_points;
    POINT_3D *path;
    long     num_points;
    long     num_path_points;
}PATH;

typedef struct {
    int num_vertices;   // number of vertices in this object

    int num_polygons;      // number of polygons in this object

    int num_texture_vertices;

    POINT_3D *local_vertices;  //local veritces... never change

    POINT_3D *camera_vertices;   //camera coordinates

    POINT_3D *vertices_rot; //rotated vertices...used for animation

    POLYGON *polys;     //pointer to polygon malloc necessary size

    POINT_3D world_pos; //position in world

    VECTOR_3D *vertex_normal,*vertex_normal_rot;

    PATH object_path;

    Fixed32 radius;       // the average radius of the object used in 3d volume clipping

    unsigned char visible;        // used in 3d volume clipping to tell whether the object is
			// is visible or totally out of the field of view
			
    unsigned char animation;

    unsigned char face_type;

    unsigned char shade_type;

    unsigned char use_truespace_pos;

    unsigned char frustrum;

    unsigned char cull_tag;

    unsigned char *texturemap;

    unsigned char mirror;  // if this object is a mirror surface or not

    int x_ang,y_ang,z_ang;

    int clip_near_z;

}OBJECT;

typedef struct {
		char    manufacturer;
		char    version;
		char    encoding;
		char    bits_per_pixel;
		short      xmin,ymin;
		short     xmax,ymax;
		short     hres;
		short     vres;
		char    palette[48];
		char    reserved;
		char    colour_planes;
		short     bytes_per_line;
		short     palette_type;
		char    filler[58];
	       } PCXHEAD;


#endif
