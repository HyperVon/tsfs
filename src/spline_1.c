/****************************************************************************
 spline_1.c, ver 1.0 (06/03/1997) from "Exploring 3DS secrets" series :)
 ----------------------------------------------------------------------------
 Written by Timur Davidenko (aka Adept/Esteem), 1997.
 Send any comments/suggestions to: adept@aquanet.co.il
 I`ll be glad to see any responses.
 ----------------------------------------------------------------------------
 This file contain C source code for interpolating spline used by
 3D studio, including all spline controls parameters.
 I`ve been looking for this formuls for more then half year,and i belive
 still many people will be very glad to know them,thats why i releasing
 this source. It not optimized for speed or anything else i tried to keep
 it as clear as posible.

 Big thanks to Jare/Iguana,Xanthome/Darkzone and RexDeathscar/Waterlogic
 for great support on topic.

 I compile this source with Borland C/C++ 3.1,it shouldn`t be a a problem
 to compile with other AnsiC compiler, it only require standart graphics
 library to be linked and egavga.bgi file to be present in same directory.
 ----------------------------------------------------------------------------
 Tech notes:
		It is Kochanek-Bartels spline sub-type of hermit curve developped
		espesially for computer key animations,first presented at SIGGRAPH'84.
		Basic derivatives and ease calculation function derrived from
		3D Studio and Lightwave technical documents.

		Basic hermit curve evaluted as follow:
				.
					o  P1(x,y,z), R1(x,y,z)="departing derivative"
					.	\
					 .	\
						.		\
						 .		\
							 . . .o P4(x,y,z), R4(x,y,z)="arriving derivative"

(1)	X(t) = T * M * G
		where:
			B - Hermit Basis Matrix.
			G - Hermit Geometry Matrix.
			T - Time matrix.

			Ú             ¿         Ú      ¿
			³  2 -2  1  1 ³         ³  P1x ³
	M = ³ -3  3 -2 -1 ³     G = ³  P4x ³   T = [ t^3 t^2 t 1]
			³  0  0  1  0 ³         ³  R1x ³
			³  1  0  0  0 ³         ³  R4x ³
			À             Ù					À      Ù

		P1 and P4 are end points of spline segment while R1 and R4 thier
		respective tangent vectors	(derivatives)

		Another way to write the hermite spline equations is as	a
		Blending Function. This is really just the multiplied-out results
		of the above matrix equation, and looks like this:

(2)	X(t) = 	( 2t3 - 3t2 + 1)*P1x +
						(-2t3 + 3t2    )*P4x +
						(  t3 - 2t2 + t)*R1x +
						(  t3 - t2     )*R4x

		When continuity,bias and tension slope controls appears
		derivatives (for X) calculated like this:

		pkey 	- previous key.
		key		- current key
		nkey 	- next key.
		ds 		- source derivative.
		dd 		- destination derivative.

		dsA = (1.0 - key.tens) * (1.0 - key.cont) * (1.0 + key.bias);
		dsB = (1.0 - key.tens) * (1.0 + key.cont) * (1.0 - key.bias);
		dsAdjust = (key.frame - prevkey.frame)/(nextkey.frame - prevkey.frame)
		ds = dsAdjust * ( dsA*(key.x - prevkey.x) + dsB*(nextkey.x - key.x) )

		ddA = (1.0 - key.tens) * (1.0 + key.cont) * (1.0 + key.bias);
		ddB = (1.0 - key.tens) * (1.0 - key.cont) * (1.0 - key.bias);
		ddAdjust = (nextkey.frame - key.frame)/(nextkey.frame - prevkey.frame)
		dd = ddAdjust * ( ddA*(key.x - prevkey.x) + ddB*(nextkey.x - key.x) )

		Insert key`s position and ds,dd values to equation (2) and congrads!
		u have interpolated spline :)

		I`ve a real pain figuring out myself formuls for calculating
		derivatives at first/last key of spline (3DS calculate them differently)
		I tested them almost in all	imagenable cases and they provided 100%
		correct	result but still if	you will find they give wrong results
		for some specific case plz let me know of that.
 ----------------------------------------------------------------------------
 PS.
		I hope to add for next version also quaternion interpolation source.
		Look also for 3dsrdr.c 3ds files reader by Javier Arevalo (Jare/Iguana)
		and 3DSCO20.ZIP by Mats Byggmastar (Mri/Doomsday) for great examples
		of reading .3DS files.
 ----------------------------------------------------------------------------
																DISCALIMER
 I can not be held responsible for any damages this source may produce,
 Use it on your own risk.
 You are free to distribute,copy,use and modify this source code with one
 restriction do not distribute modified copy of this source without firstly
 asking me for permission.
****************************************************************************/

