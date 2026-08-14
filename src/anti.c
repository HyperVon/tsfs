unsigned char pal_table[262144];
unsigned char anti_aliased_buffer[64000];

pre_right_shift_pal(unsigned char *dest_pal, unsigned char *src_pal)
{
 int i;

 for(i=0;i<768;i++)
  dest_pal[i] = (src_pal[i]>>1);
}

void anti_alias(unsigned char *dest,unsigned char *src,unsigned char *pal)
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


r=pal[cols];g=pal[cols+1];b=pal[cols+2];
r1=pal[col1s];g1=pal[col1s+1];b1=pal[col1s+2];
r2=pal[col2s];g2=pal[col2s+1];b2=pal[col2s+2];
r3=pal[col3s];g3=pal[col3s+1];b3=pal[col3s+2];
r4=pal[col4s];g4=pal[col4s+1];b4=pal[col4s+2];

//pal vals preshifted by 1 on the right
r+=((r1+r2+r3+r4)>>2);        // (r/2+r1/8+r2/8+r3/8+r4/8)
g+=((g1+g2+g3+g4)>>2);
b+=((b1+b2+b3+b4)>>2);

*(dest+yq)=*(pal_table+(r<<12)+(g<<6)+b);
yq++;
}
}
}
