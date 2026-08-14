#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include "pcx.h" 

#pragma aux set_mode = \
"int 0x10" \
parm caller [eax]\
modify [eax];

char exclude[1]={0};

unsigned char *buffer;
unsigned char *dest;


void anti_alias(unsigned char *src,unsigned char *dest);

char *pal_table;
char pal[256][3];
unsigned char pal2[768];

void main(void)
{
float start,end;  
int frames=0;
int i;


FILE *fp;

unsigned char  *p;  // this is to solve the problem I have with Watcom
p=(unsigned char *)malloc(5*65536);  // (when I do 2 mallocs one after another (40k and 262k)
                    // beginning of code all subsequent mallocs work fine.
                    

dest=(unsigned char *)0x0a0000;set_mode(0x13);

buffer=(unsigned char *)malloc(64000);
memset(buffer,0,64000);
load_pcx("cointro2.pcx",p,pal2);

//right shift all pal rgb vals by 1
for(i=0;i<256;i++)
{
 pal2[i*3] >>= 1;
 pal2[i*3+1] >>= 1;
 pal2[i*3+2] >>= 1;
}


//load rgb LUT from file
//(make a LUT for a pcx by using 'mtbl.exe <pcx_name>')

pal_table=(char *)malloc(262144);
fp=fopen("pal.dat","rb");
fread(pal_table,64*64*64,1,fp);
fclose(fp);


buffer[100*320+160] = *(pal_table+(63<<12)+(63<<6)+(63));
buffer[99*320+160] = *(pal_table+(63<<12)+(63<<6)+(63));
buffer[101*320+160] = *(pal_table+(63<<12)+(63<<6)+(63));
buffer[100*320+161] = *(pal_table+(63<<12)+(63<<6)+(63));
buffer[100*320+159] = *(pal_table+(63<<12)+(63<<6)+(63));

memcpy(dest,buffer,64000); // see what it's like before anti-aliasing
getch();

frames=6;
start=clock();
for(i=0;i<frames;i++) 
{
anti_alias(buffer,buffer);
//memcpy(buffer,dest,64000); //uncomment this for a nice melting effect :)
memcpy(dest,buffer,64000); //uncomment this for a nice melting effect :)
}
end=clock();

set_mode(0x3);
set_mode(0x3);

cprintf("FPS : %f",(float)frames/(float)((end-start)/100.0));
}




// this function assumes that 'pal_table' is the rgb LUT... and that
// pal is the current pallette

void anti_alias(unsigned char *src,unsigned char *dest)
{
int x,y;
int cols,col1s,col2s,col3s,col4s;
int r,g,b,r1,g1,b1,r2,g2,b2,r3,g3,b3,r4,g4,b4;
unsigned char *temp;

int yq;
yq=0;
for(y=1;y<=198;y++)
{
yq=(y<<6)+(y<<8);  // y*320
yq++; // skip one pixel on the left border
for(x=1;x<=318;x++)
{
temp = src+yq;
cols=(*(temp)<<1) + *(temp);
col1s=(*(temp-320)<<1) + *(temp-320);
col2s=(*(temp+320)<<1) + *(temp+320);
col3s=(*(temp+1)<<1) + *(temp+1);
col4s=(*(temp-1)<<1) + *(temp-1);


r=pal2[cols];g=pal2[cols+1];b=pal2[cols+2];
r1=pal2[col1s];g1=pal2[col1s+1];b1=pal2[col1s+2];
r2=pal2[col2s];g2=pal2[col2s+1];b2=pal2[col2s+2];
r3=pal2[col3s];g3=pal2[col3s+1];b3=pal2[col3s+2];
r4=pal2[col4s];g4=pal2[col4s+1];b4=pal2[col4s+2];

//pal vals preshifted by 1 on the right
r+=((r1+r2+r3+r4)>>2);        // (r/2+r1/8+r2/8+r3/8+r4/8)
g+=((g1+g2+g3+g4)>>2);
b+=((b1+b2+b3+b4)>>2);

*(dest+yq)=*(pal_table+(r<<12)+(g<<6)+(b));
yq++;
}
}
}
