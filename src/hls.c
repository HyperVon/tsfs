#include "hls.h"

void RGB_to_HLS(float r, float g, float b, float *h, float *l, float *s)
{
        float max,min,delta;
        // Get Max
        if(r > g)
         if(r > b) max = r;
         else max = b;
        else
         if(g > b) max = g;
         else max = b;
         // Get Min
        if(r < g)
         if(r < b) min = r;
         else min = b;
        else
         if(g < b) min = g;
         else min = b;

          

         *l=(max+min)/2.0;
         if(max==min)
         {
                *s=0;
                *h=UNDEFINED;
         }
         else
         {
                delta=max-min;
                if (*l<=0.5) *s=delta/(max+min);
                else *s=delta/(2.0 - (max+min));

                if (r==max)
                        *h=(g-b)/delta;
                else if (g==max)
                        *h=2.0 + (b-r)/delta;
                else if (b==max)
                        *h=4.0 + (r-g)/delta;

                *h*=60;

                if(h<0) *h+=360;
           }
}

float value(float n1, float n2, float hue)
{
        if (hue > 360)
         hue-=360;
        else if (hue<0)
         hue+=360;
        if (hue<60)
         return(n1+(n2-n1)*hue/60);
        else if(hue<180)
         return(n2);
        else if(hue<240)
         return(n1+(n2-n1)*(240-hue)/60.0);
        else
         return(n1);
}

void HLS_to_RGB(float h, float l, float s, float *r, float *g, float *b)
{
        float m1,m2;

        if(l<=0.5) m2=(l*(1+s));
        else m2=(l+s-l*s);
        m1 = 2.0*l-m2;
        if (s==0)
        {
                if (h==UNDEFINED)
                 *r=*g=*b=l;
                else printf("Error\n");
        }
        else
        {
                *r=value(m1,m2,h+120);
                *g=value(m1,m2,h);
                *b=value(m1,m2,h-120);
        }
}
