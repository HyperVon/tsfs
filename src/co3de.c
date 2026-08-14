#include "co3de.h"


void gouraud(TEXTURE * vtx,unsigned char color,unsigned char transparent)
{
    TEXTURE * v1 = vtx;
    TEXTURE * v2 = vtx+1;
    TEXTURE * v3 = vtx+2;
    TEXTURE *vt,*v1a,*v2a;
    int height,heighta,temp,longest,didx,width,x1,i,di;
    char *dest,*destptr;
    TEXTURE * left_array[3], * right_array[3];
    int left_section, right_section;
    int left_section_height, right_section_height;
    int left_i, delta_left_i;
    int left_x, delta_left_x, right_x, delta_right_x;
    int pre_calc_color;


    pre_calc_color = (color<<6);



    // Sort the triangle so that v1 points to the topmost, v2 to the
    // middle and v3 to the bottom vertex.
     
    if(v1->y > v2->y)
    {
      vt = v1;
      v1 = v2;
      v2 = vt;
    }
    if(v1->y > v3->y)
    {
      vt = v1;
      v1 = v3;
      v3 = vt;
    }
    if(v2->y > v3->y)
    {
      vt = v2;
      v2 = v3;
      v3 = vt;
    }

    if( (v3->y < 0 || v1->y > 199) || (v1->x < 0 && v2->x < 0 && v3->x < 0) ||
                              (v1->x > 319 && v2->x > 319 && v3->x > 319))
      return;

    // We start out by calculating the length of the longest scanline.

    height = v3->y - v1->y;
    if(height == 0)
        return;
    temp = ((v2->y - v1->y) << 16) / height;
    longest = temp * (v3->x - v1->x) + ((v1->x - v2->x) << 16);
    if(longest == 0)
        return;

    // Now that we have the length of the longest scanline we can use that 
    // to tell us which is left and which is the right side of the triangle.

    if(longest < 0)
    {
        // If longest is neg. we have the middle vertex on the right side.
        // Store the pointers for the right and left edge of the triangle.
        right_array[0] = v3;
        right_array[1] = v2;
        right_array[2] = v1;
        right_section  = 2;
        left_array[0]  = v3;
        left_array[1]  = v1;
        left_section   = 1;

        // Calculate initial left and right parameters
        v1a = v1;
        v2a = v3;
        

        heighta = v2a->y - v1a->y;
        if(heighta > 0)
        {
         // Calculate the deltas along this section
        
         delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
         left_x = v1a->x << 16;
         delta_left_i = ((v2a->i - v1a->i) << 10) / heighta;
         left_i = v1a->i << 10;

         left_section_height = heighta;
        }
//        else
//         return;
     if(heighta <= 0) return;

     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

        if(heighta <= 0)
        {
            // The first right section had zero height. Use the next section. 
            right_section--;
     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

            if(heighta <= 0)
                return;
        }

        // Ugly compensation so that the dudx,dvdx divides won't overflow
        // if the longest scanline is very short.
        if(longest > -0x1000)
            longest = -0x1000;     
    }
    else
    {
        // If longest is pos. we have the middle vertex on the left side.
        // Store the pointers for the left and right edge of the triangle.
        left_array[0]  = v3;
        left_array[1]  = v2;
        left_array[2]  = v1;
        left_section   = 2;
        right_array[0] = v3;
        right_array[1] = v1;
        right_section  = 1;

        // Calculate initial right and left parameters
     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

        if(heighta <= 0)
            return;

        v1a = v1;
        v2a = v2;
        

        heighta = v2a->y - v1a->y;
        if(heighta > 0)
        {
         // Calculate the deltas along this section
        
         delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
         left_x = v1a->x << 16;
         delta_left_i = ((v2a->i - v1a->i) << 10) / heighta;
         left_i = v1a->i << 10;

         left_section_height = heighta;
        }
//        else
//         return;

        if(heighta <= 0)
        {
            // The first left section had zero height. Use the next section.
            left_section--;
                v1a = left_array[ left_section ];
                v2a = left_array[ left_section-1 ];
                
        
                heighta = v2a->y - v1a->y;
                if(heighta > 0)
                {
                 // Calculate the deltas along this section
                
                 delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
                 left_x = v1a->x << 16;
                 delta_left_i = ((v2a->i - v1a->i) << 10) / heighta;
                 left_i = v1a->i << 10;
        
                 left_section_height = heighta;
                }
//                else
//                 return;

            if(heighta <= 0)
                return;
        }

        // Ugly compensation so that the dudx,dvdx divides won't overflow
        // if the longest scanline is very short.
        if(longest < 0x1000)
            longest = 0x1000;     
    }

    // Now we calculate the constant deltas for u and v (dudx, dvdx)

    didx = shl10idiv(temp*(v3->i - v1->i)+((v1->i - v2->i)<<16),longest);
//    printf("2 divides\n");

    destptr = (char *)((v1->y<<6)+(v1->y<<8) + buffer);

    // If you are using a table lookup inner loop you should setup the
    // lookup table here.

    // Here starts the outer loop (for each scanline)
    didx >>= 8;

    if(!transparent)
    for(;;)         
    {
        x1 = left_x >> 16;
        width = (right_x >> 16) - x1;

        if(width > 0 && destptr >= buffer && destptr < (buffer+63680) && x1 < 319)
//        if(width > 0)
        {
            // This is the inner loop setup and the actual inner loop.
            // If you keep everything else in C that's up to you but at 
            // least remove this inner loop in C and insert some of 
            // the Assembly versions.

            
	    if(x1 < 0)
	    {
	        dest = destptr;
                width += x1;
                i = fixedmul(didx,-left_x)+(left_i >> 8);
	    }
	    else
            {
	     dest = destptr + x1;
             i  = left_i >> 8;
            }

            if(width+x1 > 319)
	    {
		if(x1 >= 0)
		 width = 319-x1;
		else
		 width = 319;
	    }



            // Watcom C/C++ 10.0 can't get this inner loop any tighter 
            // than about 10-12 clock ticks.
            if(width > 0)
	    do
            {
                *dest++ = LUT[pre_calc_color+(i>>8)];
                i += didx;
            }
            while(--width);
	    
        }

        destptr += 320;

        // Interpolate along the left edge of the triangle
        if(--left_section_height <= 0)  // At the bottom of this section?
        {
            if(--left_section <= 0)     // All sections done?
                return;
                v1a = left_array[ left_section ];
                v2a = left_array[ left_section-1 ];

        heighta = v2a->y - v1a->y;
        if(heighta > 0)
        {
         // Calculate the deltas along this section
        
         delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
         left_x = v1a->x << 16;
         delta_left_i = ((v2a->i - v1a->i) << 10) / heighta;
         left_i = v1a->i << 10;

         left_section_height = heighta;
        }
//        else
//         return;

            if(heighta <= 0)      // Nope, do the last section
                return;
        }
        else
        {
            left_x += delta_left_x;
            left_i += delta_left_i;
        }

        // Interpolate along the right edge of the triangle
        if(--right_section_height <= 0) // At the bottom of this section?
        {
            if(--right_section <= 0)    // All sections done?
                return;
     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

            if(heighta <= 0)     // Nope, do the last section
                return;
        }
        else
            right_x += delta_right_x;
    }

    else
    for(;;)         
    {
        x1 = left_x >> 16;
        width = (right_x >> 16) - x1;

        if(width > 0 && destptr >= buffer && destptr < (buffer+63680) && x1 < 319)
//        if(width > 0)
        {
            // This is the inner loop setup and the actual inner loop.
            // If you keep everything else in C that's up to you but at 
            // least remove this inner loop in C and insert some of 
            // the Assembly versions.

            
	    if(x1 < 0)
	    {
	        dest = destptr;
                width += x1;
                i = fixedmul(didx,-left_x)+(left_i >> 8);
	    }
	    else
            {
	     dest = destptr + x1;
             i  = left_i >> 8;
            }

            if(width+x1 > 319)
	    {
		if(x1 >= 0)
		 width = 319-x1;
		else
		 width = 319;
	    }



            // Watcom C/C++ 10.0 can't get this inner loop any tighter 
            // than about 10-12 clock ticks.
            if(width > 0)
	    do
            {
                *dest++ = TRANSPAR[(LUT[pre_calc_color+(i>>8)]<<8) + *dest];
                i += didx;
            }
            while(--width);
	    
        }

        destptr += 320;

        // Interpolate along the left edge of the triangle
        if(--left_section_height <= 0)  // At the bottom of this section?
        {
            if(--left_section <= 0)     // All sections done?
                return;
                v1a = left_array[ left_section ];
                v2a = left_array[ left_section-1 ];

        heighta = v2a->y - v1a->y;
        if(heighta > 0)
        {
         // Calculate the deltas along this section
        
         delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
         left_x = v1a->x << 16;
         delta_left_i = ((v2a->i - v1a->i) << 10) / heighta;
         left_i = v1a->i << 10;

         left_section_height = heighta;
        }
//        else
//         return;

            if(heighta <= 0)      // Nope, do the last section
                return;
        }
        else
        {
            left_x += delta_left_x;
            left_i += delta_left_i;
        }

        // Interpolate along the right edge of the triangle
        if(--right_section_height <= 0) // At the bottom of this section?
        {
            if(--right_section <= 0)    // All sections done?
                return;
     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

            if(heighta <= 0)     // Nope, do the last section
                return;
        }
        else
            right_x += delta_right_x;
    }

}