#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>

typedef	struct	{
	float	x,y,z;
}		Vector;

typedef	struct	{
	float	frame;				// key frame.
	Vector pos;					// Position vector.
	Vector ds,dd;				// Key derivatives.
	float	tens;					// Key tension value.
	float	cont;					// Key continuity value.
	float bias;					// Key bias value.
	float easeto;				// Key ease to value.
	float easefrom;			// Key ease from value.
}		Key;

Key keys[100];

int	MAX;								// Max used keys.


float Ease( float t, float a, float b)	{
	float k;
	float s = a+b;

	if (s == 0.0) return t;
	if (s > 1.0) {
		a = a/s;
		b = b/s;
	}
	k = 1.0/(2.0-a-b);
	if (t < a) return ((k/a)*t*t);
	else	{
		if (t < 1.0-b)	{
			return (k*(2*t - a));
		}	else {
			t = 1.0-t;
			return (1.0-(k/b)*t*t);
		}
	}
}

static void CompElementDeriv( float pp, float p, float pn,
																				float *ds, float *dd,float ksm,
																				float ksp, float kdm, float kdp )
{
	float delm, delp;

	delm = p - pp;
	delp = pn - p;
	*ds  = ksm*delm + ksp*delp;
	*dd  = kdm*delm + kdp*delp;
}

/*---------------------------------------------------------------------------
 This computes the derivative at key, as a weighted average of the linear
 slopes into and out of key, the weights being determined by the tension and
 continuity parameters.
 Actually two derivatives are computed at key:
				"ds" is the "source derivative", or "arriving derivative"
				"dd" is the "destination derivative" or "departing derivative"
---------------------------------------------------------------------------*/
void	CompDeriv( Key *keyp,Key *key,Key *keyn )	{
	float tm,cm,cp,bm,bp,tmcm,tmcp,ksm,ksp,kdm,kdp,delm,delp,c;
	float dt,fp,fn;
	dt = .5 * ( keyn->frame - keyp->frame );
	fp = ( key->frame - keyp->frame ) / dt;
	fn = ( keyn->frame - key->frame ) / dt;
	c  = fabs( key->cont );
	fp = fp + c - c * fp;
	fn = fn + c - c * fn;
	cm = 1.0 - key->cont;
	tm = 0.5 * ( 1.0 - key->tens );
	cp = 2.0 - cm;
	bm = 1.0 - key->bias;
	bp = 2.0 - bm;
	tmcm = tm*cm;   tmcp = tm*cp;
	ksm = tmcm*bp*fp;       ksp = tmcp*bm*fp;
	kdm = tmcp*bp*fn;       kdp = tmcm*bm*fn;

	CompElementDeriv( keyp->pos.x, key->pos.x, keyn->pos.x,
			&key->ds.x, &key->dd.x, ksm, ksp, kdm, kdp );

	CompElementDeriv( keyp->pos.y, key->pos.y, keyn->pos.y,
			&key->ds.y, &key->dd.y, ksm, ksp, kdm, kdp );

	CompElementDeriv( keyp->pos.z, key->pos.z, keyn->pos.z,
			&key->ds.z, &key->dd.z, ksm, ksp, kdm, kdp );
}

void	CompDerivFirst( Key *key,Key *keyn,Key *keynn )	{
	float	f20,f10,v20,v10;
	f20 = keynn->frame - key->frame;
	f10 = keyn->frame - key->frame;

	v20 = keynn->pos.x - key->pos.x;
	v10 = keyn->pos.x - key->pos.x;
	key->dd.x = (1-key->tens)*(v20*(0.25 - f10/(2*f20)) + (v10 - v20/2)*3/2 + v20/2);

	v20 = keynn->pos.y - key->pos.y;
	v10 = keyn->pos.y - key->pos.y;
	key->dd.y = (1-key->tens)*(v20*(0.25 - f10/(2*f20)) + (v10 - v20/2)*3/2 + v20/2);

	v20 = keynn->pos.z - key->pos.z;
	v10 = keyn->pos.z - key->pos.z;
	key->dd.z = (1-key->tens)*(v20*(0.25 - f10/(2*f20)) + (v10 - v20/2)*3/2 + v20/2);
}

