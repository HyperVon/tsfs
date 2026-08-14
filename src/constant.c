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


void constant(int x1, int y1, int x2, int y2, int x3, int y3,
        unsigned char color,unsigned char transparent)
{
    Fixed32 line1[200], line2[200], line3[200];
    Fixed32 temp,xspecial1,xspecial3;
    Fixed32 d,x,y,ax,ay,dx,dy,sx,sy,dxf,xt,y2y1,y3y2,y3y1;
    unsigned char *dest;

    


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
                 hline(line3[y],line2[y],color,dest);
		dest += 320;
	    }
    
	//this case x1 is right of x2
	else
            for(y=fixed_to_int(y1); y<=fixed_to_int(y3);y++)
	    {
               if(y >=0 && y < 200)
                 hline(line2[y],line3[y],color,dest);
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
                 hline(line1[y],line3[y],color,dest);
		dest += 320;
	    }

	//this case x3 left of x2
	else
            for(y=fixed_to_int(y1);y<=fixed_to_int(y3);y++)
	    {
                if(y >=0 && y < 200)
                 hline(line3[y],line1[y],color,dest);
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
                 hline(line1[y],line3[y],color,dest);
		dest += 320;
	    }

	    //do the second half of triangle
	    //only recalculate the left interp value.. right should stay the same

            for(y=fixed_to_int(y2);y<=fixed_to_int(y3);y++)
	    {
                if(y >=0 && y < 200)
                 hline(line2[y],line3[y],color,dest);
		dest += 320;
	    }
	    
	}
	else
	{

	    //do the first half of triangle

            for(y=fixed_to_int(y1);y<fixed_to_int(y2);y++)
	    {
                if(y >=0 && y < 200)
                 hline(line3[y],line1[y],color,dest);
		dest += 320;
	    }

	    //do the second half of triangle
	    //only recalculate the right interp value.. left should stay the same

            for(y=fixed_to_int(y2);y<=fixed_to_int(y3);y++)
	    {
                if(y >=0 && y < 200)
                 hline(line3[y],line2[y],color,dest);
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
                 *(dest + x) = TRANSPAR[(color<<8)+ *(dest + x)];
		dest += 320;
	    }
    
	//this case x1 is right of x2
	else
            for(y=fixed_to_int(y1); y<fixed_to_int(y3);y++)
	    {
                if(y >=0 && y < 200)
                for(x=line2[y];x<line3[y];x++)
                 *(dest + x) = TRANSPAR[(color<<8)+ *(dest + x)];

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
                 *(dest + x) = TRANSPAR[(color<<8)+ *(dest + x)];

		dest += 320;
	    }

	//this case x3 left of x2
	else
            for(y=fixed_to_int(y1);y<fixed_to_int(y3);y++)
	    {
                if(y >=0 && y < 200)
                for(x=line3[y];x<line1[y];x++)
                 *(dest + x) = TRANSPAR[(color<<8)+ *(dest + x)];

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
                 *(dest + x) = TRANSPAR[(color<<8)+ *(dest + x)];

		dest += 320;
	    }

	    //do the second half of triangle
	    //only recalculate the left interp value.. right should stay the same

            for(y=fixed_to_int(y2);y<fixed_to_int(y3);y++)
	    {
                if(y >=0 && y < 200)
                for(x=line2[y];x<line3[y];x++)
                  *(dest + x) = TRANSPAR[(color<<8)+ *(dest + x)];

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
                  *(dest + x) = TRANSPAR[(color<<8)+ *(dest + x)];

		dest += 320;
	    }

	    //do the second half of triangle
	    //only recalculate the right interp value.. left should stay the same

            for(y=fixed_to_int(y2);y<fixed_to_int(y3);y++)
	    {
                if(y >=0 && y < 200)
                for(x=line3[y];x<line2[y];x++)
                  *(dest + x) = TRANSPAR[(color<<8)+ *(dest + x)];

		dest += 320;
	    }
	    
	}
    }
  
}