void hline(int x1, int x2, unsigned char color,unsigned char *buffer);
#pragma aux hline = \
"       add     edi, ebx " \
"       sub     ecx, ebx    " \
"       cmp     ecx, 0 " \
"       jl      negate " \
"       jmp     regular " \
"negate: neg ecx " \
"regular: shr     ecx, 1       " \
"       jnc     Start        " \
"       stosb                " \
"Start:          " \
"       mov     ah, al  " \
"       rep     stosw        " \
parm caller [ebx] [ecx] [al] [edi] \
modify [ebx ecx al edi];


void flat(int x1, int y1, int x2, int y2, int x3, int y3,
        unsigned char color,unsigned char shade,unsigned char transparent)
{
    Fixed32 line1[200], line2[200], line3[200];
    Fixed32 temp,xspecial1,xspecial3;
    Fixed32 d,x,y,ax,ay,dx,dy,sx,sy,dxf,xt,y2y1,y3y2,y3y1;
    unsigned char *dest,actual_color;

    actual_color = LUT[(color<<6)+shade];
//    actual_color=color;
    


    if((x1 == x2 && x2 == x3) || (y1 == y2 && y2 == y3))
       return;




    if (y1 > y2)
    {
	temp = y1;
	y1 = y2;
	y2 = temp;

	temp = x1;
	x1 = x2;
	x2 = temp;
    }

    if (y1 > y3)
    {
	  temp = y1;
	  y1 = y3;
	  y3 = temp; 

	  temp = x1;
	  x1 = x3;
	  x3 = temp;
    }

    if (y2 > y3)
    {
	temp = y2;
	y2 = y3;
	y3 = temp;

	temp = x2;
	x2 = x3;
	x3 = temp;
    }

    if( (y3 < 0 || y1 > _199) || (x1 < 0 && x2 < 0 && x3 < 0) ||
                              (x1 > _319 && x2 > _319 && x3 > _319))
      return;


    y2y1 = _1+y2-y1;
    y3y1 = _1+y3-y1;
    y3y2 = _1+y3-y2;

    dest = buffer+(fixed_to_int(y1)<<6)+ (fixed_to_int(y1)<<8);


    if(y1 != y2)
    {
       x=x1;
       y=fixed_to_int(y1);
       //lets do line1 first x1,y1 -> x2,y2 only if we have to !
       dxf = fixeddiv((x2-x1),y2y1);
       for(;y<=fixed_to_int(y2);y++)
       {
        if(y >= 0 && y < 200)
        {
         xt = fixed_to_int(x);
         if(xt < 0) xt = 0;
         if(xt > 319) xt = 319;
         line1[y] = xt;
        }
        x+=dxf;
       }
       xspecial1 = x-dxf;
    }

    if(y2 != y3)
    {
      x=x2;
      y=fixed_to_int(y2);
      dxf = fixeddiv((x3-x2),y3y2);
      for(;y<=fixed_to_int(y3);y++)
      {
        if(y >= 0 && y < 200)
        {
         xt = fixed_to_int(x);
         if(xt < 0) xt = 0;
         if(xt > 319) xt = 319;
         line2[y] = xt;
        }
        x+=dxf;
      }
    }

    x=x1;
    y=fixed_to_int(y1);
            dxf = fixeddiv((x3-x1),y3y1);
            for(;y<=fixed_to_int(y3);y++)
	    {
             if(y >= 0 && y < 200)
             {
              xt = fixed_to_int(x);
              if(xt < 0) xt = 0;
              if(xt > 319) xt = 319;
              line3[y] = xt;
             }
             if(y == fixed_to_int(y2))
              xspecial3 = x;
	     x+=dxf;
	    }


    if(transparent == 0)
    if(y1==y2)  //flat top triangle
    {

        //see what side x1 and x2 are on.. left or right
	//this case x1 is left of x2
        if(x1 < x2)
            for(y=fixed_to_int(y1); y<=fixed_to_int(y3);y++)
	    {
                if(y >=0 && y < 200)
                 hline(line3[y],line2[y],actual_color,dest);
		dest += 320;
	    }
    
	//this case x1 is right of x2
	else
            for(y=fixed_to_int(y1); y<=fixed_to_int(y3);y++)
	    {
               if(y >=0 && y < 200)
                 hline(line2[y],line3[y],actual_color,dest);
		dest += 320;
	    }
    }
    else if(y2 == y3) //flat bottom
    {
	
        //see what side x2 and x3 are on.. left or right
	//this case x2 is left of x3
        if(x2 < x3)
            for(y=fixed_to_int(y1);y<=fixed_to_int(y3);y++)
	    {
                if(y >=0 && y < 200)
                 hline(line1[y],line3[y],actual_color,dest);
		dest += 320;
	    }

	//this case x3 left of x2
	else
            for(y=fixed_to_int(y1);y<=fixed_to_int(y3);y++)
	    {
                if(y >=0 && y < 200)
                 hline(line3[y],line1[y],actual_color,dest);
		dest += 320;
	    }
    }
    //this means its not a flat top or bottom triangle...
    else
    {
//         if(line1[fixed_to_int(y2)] < line3[fixed_to_int(y2)])
         if(xspecial1 < xspecial3)
         {

            for(y=fixed_to_int(y1);y<fixed_to_int(y2);y++)
	    {
                if(y >=0 && y < 200)
                 hline(line1[y],line3[y],actual_color,dest);
		dest += 320;
	    }

	    //do the second half of triangle
	    //only recalculate the left interp value.. right should stay the same

            for(y=fixed_to_int(y2);y<=fixed_to_int(y3);y++)
	    {
                if(y >=0 && y < 200)
                 hline(line2[y],line3[y],actual_color,dest);
		dest += 320;
	    }
	    
	}
	else
	{

	    //do the first half of triangle

            for(y=fixed_to_int(y1);y<fixed_to_int(y2);y++)
	    {
                if(y >=0 && y < 200)
                 hline(line3[y],line1[y],actual_color,dest);
		dest += 320;
	    }

	    //do the second half of triangle
	    //only recalculate the right interp value.. left should stay the same

            for(y=fixed_to_int(y2);y<=fixed_to_int(y3);y++)
	    {
                if(y >=0 && y < 200)
                 hline(line3[y],line2[y],actual_color,dest);
		dest += 320;
	    }
	    
	}
    }
    else //it's transparent
    if(y1==y2)  //flat top triangle
    {

        //see what side x1 and x2 are on.. left or right
	//this case x1 is left of x2
        if(x1 < x2)
            for(y=fixed_to_int(y1); y<fixed_to_int(y3);y++)
	    {
                if(y >=0 && y < 200)
                for(x=line3[y];x<line2[y];x++)
                 *(dest + x) = TRANSPAR[(actual_color<<8)+ *(dest + x)];
		dest += 320;
	    }
    
	//this case x1 is right of x2
	else
            for(y=fixed_to_int(y1); y<fixed_to_int(y3);y++)
	    {
                if(y >=0 && y < 200)
                for(x=line2[y];x<line3[y];x++)
                 *(dest + x) = TRANSPAR[(actual_color<<8)+ *(dest + x)];

		dest += 320;
	    }
    }
    else if(y2 == y3) //flat bottom
    {
	
        //see what side x2 and x3 are on.. left or right
	//this case x2 is left of x3
        if(x2 < x3)
            for(y=fixed_to_int(y1);y<fixed_to_int(y3);y++)
	    {
                if(y >=0 && y < 200)
                for(x=line1[y];x<line3[y];x++)
                 *(dest + x) = TRANSPAR[(actual_color<<8)+ *(dest + x)];

		dest += 320;
	    }

	//this case x3 left of x2
	else
            for(y=fixed_to_int(y1);y<fixed_to_int(y3);y++)
	    {
                if(y >=0 && y < 200)
                for(x=line3[y];x<line1[y];x++)
                 *(dest + x) = TRANSPAR[(actual_color<<8)+ *(dest + x)];

		dest += 320;
	    }
    }
    //this means its not a flat top or bottom triangle...
    else
    {
//         if(line1[fixed_to_int(y2)] < line3[fixed_to_int(y2)])
         if(xspecial1 < xspecial3)
         {

            for(y=fixed_to_int(y1);y<fixed_to_int(y2);y++)
	    {
                if(y >=0 && y < 200)
                for(x=line1[y];x<line3[y];x++)
                 *(dest + x) = TRANSPAR[(actual_color<<8)+ *(dest + x)];

		dest += 320;
	    }

	    //do the second half of triangle
	    //only recalculate the left interp value.. right should stay the same

            for(y=fixed_to_int(y2);y<fixed_to_int(y3);y++)
	    {
                if(y >=0 && y < 200)
                for(x=line2[y];x<line3[y];x++)
                  *(dest + x) = TRANSPAR[(actual_color<<8)+ *(dest + x)];

		dest += 320;
	    }
	    
	}
	else
	{

	    //do the first half of triangle

            for(y=fixed_to_int(y1);y<fixed_to_int(y2);y++)
	    {
                if(y >=0 && y < 200)
                for(x=line3[y];x<line1[y];x++)
                  *(dest + x) = TRANSPAR[(actual_color<<8)+ *(dest + x)];

		dest += 320;
	    }

	    //do the second half of triangle
	    //only recalculate the right interp value.. left should stay the same

            for(y=fixed_to_int(y2);y<fixed_to_int(y3);y++)
	    {
                if(y >=0 && y < 200)
                for(x=line3[y];x<line2[y];x++)
                  *(dest + x) = TRANSPAR[(actual_color<<8)+ *(dest + x)];

		dest += 320;
	    }
	    
	}
    }
  
}

