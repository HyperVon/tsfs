#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include <mem.h>
#include <time.h>
#include "pcx.h"

unsigned char match_color(int r,int g,int b,char pal[256][3]);

void main(int argc,char *argv[])
{
FILE *fp;
unsigned char *pal_table;
unsigned char pal[256][3],pal2[768];
int i;

if(argc < 3)
{
 printf("Usage: mtbl file.pcx file.lut\n");
 exit(1);
}
printf("loading palette...");
get_pcx_pal(argv[1],pal2);

printf("\nAdjusting palette...");
for(i=0;i<256;i++)
{
 pal[i][0] = pal2[i*3];
 pal[i][1] = pal2[i*3+1];
 pal[i][2] = pal2[i*3+2];
}
printf("\nBuilding palette table ");
 
// make a pallete table ([63][63][63])
pal_table=(unsigned char *)malloc(262144*sizeof(unsigned char));
build_table(pal,pal_table);
fp=fopen(argv[2],"wb");
fwrite(pal_table,64*64*64,1,fp);
fclose(fp);

free(pal_table);
}

unsigned char match_color(int r,int g,int b,unsigned char pal[256][3])
{
unsigned char col;
int min=0x300000,k;
int i;
for(i=0;i<=255;i=i+1)
{
if((k=(pal[i][0]-r)*(pal[i][0]-r)+(pal[i][1]-g)*(pal[i][1]-g)+(pal[i][2]-b)*(pal[i][2]-b))<min)
{
min=k;
col=i;
}
}
//if(r==63 && g==63 && b==63){set_mode(0x3);cprintf("\n\r63,63,63 %d,  %d,%d,%d\n\r",col,pal[col][0],pal[col][1],pal[col][2]);}
return col;
}


void build_table(unsigned char pal[256][3],unsigned char *table)
{
int r,g,b;
for(r=0;r<=63;r=r+1)
{
putch('.');
for(g=0;g<=63;g=g+1)
for(b=0;b<=63;b=b+1)
{
*(table+r*4096+g*64+b)=match_color(r,g,b,pal);
}
}
}
