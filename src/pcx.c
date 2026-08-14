#include "pcx.h"

void load_pcx(char filename[],unsigned char *buffer,unsigned char *palette)
{
    FILE *fp;
    int i,index=0,j;
    PCXHEAD header;
    short width,depth;
    short bytes;
    unsigned char c, run;
    unsigned int n;
    
    if ((fp = fopen(filename,"rb")) != NULL)
    {
      if (fread((char *)&header,1,sizeof(PCXHEAD),fp) == sizeof(PCXHEAD))
      {
	if (header.manufacturer == 0x0a && header.version == 5)
	{
	  if (!fseek(fp,-769L,SEEK_END))
	  {
	    if (fgetc(fp) == 0x0c && fread(palette,1,768,fp) == 768)
	    {
	      fseek(fp,128L,SEEK_SET);
	      width = header.xmax - header.xmin + 1;
	      depth = header.ymax - header.ymin + 1;
	      bytes = header.bytes_per_line;

  for (i = 0; i < 768; i++) 
   palette[i] = palette[i] >> 2;

  for(j=0;j<depth;j++)
  {
      n=0;
  do
  {
    c = fgetc(fp) & 0xff;
    /* if it's a run of bytes field */
    if ((c & 0xc0) == 0xc0)
    {
      /* and off the high bits */
      run = c & 0x3f;
      /* get the run byte */
      c = fgetc(fp);
      /* run the byte */
      n += run;
      for(i=0;i<run;i++)
	 buffer[index++]=c;
    }
    else
    {
      n++;
      buffer[index++]=c;
    }
  }
  while (n < bytes);
  }
  
	    }
	    else { printf("Error reading palette\n"); exit(1);}
	  }
	  else {printf("Error seeking to palette\n"); exit(1); }
	}
	else{ printf("Not a 256 color PCX file\n"); exit(1);}
      }
      else {printf("Error reading %s\n",filename); exit(1); }
    }
    else {printf("Error opening %s\n",filename); exit(1);}
    fclose(fp);
}
  

void load_pcxFN(char filename[],unsigned char *buffer,unsigned char *palette)
{
    FILE *fp;
    int i,index=0,j;
    PCXHEAD header;
    short width,depth;
    short bytes;
    unsigned char c, run;
    unsigned int n;
    
    if ((fp = fopen(filename,"rb")) != NULL)
    {
      if (fread((char *)&header,1,sizeof(PCXHEAD),fp) == sizeof(PCXHEAD))
      {
	if (header.manufacturer == 0x0a && header.version == 5)
	{
	      width = header.xmax - header.xmin + 1;
	      depth = header.ymax - header.ymin + 1;
	      bytes = header.bytes_per_line;


  for(j=0;j<depth;j++)
  {
      n=0;
  do
  {
    c = fgetc(fp) & 0xff;
    /* if it's a run of bytes field */
    if ((c & 0xc0) == 0xc0)
    {
      /* and off the high bits */
      run = c & 0x3f;
      /* get the run byte */
      c = fgetc(fp);
      /* run the byte */
      n += run;
      for(i=0;i<run;i++)
	 buffer[index++]=c;
    }
    else
    {
      n++;
      buffer[index++]=c;
    }
  }
  while (n < bytes);
  }

  if (fgetc(fp) == 0x0c && fread(palette,1,768,fp) == 768)
   printf("palette ok\n");
  else
   printf("Palette not ok\n");

  for (i = 0; i < 768; i++)
   palette[i] = palette[i] >> 2;

        }
	else{ printf("Not a 256 color PCX file\n"); exit(1);}
      }
      else {printf("Error reading %s\n",filename); exit(1); }
    }
    else {printf("Error opening %s\n",filename); exit(1);}
    fclose(fp);
}

void load_pcxFP(FILE *fp,unsigned char *buffer,unsigned char *palette)
{
    int i,index=0,j;
    PCXHEAD header;
    short width,depth;
    short bytes;
    unsigned char c, run;
    unsigned int n;
    
    if (fp != NULL)
    {
      if (fread((char *)&header,1,sizeof(PCXHEAD),fp) == sizeof(PCXHEAD))
      {
	if (header.manufacturer == 0x0a && header.version == 5)
	{
	      width = header.xmax - header.xmin + 1;
	      depth = header.ymax - header.ymin + 1;
	      bytes = header.bytes_per_line;


  for(j=0;j<depth;j++)
  {
      n=0;
  do
  {
    c = fgetc(fp) & 0xff;
    /* if it's a run of bytes field */
    if ((c & 0xc0) == 0xc0)
    {
      /* and off the high bits */
      run = c & 0x3f;
      /* get the run byte */
      c = fgetc(fp);
      /* run the byte */
      n += run;
      for(i=0;i<run;i++)
	 buffer[index++]=c;
    }
    else
    {
      n++;
      buffer[index++]=c;
    }
  }
  while (n < bytes);
  }

  if (!(fgetc(fp) == 0x0c && fread(palette,1,768,fp) == 768))
   printf("Palette Read Error.\n");

  for (i = 0; i < 768; i++)
   palette[i] = palette[i] >> 2;

        }
	else{ printf("Not a 256 color PCX file\n"); exit(1);}
      }
      else {printf("Error reading file\n"); exit(1); }
    }
    else {printf("Error opening file\n"); exit(1);}
//    fclose(fp);
}

void get_pcx_pal(char filename[],unsigned char *palette)
{
    FILE *fp;
    int i,index=0,j;
    PCXHEAD header;
    short width,depth;
    short bytes;
    unsigned char c, run;
    unsigned int n;
    
    if ((fp = fopen(filename,"rb")) == NULL)
    {
     printf("PCX PAL LOAD ERROR!\n");
     exit(1);
    }

     if (fread((char *)&header,1,sizeof(PCXHEAD),fp) == sizeof(PCXHEAD))
      if (header.manufacturer == 0x0a && header.version == 5)
        if (!fseek(fp,-769L,SEEK_END))
          if(fgetc(fp) ==  0x0C)
            if(fread(palette,1,768,fp) != 768)
            {
             printf("PAL LOAD ERROR\n");
             exit(1);
            }

  for (i = 0; i < 768; i++) 
   palette[i] = palette[i] >> 2;

            
    fclose(fp);
}