void texture(TEXTURE * vtx,unsigned char transparent)
{
    TEXTURE * v1 = vtx;
    TEXTURE * v2 = vtx+1;
    TEXTURE * v3 = vtx+2;
    TEXTURE *vt,*v1a,*v2a;
    int height,heighta,temp,longest,dudx,dvdx,width,x1,u,v,du,dv;
    char *dest,*destptr;
    TEXTURE * left_array[3], * right_array[3];
    int left_section, right_section;
    int left_section_height, right_section_height;
    int left_u, delta_left_u, left_v, delta_left_v;
    int left_x, delta_left_x, right_x, delta_right_x;


    // Sort the triangle so that v1 points to the topmost, v2 to the
    // middle and v3 to the bottom vertex.
     
    if(v1->y > v2->y)
    {
      vt = v1;
      v1 = v2;
      v2 = vt;
    }
    if(v1->y > v3->y)
    {
      vt = v1;
      v1 = v3;
      v3 = vt;
    }
    if(v2->y > v3->y)
    {
      vt = v2;
      v2 = v3;
      v3 = vt;
    }

    if( (v3->y < 0 || v1->y > 199) || (v1->x < 0 && v2->x < 0 && v3->x < 0) ||
                              (v1->x > 319 && v2->x > 319 && v3->x > 319))
      return;

    // We start out by calculating the length of the longest scanline.

    height = v3->y - v1->y;
    if(height == 0)
        return;
    temp = ((v2->y - v1->y) << 16) / height;
    longest = temp * (v3->x - v1->x) + ((v1->x - v2->x) << 16);
    if(longest == 0)
        return;

    // Now that we have the length of the longest scanline we can use that 
    // to tell us which is left and which is the right side of the triangle.

    if(longest < 0)
    {
        // If longest is neg. we have the middle vertex on the right side.
        // Store the pointers for the right and left edge of the triangle.
        right_array[0] = v3;
        right_array[1] = v2;
        right_array[2] = v1;
        right_section  = 2;
        left_array[0]  = v3;
        left_array[1]  = v1;
        left_section   = 1;

        // Calculate initial left and right parameters
        v1a = v1;
        v2a = v3;
        

        heighta = v2a->y - v1a->y;
        if(heighta > 0)
        {
         // Calculate the deltas along this section
        
         delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
         left_x = v1a->x << 16;
         delta_left_u = ((v2a->tx - v1a->tx) << 10) / heighta;
         left_u = v1a->tx << 10;
         delta_left_v = ((v2a->ty - v1a->ty) << 10) / heighta;
         left_v = v1a->ty << 10;

         left_section_height = heighta;
        }
//        else
//         return;
     if(heighta <= 0) return;

     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

        if(heighta <= 0)
        {
            // The first right section had zero height. Use the next section. 
            right_section--;
     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

            if(heighta <= 0)
                return;
        }

        // Ugly compensation so that the dudx,dvdx divides won't overflow
        // if the longest scanline is very short.
        if(longest > -0x1000)
            longest = -0x1000;     
    }
    else
    {
        // If longest is pos. we have the middle vertex on the left side.
        // Store the pointers for the left and right edge of the triangle.
        left_array[0]  = v3;
        left_array[1]  = v2;
        left_array[2]  = v1;
        left_section   = 2;
        right_array[0] = v3;
        right_array[1] = v1;
        right_section  = 1;

        // Calculate initial right and left parameters
     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

        if(heighta <= 0)
            return;

        v1a = v1;
        v2a = v2;
        

        heighta = v2a->y - v1a->y;
        if(heighta > 0)
        {
         // Calculate the deltas along this section
        
         delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
         left_x = v1a->x << 16;
         delta_left_u = ((v2a->tx - v1a->tx) << 10) / heighta;
         left_u = v1a->tx << 10;
         delta_left_v = ((v2a->ty - v1a->ty) << 10) / heighta;
         left_v = v1a->ty << 10;

         left_section_height = heighta;
        }
//        else
//         return;

        if(heighta <= 0)
        {
            // The first left section had zero height. Use the next section.
            left_section--;
                v1a = left_array[ left_section ];
                v2a = left_array[ left_section-1 ];
                
        
                heighta = v2a->y - v1a->y;
                if(heighta > 0)
                {
                 // Calculate the deltas along this section
                
                 delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
                 left_x = v1a->x << 16;
                 delta_left_u = ((v2a->tx - v1a->tx) << 10) / heighta;
                 left_u = v1a->tx << 10;
                 delta_left_v = ((v2a->ty - v1a->ty) << 10) / heighta;
                 left_v = v1a->ty << 10;
        
                 left_section_height = heighta;
                }
//                else
//                 return;

            if(heighta <= 0)
                return;
        }

        // Ugly compensation so that the dudx,dvdx divides won't overflow
        // if the longest scanline is very short.
        if(longest < 0x1000)
            longest = 0x1000;     
    }

    // Now we calculate the constant deltas for u and v (dudx, dvdx)

    dudx = shl10idiv(temp*(v3->tx - v1->tx)+((v1->tx - v2->tx)<<16),longest);
    dvdx = shl10idiv(temp*(v3->ty - v1->ty)+((v1->ty - v2->ty)<<16),longest);
//    printf("2 divides\n");

    destptr = (char *)((v1->y<<6)+(v1->y<<8) + buffer);

    // If you are using a table lookup inner loop you should setup the
    // lookup table here.

    // Here starts the outer loop (for each scanline)
    dudx >>= 8;
    dvdx >>= 8;

    if(!transparent)
    for(;;)         
    {
        x1 = left_x >> 16;
        width = (right_x >> 16) - x1;

        if(width > 0 && destptr >= buffer && destptr < (buffer+63680) && x1 < 319)
//        if(width > 0)
        {
            // This is the inner loop setup and the actual inner loop.
            // If you keep everything else in C that's up to you but at 
            // least remove this inner loop in C and insert some of 
            // the Assembly versions.

            
	    if(x1 < 0)
	    {
	        dest = destptr;
                width += x1;
                u = fixedmul(dudx,-left_x)+(left_u >> 8);
                v = fixedmul(dvdx,-left_x)+(left_v >> 8);
	    }
	    else
            {
	     dest = destptr + x1;
             u  = left_u >> 8;
             v  = left_v >> 8;
            }

            if(width+x1 > 319)
	    {
		if(x1 >= 0)
		 width = 319-x1;
		else
		 width = 319;
	    }



            // Watcom C/C++ 10.0 can't get this inner loop any tighter 
            // than about 10-12 clock ticks.
            if(width > 0)
	    do
            {
                *dest++ = texturemap[ (v & 0xff00) + ((u & 0xff00) >> 8) ];
                u += dudx;
                v += dvdx;
            }
            while(--width);
	    
        }

        destptr += 320;

        // Interpolate along the left edge of the triangle
        if(--left_section_height <= 0)  // At the bottom of this section?
        {
            if(--left_section <= 0)     // All sections done?
                return;
                v1a = left_array[ left_section ];
                v2a = left_array[ left_section-1 ];

        heighta = v2a->y - v1a->y;
        if(heighta > 0)
        {
         // Calculate the deltas along this section
        
         delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
         left_x = v1a->x << 16;
         delta_left_u = ((v2a->tx - v1a->tx) << 10) / heighta;
         left_u = v1a->tx << 10;
         delta_left_v = ((v2a->ty - v1a->ty) << 10) / heighta;
         left_v = v1a->ty << 10;

         left_section_height = heighta;
        }
//        else
//         return;

            if(heighta <= 0)      // Nope, do the last section
                return;
        }
        else
        {
            left_x += delta_left_x;
            left_u += delta_left_u;
            left_v += delta_left_v;
        }

        // Interpolate along the right edge of the triangle
        if(--right_section_height <= 0) // At the bottom of this section?
        {
            if(--right_section <= 0)    // All sections done?
                return;
     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

            if(heighta <= 0)     // Nope, do the last section
                return;
        }
        else
            right_x += delta_right_x;
    }
    //it's not transparent
    else
    for(;;)         
    {
        x1 = left_x >> 16;
        width = (right_x >> 16) - x1;

        if(width > 0 && destptr >= buffer && destptr < (buffer+63680) && x1 < 319)
//        if(width > 0)
        {
            // This is the inner loop setup and the actual inner loop.
            // If you keep everything else in C that's up to you but at 
            // least remove this inner loop in C and insert some of 
            // the Assembly versions.

            
	    if(x1 < 0)
	    {
	        dest = destptr;
                width += x1;
                u = fixedmul(dudx,-left_x)+(left_u >> 8);
                v = fixedmul(dvdx,-left_x)+(left_v >> 8);
	    }
	    else
            {
	     dest = destptr + x1;
             u  = left_u >> 8;
             v  = left_v >> 8;
            }

            if(width+x1 > 319)
	    {
		if(x1 >= 0)
		 width = 319-x1;
		else
		 width = 319;
	    }



            // Watcom C/C++ 10.0 can't get this inner loop any tighter 
            // than about 10-12 clock ticks.
            if(width > 0)
	    do
            {
                *dest++ = TRANSPAR[(texturemap[ (v & 0xff00) + ((u & 0xff00) >> 8) ]<<8)+ *dest];
                u += dudx;
                v += dvdx;
            }
            while(--width);
	    
        }

        destptr += 320;

        // Interpolate along the left edge of the triangle
        if(--left_section_height <= 0)  // At the bottom of this section?
        {
            if(--left_section <= 0)     // All sections done?
                return;
                v1a = left_array[ left_section ];
                v2a = left_array[ left_section-1 ];

        heighta = v2a->y - v1a->y;
        if(heighta > 0)
        {
         // Calculate the deltas along this section
        
         delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
         left_x = v1a->x << 16;
         delta_left_u = ((v2a->tx - v1a->tx) << 10) / heighta;
         left_u = v1a->tx << 10;
         delta_left_v = ((v2a->ty - v1a->ty) << 10) / heighta;
         left_v = v1a->ty << 10;

         left_section_height = heighta;
        }
//        else
//         return;

            if(heighta <= 0)      // Nope, do the last section
                return;
        }
        else
        {
            left_x += delta_left_x;
            left_u += delta_left_u;
            left_v += delta_left_v;
        }

        // Interpolate along the right edge of the triangle
        if(--right_section_height <= 0) // At the bottom of this section?
        {
            if(--right_section <= 0)    // All sections done?
                return;
     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

            if(heighta <= 0)     // Nope, do the last section
                return;
        }
        else
            right_x += delta_right_x;
    }

}

