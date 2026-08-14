#ifndef _LUTS_H_
#define _LUTS_H_

unsigned char LUT[16384];
unsigned char TRANSPAR[65536];
float L[256][64];


int findRGB(unsigned char R, unsigned char G, unsigned char B,unsigned char *pal);
void make_shade_lut(unsigned char *pal);
void make_trans_lut(unsigned char *pal,float trans);

#endif
