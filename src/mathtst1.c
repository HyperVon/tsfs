#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "cotypes.h"
#include "fixed32.h"

main()
{
 float start,end,a,b,c;
 Fixed32 d,e,f;
 int i;

 start = clock();
 for(i=0;i<9000000;i++)
 {
  a = rand();
  b = rand() + 1.0;
  c = a/b;
  c = a + b;
  c = a * b;
  c = a - b;
 }
 end = clock();

 printf("elapsed time(float): %f\n",(end-start)/100.0);

 start = clock();
 for(i=0;i<9000000;i++)
 {
  e = rand();
  d = rand() + 1;
  f = fixeddiv(e,d);
  f = fixedmul(e,d);
  f = e - d;
  f = e + d;
 }
 end = clock();

 printf("elapsed time(fixed): %f\n",(end-start)/100.0);
 
 //printf("Div by 0: %d\n",fixeddiv(56,0));
}