void tmapflat(TEXTURE * vtx,unsigned char transparent)
{
    TEXTURE * v1 = vtx;
    TEXTURE * v2 = vtx+1;
    TEXTURE * v3 = vtx+2;
    TEXTURE *vt,*v1a,*v2a;
    int height,heighta,temp,longest,dudx,dvdx,width,x1,u,v,du,dv;
    char *dest,*destptr;
    TEXTURE * left_array[3], * right_array[3];
    int left_section, right_section;
    int left_section_height, right_section_height;
    int left_u, delta_left_u, left_v, delta_left_v;
    int left_x, delta_left_x, right_x, delta_right_x;


    // Sort the triangle so that v1 points to the topmost, v2 to the
    // middle and v3 to the bottom vertex.
     
    if(v1->y > v2->y)
    {
      vt = v1;
      v1 = v2;
      v2 = vt;
    }
    if(v1->y > v3->y)
    {
      vt = v1;
      v1 = v3;
      v3 = vt;
    }
    if(v2->y > v3->y)
    {
      vt = v2;
      v2 = v3;
      v3 = vt;
    }

    if( (v3->y < 0 || v1->y > 199) || (v1->x < 0 && v2->x < 0 && v3->x < 0) ||
                              (v1->x > 319 && v2->x > 319 && v3->x > 319))
      return;

    // We start out by calculating the length of the longest scanline.

    height = v3->y - v1->y;
    if(height == 0)
        return;
    temp = ((v2->y - v1->y) << 16) / height;
    longest = temp * (v3->x - v1->x) + ((v1->x - v2->x) << 16);
    if(longest == 0)
        return;

    // Now that we have the length of the longest scanline we can use that 
    // to tell us which is left and which is the right side of the triangle.

    if(longest < 0)
    {
        // If longest is neg. we have the middle vertex on the right side.
        // Store the pointers for the right and left edge of the triangle.
        right_array[0] = v3;
        right_array[1] = v2;
        right_array[2] = v1;
        right_section  = 2;
        left_array[0]  = v3;
        left_array[1]  = v1;
        left_section   = 1;

        // Calculate initial left and right parameters
        v1a = v1;
        v2a = v3;
        

        heighta = v2a->y - v1a->y;
        if(heighta > 0)
        {
         // Calculate the deltas along this section
        
         delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
         left_x = v1a->x << 16;
         delta_left_u = ((v2a->tx - v1a->tx) << 10) / heighta;
         left_u = v1a->tx << 10;
         delta_left_v = ((v2a->ty - v1a->ty) << 10) / heighta;
         left_v = v1a->ty << 10;

         left_section_height = heighta;
        }
//        else
//         return;
     if(heighta <= 0) return;

     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

        if(heighta <= 0)
        {
            // The first right section had zero height. Use the next section. 
            right_section--;
     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

            if(heighta <= 0)
                return;
        }

        // Ugly compensation so that the dudx,dvdx divides won't overflow
        // if the longest scanline is very short.
        if(longest > -0x1000)
            longest = -0x1000;     
    }
    else
    {
        // If longest is pos. we have the middle vertex on the left side.
        // Store the pointers for the left and right edge of the triangle.
        left_array[0]  = v3;
        left_array[1]  = v2;
        left_array[2]  = v1;
        left_section   = 2;
        right_array[0] = v3;
        right_array[1] = v1;
        right_section  = 1;

        // Calculate initial right and left parameters
     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

        if(heighta <= 0)
            return;

        v1a = v1;
        v2a = v2;
        

        heighta = v2a->y - v1a->y;
        if(heighta > 0)
        {
         // Calculate the deltas along this section
        
         delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
         left_x = v1a->x << 16;
         delta_left_u = ((v2a->tx - v1a->tx) << 10) / heighta;
         left_u = v1a->tx << 10;
         delta_left_v = ((v2a->ty - v1a->ty) << 10) / heighta;
         left_v = v1a->ty << 10;

         left_section_height = heighta;
        }
//        else
//         return;

        if(heighta <= 0)
        {
            // The first left section had zero height. Use the next section.
            left_section--;
                v1a = left_array[ left_section ];
                v2a = left_array[ left_section-1 ];
                
        
                heighta = v2a->y - v1a->y;
                if(heighta > 0)
                {
                 // Calculate the deltas along this section
                
                 delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
                 left_x = v1a->x << 16;
                 delta_left_u = ((v2a->tx - v1a->tx) << 10) / heighta;
                 left_u = v1a->tx << 10;
                 delta_left_v = ((v2a->ty - v1a->ty) << 10) / heighta;
                 left_v = v1a->ty << 10;
        
                 left_section_height = heighta;
                }
//                else
//                 return;

            if(heighta <= 0)
                return;
        }

        // Ugly compensation so that the dudx,dvdx divides won't overflow
        // if the longest scanline is very short.
        if(longest < 0x1000)
            longest = 0x1000;     
    }

    // Now we calculate the constant deltas for u and v (dudx, dvdx)

    dudx = shl10idiv(temp*(v3->tx - v1->tx)+((v1->tx - v2->tx)<<16),longest);
    dvdx = shl10idiv(temp*(v3->ty - v1->ty)+((v1->ty - v2->ty)<<16),longest);
//    printf("2 divides\n");

    destptr = (char *)((v1->y<<6)+(v1->y<<8) + buffer);

    // If you are using a table lookup inner loop you should setup the
    // lookup table here.

    // Here starts the outer loop (for each scanline)
    dudx >>= 8;
    dvdx >>= 8;

    if(!transparent)
    for(;;)         
    {
        x1 = left_x >> 16;
        width = (right_x >> 16) - x1;

        if(width > 0 && destptr >= buffer && destptr < (buffer+63680) && x1 < 319)
//        if(width > 0)
        {
            // This is the inner loop setup and the actual inner loop.
            // If you keep everything else in C that's up to you but at 
            // least remove this inner loop in C and insert some of 
            // the Assembly versions.

            
	    if(x1 < 0)
	    {
	        dest = destptr;
                width += x1;
                u = fixedmul(dudx,-left_x)+(left_u >> 8);
                v = fixedmul(dvdx,-left_x)+(left_v >> 8);
	    }
	    else
            {
	     dest = destptr + x1;
             u  = left_u >> 8;
             v  = left_v >> 8;
            }

            if(width+x1 > 319)
	    {
		if(x1 >= 0)
		 width = 319-x1;
		else
		 width = 319;
	    }



            // Watcom C/C++ 10.0 can't get this inner loop any tighter 
            // than about 10-12 clock ticks.
            if(width > 0)
	    do
            {
                *dest++ = LUT[(texturemap[(v & 0xff00) + ((u & 0xff00) >> 8)]<<6) + intensity];
                u += dudx;                                                      
                v += dvdx;
            }
            while(--width);
	    
        }

        destptr += 320;

        // Interpolate along the left edge of the triangle
        if(--left_section_height <= 0)  // At the bottom of this section?
        {
            if(--left_section <= 0)     // All sections done?
                return;
                v1a = left_array[ left_section ];
                v2a = left_array[ left_section-1 ];

        heighta = v2a->y - v1a->y;
        if(heighta > 0)
        {
         // Calculate the deltas along this section
        
         delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
         left_x = v1a->x << 16;
         delta_left_u = ((v2a->tx - v1a->tx) << 10) / heighta;
         left_u = v1a->tx << 10;
         delta_left_v = ((v2a->ty - v1a->ty) << 10) / heighta;
         left_v = v1a->ty << 10;

         left_section_height = heighta;
        }
//        else
//         return;

            if(heighta <= 0)      // Nope, do the last section
                return;
        }
        else
        {
            left_x += delta_left_x;
            left_u += delta_left_u;
            left_v += delta_left_v;
        }

        // Interpolate along the right edge of the triangle
        if(--right_section_height <= 0) // At the bottom of this section?
        {
            if(--right_section <= 0)    // All sections done?
                return;
     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

            if(heighta <= 0)     // Nope, do the last section
                return;
        }
        else
            right_x += delta_right_x;
    }
    //not transparent
    else
    for(;;)         
    {
        x1 = left_x >> 16;
        width = (right_x >> 16) - x1;

        if(width > 0 && destptr >= buffer && destptr < (buffer+63680) && x1 < 319)
//        if(width > 0)
        {
            // This is the inner loop setup and the actual inner loop.
            // If you keep everything else in C that's up to you but at 
            // least remove this inner loop in C and insert some of 
            // the Assembly versions.

            
	    if(x1 < 0)
	    {
	        dest = destptr;
                width += x1;
                u = fixedmul(dudx,-left_x)+(left_u >> 8);
                v = fixedmul(dvdx,-left_x)+(left_v >> 8);
	    }
	    else
            {
	     dest = destptr + x1;
             u  = left_u >> 8;
             v  = left_v >> 8;
            }

            if(width+x1 > 319)
	    {
		if(x1 >= 0)
		 width = 319-x1;
		else
		 width = 319;
	    }



            // Watcom C/C++ 10.0 can't get this inner loop any tighter 
            // than about 10-12 clock ticks.
            if(width > 0)
	    do
            {
                *dest++ = TRANSPAR[(LUT[(texturemap[(v & 0xff00) + ((u & 0xff00) >> 8)]<<6) + intensity]<<8) + *dest];
                u += dudx;                                                      
                v += dvdx;
            }
            while(--width);
	    
        }

        destptr += 320;

        // Interpolate along the left edge of the triangle
        if(--left_section_height <= 0)  // At the bottom of this section?
        {
            if(--left_section <= 0)     // All sections done?
                return;
                v1a = left_array[ left_section ];
                v2a = left_array[ left_section-1 ];

        heighta = v2a->y - v1a->y;
        if(heighta > 0)
        {
         // Calculate the deltas along this section
        
         delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
         left_x = v1a->x << 16;
         delta_left_u = ((v2a->tx - v1a->tx) << 10) / heighta;
         left_u = v1a->tx << 10;
         delta_left_v = ((v2a->ty - v1a->ty) << 10) / heighta;
         left_v = v1a->ty << 10;

         left_section_height = heighta;
        }
//        else
//         return;

            if(heighta <= 0)      // Nope, do the last section
                return;
        }
        else
        {
            left_x += delta_left_x;
            left_u += delta_left_u;
            left_v += delta_left_v;
        }

        // Interpolate along the right edge of the triangle
        if(--right_section_height <= 0) // At the bottom of this section?
        {
            if(--right_section <= 0)    // All sections done?
                return;
     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

            if(heighta <= 0)     // Nope, do the last section
                return;
        }
        else
            right_x += delta_right_x;
    }

}

