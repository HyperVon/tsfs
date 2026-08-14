typedef struct {
   float x,y;           // screen coords
   float tx,ty;         // texture coords
   float i;             // intensity 
}TEXTURE;


void texture(TEXTURE * vtx,unsigned char transparent)
{
    TEXTURE * v1 = vtx;
    TEXTURE * v2 = vtx+1;
    TEXTURE * v3 = vtx+2;
    TEXTURE *vt,*v1a,*v2a;
    float height,heighta,temp,longest,dudx,dvdx,width,x1,u,v,du,dv;
    char *dest,*destptr;
    TEXTURE * left_array[3], * right_array[3];
    int left_section, right_section;
    float left_section_height, right_section_height;
    float left_u, delta_left_u, left_v, delta_left_v;
    float left_x, delta_left_x, right_x, delta_right_x;


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
    temp = (v2->y - v1->y) / height;
    longest = temp * (v3->x - v1->x) + ((v1->x - v2->x)*65536);
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
        
         delta_left_x = ((v2a->x - v1a->x) ) / heighta;
         left_x = v1a->x ;
         delta_left_u = ((v2a->tx - v1a->tx) ) / heighta;
         left_u = v1a->tx ;
         delta_left_v = ((v2a->ty - v1a->ty) ) / heighta;
         left_v = v1a->ty;

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

     delta_right_x = ((v2a->x - v1a->x) ) / heighta;
     right_x = v1a->x ;

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

     delta_right_x = ((v2a->x - v1a->x) ) / heighta;
     right_x = v1a->x ;

     right_section_height = heighta;
    }
//    else
//     return;

            if(heighta <= 0)
                return;
        }

        // Ugly compensation so that the dudx,dvdx divides won't overflow
        // if the longest scanline is very short.
//        if(longest > -0x1000)
//            longest = -0x1000;     
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

     delta_right_x = ((v2a->x - v1a->x) ) / heighta;
     right_x = v1a->x ;

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
        
         delta_left_x = ((v2a->x - v1a->x) ) / heighta;
         left_x = v1a->x ;
         delta_left_u = ((v2a->tx - v1a->tx) ) / heighta;
         left_u = v1a->tx ;
         delta_left_v = ((v2a->ty - v1a->ty) ) / heighta;
         left_v = v1a->ty ;

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
                
                 delta_left_x = ((v2a->x - v1a->x) ) / heighta;
                 left_x = v1a->x ;
                 delta_left_u = ((v2a->tx - v1a->tx) ) / heighta;
                 left_u = v1a->tx ;
                 delta_left_v = ((v2a->ty - v1a->ty) ) / heighta;
                 left_v = v1a->ty ;
        
                 left_section_height = heighta;
                }
//                else
//                 return;

            if(heighta <= 0)
                return;
        }

        // Ugly compensation so that the dudx,dvdx divides won't overflow
        // if the longest scanline is very short.
//        if(longest < 0x1000)
//            longest = 0x1000;     
    }

    // Now we calculate the constant deltas for u and v (dudx, dvdx)

    dudx = (temp*(v3->tx - v1->tx)+((v1->tx - v2->tx))/longest);
    dvdx = (temp*(v3->ty - v1->ty)+((v1->ty - v2->ty))/longest);
//    printf("2 divides\n");

    destptr = (char *)((int)v1->y * 320 + buffer);

    // If you are using a table lookup inner loop you should setup the
    // lookup table here.

    // Here starts the outer loop (for each scanline)
   dudx /= 256;
   dvdx /= 256;

    if(!transparent)
    for(;;)         
    {
        x1 = left_x ;
        width = (right_x ) - x1;

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
                u = dudx*-left_x + left_u ;
                v = dvdx*-left_x + left_v ;
	    }
	    else
            {
             dest = destptr + (int)x1;
             u  = left_u ;
             v  = left_v ;
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
                *dest++ = texturemap[ ((int)v & 0xff00) + (((int)u & 0xff00) >> 8) ];
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
        
         delta_left_x = ((v2a->x - v1a->x) ) / heighta;
         left_x = v1a->x ;
         delta_left_u = ((v2a->tx - v1a->tx) ) / heighta;
         left_u = v1a->tx ;
         delta_left_v = ((v2a->ty - v1a->ty) ) / heighta;
         left_v = v1a->ty ;

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

     delta_right_x = ((v2a->x - v1a->x) ) / heighta;
     right_x = v1a->x ;

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
        x1 = left_x ;
        width = (right_x ) - x1;

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
                u = dudx*-left_x + left_u ;
                v = dvdx*-left_x + left_v ;
	    }
	    else
            {
             dest = destptr + (int)x1;
             u  = left_u;
             v  = left_v;
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
                *dest++ = TRANSPAR[(texturemap[ ((int)v & 0xff00) + (((int)u & 0xff00) >> 8) ]<<8)+ *dest];
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
        
         delta_left_x = ((v2a->x - v1a->x) ) / heighta;
         left_x = v1a->x ;
         delta_left_u = ((v2a->tx - v1a->tx) ) / heighta;
         left_u = v1a->tx ;
         delta_left_v = ((v2a->ty - v1a->ty) ) / heighta;
         left_v = v1a->ty ;

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

     delta_right_x = ((v2a->x - v1a->x) ) / heighta;
     right_x = v1a->x ;

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
