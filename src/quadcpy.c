#include <stdio.h>
#include <time.h>

unsigned char *screen = (unsigned char *)0x0A0000;

void quadcpy(unsigned char *Destination,unsigned char *Source, int Length);
#pragma aux quadcpy = " rep movsd "\
parm [EDI] [ESI] [ECX]\
modify [ESI EDI ECX];
 
inline crap(void)
{
 int i;
  i+=2;
}

main()
{
 float start,end;
 unsigned char *buf1,*buf2;
 int i;

 buf1 = (unsigned char *) malloc(sizeof(unsigned char)*64000);
 buf2 = (unsigned char *) malloc(sizeof(unsigned char)*64000);

 printf("normal memcpy\n");
 start=clock();
 for(i=0;i<6000;i++)
  memcpy(screen,buf2,64000);
 end=clock();

 printf("Time: %f\n",((end-start)/100.0));

 printf("normal memcpy\n");
 start=clock();
 for(i=0;i<6000;i++)
  quadcpy(screen,buf2,16000);
 end=clock();

 printf("Time: %f\n",((end-start)/100.0));

}