void tmapgour(TEXTURE * vtx,unsigned char transparent)
{
    TEXTURE * v1 = vtx;
    TEXTURE * v2 = vtx+1;
    TEXTURE * v3 = vtx+2;
    TEXTURE *vt,*v1a,*v2a;
    int height,heighta,temp,longest,width,x1,i,didx,dudx,dvdx;
    char *dest,*destptr;                                
    TEXTURE * left_array[3], * right_array[3];
    int left_section, right_section;
    int left_section_height, right_section_height;
    int left_i, delta_left_i;
    int left_u, delta_left_u, left_v, delta_left_v;
    int left_x, delta_left_x, right_x, delta_right_x;
    int u,v;

// Sort the triangle so that v1 points to the topmost, v2 to the
    // middle and v3 to the bottom vertex.
     
    if(v1->y > v2->y)
    {
      vt = v1;
      v1 = v2;
      v2 = vt;
    }
    if(v1->y > v3->y)
    {
      vt = v1;
      v1 = v3;
      v3 = vt;
    }
    if(v2->y > v3->y)
    {
      vt = v2;
      v2 = v3;
      v3 = vt;
    }

    if( (v3->y < 0 || v1->y > 199) || (v1->x < 0 && v2->x < 0 && v3->x < 0) ||
                              (v1->x > 319 && v2->x > 319 && v3->x > 319))
      return;

    // We start out by calculating the length of the longest scanline.

    height = v3->y - v1->y;
    if(height == 0)
        return;
    temp = ((v2->y - v1->y) << 16) / height;
    longest = temp * (v3->x - v1->x) + ((v1->x - v2->x) << 16);
    if(longest == 0)
        return;

    // Now that we have the length of the longest scanline we can use that 
    // to tell us which is left and which is the right side of the triangle.

    if(longest < 0)
    {
        // If longest is neg. we have the middle vertex on the right side.
        // Store the pointers for the right and left edge of the triangle.
        right_array[0] = v3;
        right_array[1] = v2;
        right_array[2] = v1;
        right_section  = 2;
        left_array[0]  = v3;
        left_array[1]  = v1;
        left_section   = 1;

        // Calculate initial left and right parameters
        v1a = v1;
        v2a = v3;
        

        heighta = v2a->y - v1a->y;
        if(heighta > 0)
        {
         // Calculate the deltas along this section
        
         delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
         left_x = v1a->x << 16;
         delta_left_u = ((v2a->tx - v1a->tx) << 10) / heighta;
         left_u = v1a->tx << 10;
         delta_left_v = ((v2a->ty - v1a->ty) << 10) / heighta;
         left_v = v1a->ty << 10;
	 delta_left_i = ((v2a->i - v1a->i) << 10) / heighta;
         left_i = v1a->i << 10;

         left_section_height = heighta;
        }
//        else
//         return;
     if(heighta <= 0) return;

     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

        if(heighta <= 0)
        {
            // The first right section had zero height. Use the next section. 
            right_section--;
     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

            if(heighta <= 0)
                return;
        }

        // Ugly compensation so that the dudx,dvdx divides won't overflow
        // if the longest scanline is very short.
        if(longest > -0x1000)
            longest = -0x1000;     
    }
    else
    {
        // If longest is pos. we have the middle vertex on the left side.
        // Store the pointers for the left and right edge of the triangle.
        left_array[0]  = v3;
        left_array[1]  = v2;
        left_array[2]  = v1;
        left_section   = 2;
        right_array[0] = v3;
        right_array[1] = v1;
        right_section  = 1;

        // Calculate initial right and left parameters
     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

        if(heighta <= 0)
            return;

        v1a = v1;
        v2a = v2;
        

        heighta = v2a->y - v1a->y;
        if(heighta > 0)
        {
         // Calculate the deltas along this section
        
         delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
         left_x = v1a->x << 16;
         delta_left_u = ((v2a->tx - v1a->tx) << 10) / heighta;
         left_u = v1a->tx << 10;
         delta_left_v = ((v2a->ty - v1a->ty) << 10) / heighta;
         left_v = v1a->ty << 10;
         delta_left_i = ((v2a->i - v1a->i) << 10) / heighta;
         left_i = v1a->i << 10;


         left_section_height = heighta;
        }
//        else
//         return;

        if(heighta <= 0)
        {
            // The first left section had zero height. Use the next section.
            left_section--;
                v1a = left_array[ left_section ];
                v2a = left_array[ left_section-1 ];
                
        
                heighta = v2a->y - v1a->y;
                if(heighta > 0)
                {
                 // Calculate the deltas along this section
                
                 delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
                 left_x = v1a->x << 16;
                 delta_left_u = ((v2a->tx - v1a->tx) << 10) / heighta;
                 left_u = v1a->tx << 10;
                 delta_left_v = ((v2a->ty - v1a->ty) << 10) / heighta;
                 left_v = v1a->ty << 10;
		 delta_left_i = ((v2a->i - v1a->i) << 10) / heighta;
                 left_i = v1a->i << 10;
        
                 left_section_height = heighta;
                }
//                else
//                 return;

            if(heighta <= 0)
                return;
        }

        // Ugly compensation so that the dudx,dvdx divides won't overflow
        // if the longest scanline is very short.
        if(longest < 0x1000)
            longest = 0x1000;     
    }

    // Now we calculate the constant deltas for u and v (dudx, dvdx)

    dudx = shl10idiv(temp*(v3->tx - v1->tx)+((v1->tx - v2->tx)<<16),longest);
    dvdx = shl10idiv(temp*(v3->ty - v1->ty)+((v1->ty - v2->ty)<<16),longest);
    didx = shl10idiv(temp*(v3->i - v1->i)+((v1->i - v2->i)<<16),longest); 
//    printf("2 divides\n");

    destptr = (char *)((v1->y<<6)+(v1->y<<8) + buffer);

    // If you are using a table lookup inner loop you should setup the
    // lookup table here.

    // Here starts the outer loop (for each scanline)
    dudx >>= 8;
    dvdx >>= 8;
    didx >>= 8;

    if(!transparent)
    for(;;)         
    {
        x1 = left_x >> 16;
        width = (right_x >> 16) - x1;

        if(width > 0 && destptr >= buffer && destptr < (buffer+63680) && x1 < 319)
//        if(width > 0)
        {
            // This is the inner loop setup and the actual inner loop.
            // If you keep everything else in C that's up to you but at 
            // least remove this inner loop in C and insert some of 
            // the Assembly versions.

            
	    if(x1 < 0)
	    {
	        dest = destptr;
                width += x1;
                u = fixedmul(dudx,-left_x)+(left_u >> 8);
                v = fixedmul(dvdx,-left_x)+(left_v >> 8);
                i = fixedmul(didx,-left_x)+(left_i >> 8);
	    }
	    else
            {
	     dest = destptr + x1;
             u  = left_u >> 8;
             v  = left_v >> 8;
             i  = left_i >> 8;
            }

            if(width+x1 > 319)
	    {
		if(x1 >= 0)
		 width = 319-x1;
		else
		 width = 319;
	    }



            // Watcom C/C++ 10.0 can't get this inner loop any tighter 
            // than about 10-12 clock ticks.
            if(width > 0)
	    do
            {
                *dest++ = LUT[(texturemap[(v & 0xff00) + ((u & 0xff00) >> 8)]<<6) + (i>>8)];
                u += dudx;                                                      
                v += dvdx;
		i += didx;
            }
            while(--width);
	    
        }

        destptr += 320;

        // Interpolate along the left edge of the triangle
        if(--left_section_height <= 0)  // At the bottom of this section?
        {
            if(--left_section <= 0)     // All sections done?
                return;
                v1a = left_array[ left_section ];
                v2a = left_array[ left_section-1 ];

        heighta = v2a->y - v1a->y;
        if(heighta > 0)
        {
         // Calculate the deltas along this section
        
         delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
         left_x = v1a->x << 16;
         delta_left_u = ((v2a->tx - v1a->tx) << 10) / heighta;
         left_u = v1a->tx << 10;
         delta_left_v = ((v2a->ty - v1a->ty) << 10) / heighta;
         left_v = v1a->ty << 10;
         delta_left_i = ((v2a->i - v1a->i) << 10) / heighta;
         left_i = v1a->i << 10;


         left_section_height = heighta;
        }
//        else
//         return;

            if(heighta <= 0)      // Nope, do the last section
                return;
        }
        else
        {
            left_x += delta_left_x;
            left_u += delta_left_u;
            left_v += delta_left_v;
            left_i += delta_left_i;

        }

        // Interpolate along the right edge of the triangle
        if(--right_section_height <= 0) // At the bottom of this section?
        {
            if(--right_section <= 0)    // All sections done?
                return;
     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

            if(heighta <= 0)     // Nope, do the last section
                return;
        }
        else
            right_x += delta_right_x;
    }
    //not transparent
    else
    for(;;)         
    {
        x1 = left_x >> 16;
        width = (right_x >> 16) - x1;

        if(width > 0 && destptr >= buffer && destptr < (buffer+63680) && x1 < 319)
//        if(width > 0)
        {
            // This is the inner loop setup and the actual inner loop.
            // If you keep everything else in C that's up to you but at 
            // least remove this inner loop in C and insert some of 
            // the Assembly versions.

            
	    if(x1 < 0)
	    {
	        dest = destptr;
                width += x1;
                u = fixedmul(dudx,-left_x)+(left_u >> 8);
                v = fixedmul(dvdx,-left_x)+(left_v >> 8);
                i = fixedmul(didx,-left_x)+(left_i >> 8);
	    }
	    else
            {
	     dest = destptr + x1;
             u  = left_u >> 8;
             v  = left_v >> 8;
             i  = left_i >> 8;
            }

            if(width+x1 > 319)
	    {
		if(x1 >= 0)
		 width = 319-x1;
		else
		 width = 319;
	    }



            // Watcom C/C++ 10.0 can't get this inner loop any tighter 
            // than about 10-12 clock ticks.
            if(width > 0)
	    do
            {
                *dest++ = TRANSPAR[(LUT[(texturemap[(v & 0xff00) + ((u & 0xff00) >> 8)]<<6) + (i>>8)]<<8) + *dest];
                u += dudx;                                                      
                v += dvdx;
		i += didx;
            }
            while(--width);
	    
        }

        destptr += 320;

        // Interpolate along the left edge of the triangle
        if(--left_section_height <= 0)  // At the bottom of this section?
        {
            if(--left_section <= 0)     // All sections done?
                return;
                v1a = left_array[ left_section ];
                v2a = left_array[ left_section-1 ];

        heighta = v2a->y - v1a->y;
        if(heighta > 0)
        {
         // Calculate the deltas along this section
        
         delta_left_x = ((v2a->x - v1a->x) << 16) / heighta;
         left_x = v1a->x << 16;
         delta_left_u = ((v2a->tx - v1a->tx) << 10) / heighta;
         left_u = v1a->tx << 10;
         delta_left_v = ((v2a->ty - v1a->ty) << 10) / heighta;
         left_v = v1a->ty << 10;
         delta_left_i = ((v2a->i - v1a->i) << 10) / heighta;
         left_i = v1a->i << 10;


         left_section_height = heighta;
        }
//        else
//         return;

            if(heighta <= 0)      // Nope, do the last section
                return;
        }
        else
        {
            left_x += delta_left_x;
            left_u += delta_left_u;
            left_v += delta_left_v;
            left_i += delta_left_i;

        }

        // Interpolate along the right edge of the triangle
        if(--right_section_height <= 0) // At the bottom of this section?
        {
            if(--right_section <= 0)    // All sections done?
                return;
     v1a = right_array[ right_section ];
     v2a = right_array[ right_section-1 ];

    heighta = v2a->y - v1a->y;
    if(heighta > 0)
    {
     // Calculate the deltas along this section

     delta_right_x = ((v2a->x - v1a->x) << 16) / heighta;
     right_x = v1a->x << 16;

     right_section_height = heighta;
    }
//    else
//     return;

            if(heighta <= 0)     // Nope, do the last section
                return;
        }
        else
            right_x += delta_right_x;
    }

}

