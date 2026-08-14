#include "cotypes.h"
#include <stdio.h>

void load_pcx(char filename[],unsigned char *buffer,unsigned char *palette);
void load_pcxFN(char filename[],unsigned char *buffer,unsigned char *palette);
void load_pcxFP(FILE *fp,unsigned char *buffer,unsigned char *palette);
void get_pcx_pal(char filename[],unsigned char *palette);

