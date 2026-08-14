#include "luts.h"

int findRGB(unsigned char R, unsigned char G, unsigned char B,unsigned char *pal)
{
 int i,best_match,error2,temp_error;


 error2 = 1000000000;
 for(i=0;i<768;i+=3)
 {
  temp_error =      ((R - pal[i]) * (R - pal[i]) +
                    (G - pal[i+1]) * (G - pal[i+1]) +
                    (B - pal[i+2]) * (B - pal[i+2]));

  if(temp_error < error2)
  {
    best_match = i/3;
    error2 = temp_error;
  }
 }

  return best_match;
} 

void make_shade_lut(unsigned char *pal)
{
    int i,k,j;
    static float *h,*l,*s,*float_pal,temp,inc,r,g,b;
    float error2,temp_error;
    int best_match;

    h = (float *) malloc (sizeof(float)*256);
    l = (float *) malloc (sizeof(float)*256);
    s = (float *) malloc (sizeof(float)*256);
    float_pal = (float *) malloc (sizeof(float)*768);

    for(i=0;i<768;i++)
     float_pal[i] = pal[i];

    //normalize rgbs
    for(i=0;i<768;i+=3)
    {
      float_pal[i]   /= 64.;
      float_pal[i+1] /= 64.;
      float_pal[i+2] /= 64.;

      RGB_to_HLS(float_pal[i],float_pal[i+1],float_pal[i+2],
                    &h[i/3],&l[i/3],&s[i/3]);
    }

    //interpolate luminance value
    for(i=0;i<256;i++)
    {
      //interpolate l from 0 -> l  in steps of 32
      temp=0;
      inc = l[i]/48.;
      for(j=0;j<48;j++)
      {
        temp+=inc;
        if(temp <= 1.0)
         L[i][j]=temp;
        else
         L[i][j]=1.0;
      }

      //interpolate l from l -> 1.0 in steps of 32
      temp=l[i];
      inc = (1-temp)/32.;
      for(j=48;j<64;j++)
      {
       temp+=inc;
       if(temp <= 1.0)
        L[i][j]=temp;
       else
        L[i][j]=1.0;
      }
    }

    for(i=0;i<64;i++)
    {
	for(k=0;k<256;k++)
	{
            HLS_to_RGB(h[k],L[k][i],s[k],&r,&g,&b);

            r*=64;
            g*=64;
            b*=64;

            error2=1000000000;

            for(j=0;j<768;j+=3)
	    {
                temp_error = ((pal[j]-r)   * (pal[j]-r)) +
                             ((pal[j+1]-g) * (pal[j+1]-g)) +
                             ((pal[j+2]-b) * (pal[j+2]-b));
                             
                if(temp_error < error2)
		{
                    best_match = j/3;
                    error2 = temp_error;
		}
	    }
            printf("%3.0f%%\r",((float)i/64.)*100.);
	    LUT[(k<<6)+i]=best_match;
	}
    }
    free(h);
    free(l);
    free(s);
    free(float_pal);
}

void make_trans_lut(unsigned char *pal,float trans)
{
    int i,j,k,error2,best_match,index;
    float how_close;
    float Trans_RED,Trans_GREEN,Trans_BLUE;
    float *red,*green,*blue;

    red = (float *) malloc (sizeof(float)*256);
    green = (float *) malloc (sizeof(float)*256);
    blue = (float *) malloc (sizeof(float)*256);


    //strip out rgb components into separate arrays
    for(index=i=0;i<768;i+=3)
    {
	red[index] = pal[i];
	green[index] = pal[i+1];
	blue[index] = pal[i+2];
	index++;
    }

    for(i=0;i<256;i++)
    {
        printf("Processing Row: %d\r",i);

	for(j=0;j<256;j++)
	{

	    //calculate transparent color
	    Trans_RED = red[i]*trans+red[j]*(1-trans);
	    Trans_GREEN = green[i]*trans+green[j]*(1-trans);
	    Trans_BLUE = blue[i]*trans+blue[j]*(1-trans);

	    //set error to impossible value
            error2 = 256000;
	    best_match = 0;
	    for(k=0;k<256;k++)
	    {
		//calculate space between color that we want (Trans) and the
		//current color in the palette (k)
                how_close = ((Trans_RED - red[k]) * (Trans_RED - red[k]) +
			          (Trans_GREEN - green[k]) * (Trans_GREEN - green[k]) +
				  (Trans_BLUE - blue[k]) * (Trans_BLUE - blue[k]));
                if(how_close < error2)
		{
		    best_match = k;
                    error2 = how_close;
		}
	    }
	    //best_match is the closest color to (Trans) in the palette
	    // j=foreground index i=background index
            TRANSPAR[(j<<8)+i] = best_match;
	}
    }
    free(red);
    free(green);
    free(blue);
}