void Set_Palette_Register(int index, RGB_color_ptr color)
{
	outp(PALETTE_MASK,0xff);
	outp(PALETTE_REGISTER_WR,index);
	outp(PALETTE_DATA,color->red);
	outp(PALETTE_DATA,color->green);
	outp(PALETTE_DATA,color->blue);
}

void set_rgb(int index, unsigned char r, unsigned char g, unsigned char b)
{
	outp(PALETTE_MASK,0xff);
	outp(PALETTE_REGISTER_WR,index);
	outp(PALETTE_DATA,r);
	outp(PALETTE_DATA,g);
	outp(PALETTE_DATA,b);
} 

void Get_Palette_Register(int index, RGB_color_ptr color)
{

// this function gets the data out of a color lookup regsiter and places it
// into color

// set the palette mask register

outp(PALETTE_MASK,0xff);

// tell vga card which register we will be reading

outp(PALETTE_REGISTER_RD, index);

// now extract the data

color->red   = inp(PALETTE_DATA);
color->green = inp(PALETTE_DATA);
color->blue  = inp(PALETTE_DATA);

} // end Get_Palette_Color

void Create_Cool_Palette(void)
{
	RGB_color color;
	int index;
	for (index=0; index<64; index++)
	{
                color.red=index;
                color.green=index;
                color.blue=index;
		Set_Palette_Register(index, (RGB_color_ptr)&color);
                color.red=index;
                color.green=index;
                color.blue=-index;
		Set_Palette_Register(index+64, (RGB_color_ptr)&color);
		color.red=63-index;
		color.green=63-index;
		color.blue=0;
		Set_Palette_Register(index+128, (RGB_color_ptr)&color);
		color.red=63-index;
		color.green=0;
		color.blue=0;
		Set_Palette_Register(index+192, (RGB_color_ptr)&color);
	}
	color.red = color.blue = color.green = 0;
	Set_Palette_Register(0,(RGB_color_ptr)&color);
}