void	CompDerivLast( Key *keypp,Key *keyp,Key *key )	{
	float	f20,f10,v20,v10;
	f20 = key->frame - keypp->frame;
	f10 = key->frame - keyp->frame;

	v20 = key->pos.x - keypp->pos.x;
	v10 = key->pos.x - keyp->pos.x;
	key->ds.x = (1-key->tens)*(v20*(0.25 - f10/(2*f20)) + (v10 - v20/2)*3/2 + v20/2);

	v20 = key->pos.y - keypp->pos.y;
	v10 = key->pos.y - keyp->pos.y;
	key->ds.y = (1-key->tens)*(v20*(0.25 - f10/(2*f20)) + (v10 - v20/2)*3/2 + v20/2);

	v20 = key->pos.z - keypp->pos.z;
	v10 = key->pos.z - keyp->pos.z;
	key->ds.z = (1-key->tens)*(v20*(0.25 - f10/(2*f20)) + (v10 - v20/2)*3/2 + v20/2);
}

// Draw Kochanek-Bartels spline.
void	Draw3DSSpline()	{
	int	i,n;
	float	j,t,t2,t3,frames;
	float	x,y,z;
	float h[4];

	if (MAX < 2) return;
	if (MAX > 2)	{
		for (n = 1; n < MAX-1; n++ )	{
			CompDeriv( &keys[n-1],&keys[n],&keys[n+1] );
		}
	}
	CompDerivFirst( &keys[0],&keys[1],&keys[2] );
	CompDerivLast( &keys[MAX-3],&keys[MAX-2],&keys[MAX-1] );

	for (n = 0; n < MAX-1; n++ )	{
		line( keys[n].pos.x,480-keys[n].pos.y,
					keys[n+1].pos.x,480-keys[n+1].pos.y );	// test line.

		frames = keys[n+1].frame - keys[n].frame;
		for (j = 0; j < frames; j++)	{
			t = j/frames;
			t = Ease( t,keys[n].easefrom,keys[n+1].easeto );
			t2 = t*t;												// t2 = t^2;
			t3 = t2*t;											// t3 = t^3;

			h[0] = 2*t3 - 3*t2 + 1;
			h[1] = -2*t3 + 3*t2;
			h[2] = t3 - 2*t2 + t;
			h[3] = t3 - t2;

			x = (h[0]*keys[n].pos.x) + (h[1]*keys[n+1].pos.x) +
					(h[2]*keys[n].dd.x) + (h[3]*keys[n+1].ds.x);
			y = (h[0]*keys[n].pos.y) + (h[1]*keys[n+1].pos.y) +
					(h[2]*keys[n].dd.y)+(h[3]*keys[n+1].ds.y);
			z = (h[0]*keys[n].pos.z) + (h[1]*keys[n+1].pos.z) +
					(h[2]*keys[n].dd.z)+(h[3]*keys[n+1].ds.z);

			putpixel( x,480-y,13 );
		}
	}
}

void	SetKey( Key *key,	float frame,
							float x,float y,float z,
							float t,float c,float b,float eto,float efrom )	{
	key->frame = frame;
	key->pos.x = x;
	key->pos.y = y;
	key->pos.z = z;
	key->tens = t;
	key->cont = c;
	key->bias = b;
	key->easeto = eto;
	key->easefrom = efrom;
	key->ds.x = 0; key->ds.y = 0; key->ds.z = 0;
	key->dd.x = 0; key->dd.y = 0; key->dd.z = 0;
}

void	InitKeys()	{
	// Table of tens/bias/cont equialents.
	// Value in 3DStudio key info: |  0 | 25 | 50 |
	// Value for SetKey:           | -1 |  0 |  1 |

	// Table of easeto/easefrom equialents.
	// Value in 3DStudio key info: |  0 | 50 |
	// Value for SetKey:           |  0 |  1 |


	//							 Frm.  X   Y   Z   t  c  b  et ef
	SetKey( &keys[0],0,		100,100, 0,	 0, 0, 0, 0, 0 );
	SetKey( &keys[1],50,	250,300, 0,	 0, 0, 0, 0, 0 );
	SetKey( &keys[2],100,	400,200, 0,	 0, 0, 0, 0, 0 );
	SetKey( &keys[3],150,	550,250, 0,	 0, 0, 0, 0, 0 );
	SetKey( &keys[4],350,	150,250, 0,	 0, 0, 0, 0, 0 );

	MAX = 5;
}

int main(void)
{
	 /* request auto detection */
	 int gdriver = DETECT, gmode, errorcode;
	 initgraph(&gdriver, &gmode, "\\TC\\BGI" );
	 errorcode = graphresult();
	 if (errorcode != grOk)  /* an error occurred */
	 {
			printf("Graphics error: %s\n", grapherrormsg(errorcode));
			printf("Press any key to halt:");
			getch();
			exit(1);             /* return with error code */
	 }

	InitKeys();
	Draw3DSSpline();

	getch();
	closegraph();
	return 0;
}