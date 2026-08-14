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

Key *keys;
int MAX;                                        // Max used keys.


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