void load_palette(unsigned char palette[768])
{
  RGB_color pal;
  int i;
  for(i=0;i<768;i+=3)
  {
   pal.red = palette[i];  pal.green = palette[i+1]; pal.blue = palette[i+2];
   Set_Palette_Register(i/3,(RGB_color_ptr)&pal);
  }
}

void save_palette(unsigned char palette[768])
{
  int i;
  RGB_color pal;
  for(i=0;i<768;i+=3)
  {
   Get_Palette_Register(i/3,(RGB_color_ptr)&pal);
   palette[i] = pal.red; palette[i+1] = pal.green; palette[i+2]=pal.blue;
  }
}

void fade_out(unsigned char pal[768])
{
	int i,j;
	int count=0;
        static unsigned char pal2[768];
	
	for(i=0;i<768;i++)
	 pal2[i] = pal[i];

	for (j=0; j<256 && count!=768; j++)
	{ count=0;
	for (i=0; i<768; i++)
	{
		if (pal2[i]>0) pal2[i]-=1;
		if (pal2[i]==0) count++;
		if (i%256==0)
		{
			Wait_For_Vsync();
			load_palette(pal2);
		}
	}
	}
}

void clear_palette(unsigned char pal[768])
{
	int i;
	for (i=0; i<768; i++) pal[i]=0;
}

