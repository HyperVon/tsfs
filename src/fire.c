unsigned char firemap[16384],firelut[256],fire_buffer[4224];

void Make_Fire_Palette(unsigned char *pal_orig)
{
        int index,i;
        unsigned char pal[768];

	for (index=0; index<64; index++)
	{
                pal[index*3] = index;
                pal[index*3+1] = 0;
                pal[index*3+2] = 0;

                pal[(index+64)*3] = 63;
                pal[(index+64)*3+1] = index;
                pal[(index+64)*3+2] = 0;

                pal[(index+128)*3] = 63;
                pal[(index+128)*3+1] = 63;
                pal[(index+128)*3+2] = index/2;

                pal[(index+192)*3] = 63;
                pal[(index+192)*3+1] = 63;
                pal[(index+192)*3+2] = 31+(index/2);
	}
        pal[0] = pal[1] = pal[2] = 0;

        for(i=0;i<256;i++)
         firelut[i] = findRGB(pal[i*3],pal[i*3+1],pal[i*3+2],pal_orig);

}

void fire(void)
{
        int x,y,i,m;
        int color;
        static unsigned char first=1;
        unsigned char *src,*y_start_fire,*y_start_buffer;

        if(first)
        {
         memset(fire_buffer,0,66*64);
         memset(firemap,0,16384);
         first=0;
        }

                for (y=64;y<66;y++)
                {
                 m=(y<<6);
                 for (x=0;x<64;x++)
                  fire_buffer[m+x] = rand() & 0x00FF;
                }
		
                for (y=1;y<66;y++)
		{
                    y_start_fire = fire_buffer + (y<<6);
                for (x=1;x<63;x++)
                {
		  src = y_start_fire + x;

                  color = (*src + *(src-64) + *(src+64) +
                                    *(src-1) + *(src+1))/5;
                  color -= rand()%6;
                  if (color<0) color=0;
                  *(src-64) = color;
                }
		}
		
/*                for(y=0;y<252;y+=4)
		{
                    y_start_fire = fire_buffer + (y<<4);
                    y_start_buffer = firemap + (y<<8);
                for(x=0;x<252;x+=4)
		{
                    color = firelut[*(y_start_fire + (x>>2))];
		    src = y_start_buffer  + x;
		    
		    *src = color;
		    *(src+1) = color;
		    *(src+2) = color;
		    *(src+3) = color;
		    
                    *(src+256) = color;
                    *(src+257) = color;
                    *(src+258) = color;
                    *(src+259) = color;

                    *(src+512) = color;
                    *(src+513) = color;
                    *(src+514) = color;
                    *(src+515) = color;

                    *(src+768) = color;
                    *(src+769) = color;
                    *(src+770) = color;
                    *(src+771) = color;
		}
		}
                */
                /*
                for(y=0;y<126;y+=2)
		{
                    y_start_fire = fire_buffer + (y<<5);
                    y_start_buffer = firemap + (y<<7);
                for(x=0;x<126;x+=2)
		{
                    color = *(y_start_fire + (x>>1));
		    src = y_start_buffer  + x;
		    
		    *src = color;
		    *(src+1) = color;
		    
                    *(src+128) = color;
                    *(src+129) = color;

                    *(src+256) = color;
                    *(src+257) = color;

                    *(src+384) = color;
                    *(src+385) = color;
		}
		}
                */

}
