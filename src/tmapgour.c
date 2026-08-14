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

