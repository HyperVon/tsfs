#ifndef _GLOBALS_H_
#define _GLOBALS_H_

extern unsigned char  *video_buffer = (char *)0x0a0000;
extern unsigned char  *buffer = (char *) 0x0a0000;
extern unsigned char  *texturemap;
extern int intensity;
extern const int _319 = 319<<16, _199 = 199<<16, _1 = 65536;
extern int TEXTURE_WIDTH=256;
extern int TEXTURE_HEIGHT=256;

extern unsigned char LUT[16384];
extern unsigned char TRANSPAR[65536];
extern float L[256][64];

#endif
