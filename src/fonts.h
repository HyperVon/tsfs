#define CENTER 1
#define LEFT 2
#define RIGHT 3


//limited to 8 bit wide fonts for now!

unsigned char *fonts[256];
unsigned short font_height,font_width,original_font_mask;


void load_fontFN(char *filename,int height,int width)
{
 int i,k,start_char,end_char;
 FILE *fp;
 
 fp = fopen(filename,"rb");
 if(fp == NULL)
 {
     printf("FONT LOAD ERROR: %s does not exist.\n",filename);
     exit(1);
 }

 for(i=0;i<256;i++)
  fonts[i] = (unsigned char *) malloc (height*sizeof(unsigned char));
  
 if(strcmp(filename,"6x6.fnt") == 0)
 {
     start_char = 32;
     end_char = 128;
     original_font_mask = 128;
 }

 else if(strcmp(filename,"future.fnt") == 0)
 {
     start_char = 0;
     end_char = 256;
     original_font_mask = 128;
 }
 
 if(width <= 8)
  for(i=start_char;i<end_char;i++)
   fread(fonts[i],sizeof(unsigned char),height,fp);
 else if(width > 8 && width <= 16)
  for(i=start_char;i<end_char;i++)
   fread(fonts[i],sizeof(unsigned short),height,fp);
 else
 {
  printf("FONT LOAD ERROR: width too wide\n");
  return;
 }

 font_height = height;
 font_width = width;
}

void putfnt(short x, short y, unsigned char c,unsigned char color)
{
    unsigned short t;
    int row,col,Y;
    unsigned short mask;
    unsigned char *dest;

    dest = buffer + (y<<6) + (y<<8) + x;
    for(row=0;row < font_height;row++)
    {
     mask=original_font_mask;
     t = fonts[c][row];
     
     for(col=0; col < font_width;col++)
     {
	 if(t & mask)
          *(dest+col) = color;
	 mask >>= 1;
     }
     dest+=320;
    }
}
    
void putstr(short x, short y, char *string, int mode,unsigned char color)
{
    int i,len;

    if(mode == LEFT)
    for(i=0;string[i] != '\0';i++)
    {
     putfnt(x,y,string[i],color);
     x+=font_width+1;
    }

    else if(mode == CENTER)
    {
	x = (320-(strlen(string)*font_width))>>1;
	
        for(i=0;string[i] != '\0';i++)
        {
         putfnt(x,y,string[i],color);
         x+=font_width+1;
        }
    }
}

void putfnt_trans(short x, short y, unsigned char c,unsigned char color)
{
    unsigned short t;
    int row,col,Y;
    unsigned short mask;
    unsigned char *dest;

    dest = buffer + (y<<6) + (y<<8) + x;
    for(row=0;row < font_height;row++)
    {
     mask=original_font_mask;
     t = fonts[c][row];
     
     for(col=0; col < font_width;col++)
     {
	 if(t & mask)
          *(dest+col) = TRANSPAR[(color<<8)+*(dest+col)];
	 mask >>= 1;
     }
     dest+=320;
    }
}


void putstr_trans(short x, short y, char *string, int mode,unsigned char color)
{
    int i,len;

    if(mode == LEFT)
    for(i=0;string[i] != '\0';i++)
    {
     putfnt_trans(x,y,string[i],color);
     x+=font_width+1;
    }

    else if(mode == CENTER)
    {
	x = (320-(strlen(string)*font_width))>>1;
	
        for(i=0;string[i] != '\0';i++)
        {
         putfnt_trans(x,y,string[i],color);
         x+=font_width+1;
        }
    }
}