void black_video_palette(void)
{
	int i;
        static unsigned char pal[768];
	for (i=0; i<768; i++) pal[i]=0;
	load_palette(pal);
}

void fade_in(unsigned char pal[768])
{
	int i,j;
	int count=0;
        static unsigned char pal2[768];

	clear_palette(pal2);
	for (j=0; j<256 && count!=768; j++)
	{ count=0;
	for (i=0; i<768; i++)
	{
		if (pal2[i]<pal[i]) pal2[i]+=1;
		if (pal2[i]==pal[i]) count++;
		if (i%256==0)
		{
			Wait_For_Vsync();
			load_palette(pal2);
		}
	}
	}
}

void line(int x1, int y1, int x2, int y2, unsigned char color)
{
 int d,x,y,ax,ay,sx,sy,dx,dy;

 dx = x2-x1;  ax=ABS(dx)<<1;
 dy = y2-y1;  ay=ABS(dy)<<1;

 sx=SGN(dx);
 sy=SGN(dy);

 x=x1;
 y=y1;
 if(ax > ay)
 {
  d = ay-(ax>>1);
  for(;;)
  {
   buffer[(y<<6)+(y<<8)+x] = color;
   if(x==x2) return;
   if(d >= 0)
   {
    y+=sy;
    d-=ax;
   }
   x+=sx;
   d+=ay;
  }
 }
 else
 {
  d=ax-(ay>>1);
  for(;;)
  {
   buffer[(y<<6)+(y<<8)+x] = color;
   if(y==y2) return;
   if(d>=0)
   {
    x+=sx;
    d-=ay;
   }
   y+=sy;
   d+=ax;
  }
 }
}


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
