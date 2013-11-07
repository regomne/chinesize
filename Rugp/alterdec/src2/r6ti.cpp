
#include	"common.h"



static void		Opaque1( CStream *s, CImageWriter *image, int width, int height, dword depth );
static void		Opaque2( CStream *s, CImageWriter *image, int width, int height, dword depth );
static void		Transparent( CStream *s, CImageWriter *image, int width, int height, dword depth );



void ReadR6Ti( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
	dword offset = object->offset + s->getreadbyte() / 2;

	s->readdword();
	s->readword();
	s->readword();
	s->readdword();
	int width = s->readword();
	int height = s->readword();
	dword flag = s->readdword();

	dword depth = s->readdword();
	s->readbyte();
	s->readword();

	ReadClassList( s, cache, object );
	dword size=s->readdword();	//	data size
	s->readdword();
	s->readdword();

    //Ö±½Ó¶ÁÈ¡block
	s->seek((size+1)/2*2+2);
	return;

	CImageWriter	image( width, height );

	switch ( flag & 0xff )
	{
	case 2:
		if ( depth & 0x4 )
			Opaque1( s, &image, width, height, depth );
		else
			Opaque2( s, &image, width, height, depth );
		break;
	case 3:
		Transparent( s, &image, width, height, depth );
		break;
	default:
		throw "ReadR6Ti";
	}
		
	image.write( offset );
}

void Opaque1( CStream *s, CImageWriter *image, int width, int height, dword depth )
{

	for ( int y=0; y<height; y++ )
	{
		int		count = -1;
		int 	r, g, b;
		int		dr, dg, db;

		r = g = b = 0;
		dr = dg = db = 0;

		for ( int x=0; x<width; )
		{
			if ( count == -1 )
				count = s->readunsigned();

			if ( s->readbit() == 1 )
			{
			}
			else
			{

				dg = s->readsigned();


				r += s->readsigned() * 2;

				b += s->readsigned() * 2;
			}

			x++;

			if ( count-- == 0  &&  x < width )
			{
				int n = s->readunsigned() + 1;

				x+=n;

			}
		}
	}

}


void Opaque1O( CStream *s, CImageWriter *image, int width, int height, dword depth )
{
	static int prbuf[2000];
	static int pgbuf[2000];
	static int pbbuf[2000];
	
	int *	pr = prbuf;
	int *	pg = pgbuf;
	int *	pb = pbbuf;

	if ( width > 2000 )
	{
		pr = new int[width];
		pg = new int[width];
		pb = new int[width];
	}
	
	for ( int y=0; y<height; y++ )
	{
		int		count = -1;
		int 	r, g, b;
		int		dr, dg, db;

		r = g = b = 0;
		dr = dg = db = 0;

		for ( int x=0; x<width; )
		{
			if ( count == -1 )
				count = s->readunsigned();

			if ( s->readbit() == 1 )
			{
				r=pr[x], g=pg[x], b=pb[x];
				dr = dg = db = 0;
			}
			else
			{
				int t = g + dg*2;
				if ( t > 255 )  t = 255;
				if ( t < 0 )  t = 0;
				t -= g;

				dg = s->readsigned();

				dg += t/2;
				if ( dg < -128 )  dg = -128;
				if ( dg > 127 )  dg = 127;

				db = dr = dg;
				r += dr * 2;
				g += dg * 2;
				b += db * 2;
				
				if ( r > 255 )  r = 255;
				if ( g > 255 )  g = 255;
				if ( b > 255 )  b = 255;
				if ( r < 0 )  r = 0;
				if ( g < 0 )  g = 0;
				if ( b < 0 )  b = 0;

				r += s->readsigned() * 2;
				if ( r > 255 )  r = 255;
				if ( r < 0 )  r = 0;

				b += s->readsigned() * 2;
				if ( b > 255 )  b = 255;
				if ( b < 0 )  b = 0;
			}

			pr[x]=r, pg[x]=g, pb[x]=b;
			image->set( x++, y, r, g, b );

			if ( count-- == 0  &&  x < width )
			{
				int n = s->readunsigned() + 1;

				for ( int i=0; i<n; i++ )
				{
					pr[x]=r, pg[x]=g, pb[x]=b;
					image->set( x++, y, r, g, b );
				}

				dr = dg = db = 0;
			}
		}
	}

	if ( pr != prbuf )  delete[] pr;
	if ( pg != pgbuf )  delete[] pg;
	if ( pb != pbbuf )  delete[] pb;
}

void Opaque2( CStream *s, CImageWriter *image, int width, int height, dword depth )
{
	int 	r, g, b;
	for ( int y=0; y<height; y++ )
	{
		int		count = -1;

		r = g = b = 0;

		for ( int x=0; x<width; )
		{
			if ( count < 0 )
				count = s->readunsigned();

			if ( s->readbit() == 1 )
			{
			}
			else
			{
				s->readsigned();
				s->readsigned();
				s->readsigned();
			}

			x++;

			if ( --count < 0  &&  x < width )
			{
				int n = s->readunsigned() + 1;

				x+=n;
			}
		}
	}

}


void Opaque2O( CStream *s, CImageWriter *image, int width, int height, dword depth )
{
	static int prbuf[2000];
	static int pgbuf[2000];
	static int pbbuf[2000];
	
	int *	pr = prbuf;
	int *	pg = pgbuf;
	int *	pb = pbbuf;

	if ( width > 2000 )
	{
		pr = new int[width];
		pg = new int[width];
		pb = new int[width];
	}

	int rdepth = depth >> 8 & 0xff;
	int gdepth = depth >> 16 & 0xff;
	int bdepth = depth >> 24 & 0xff;
	
	int 	r, g, b;
	for ( int y=0; y<height; y++ )
	{
		int		count = -1;

		r = g = b = 0;

		for ( int x=0; x<width; )
		{
			if ( count < 0 )
				count = s->readunsigned();

			if ( s->readbit() == 1 )
			{
				r = pr[x];
				g = pg[x];
				b = pb[x];
			}
			else
			{
				int d = s->readsigned() << (8-gdepth);
				r += d;
				g += d;
				b += d;
				if ( r > 255 )  r = 255;
				if ( g > 255 )  g = 255;
				if ( b > 255 )  b = 255;
				if ( r < 0 )  r = 0;
				if ( g < 0 )  g = 0;
				if ( b < 0 )  b = 0;

				r += s->readsigned() << (8-rdepth);
				if ( r > 255 )  r = 255;
				if ( r < 0 )  r = 0;

				b += s->readsigned() << (8-bdepth);
				if ( b > 255 )  b = 255;
				if ( b < 0 )  b = 0;

				r &= 0x100 - (1<<(8-rdepth));
				g &= 0x100 - (1<<(8-gdepth));
				b &= 0x100 - (1<<(8-bdepth));

				pr[x] = r;
				pg[x] = g;
				pb[x] = b;
			}

			image->set( x++, y, r, g, b );

			if ( --count < 0  &&  x < width )
			{
				int n = s->readunsigned() + 1;

				for ( int i=0; i<n; i++ )
				{
					pr[x] = r;
					pg[x] = g;
					pb[x] = b;
					image->set( x++, y, r, g, b );
				}
			}
		}
	}

	if ( pr != prbuf )  delete[] pr;
	if ( pg != pgbuf )  delete[] pg;
	if ( pb != pbbuf )  delete[] pb;
}

void Transparent( CStream *s, CImageWriter *image, int width, int height, dword depth )
{

	for ( int y=0; y<height; y++ )
	{
		int		r = 0;
		int		g = 0;
		int		b = 0;
		int		a = 0;
		int		dg = 0;
		int		ta = 0;
		bool	f = true;
		int		samea = 0;
		int		samef = 0;

		for ( int x=0; x<width; )
		{
			if ( --samea < 0 )
			{
				ta += s->readsigned();

				if ( ta == 0 )
				{
					int n = s->readunsigned();
					x+=n+1;
					continue;
				}

				if ( ta == 32 )
					samea = s->readunsigned();

			}

			if ( --samef < 0 )
			{
				f = f ? false : true;

				samef = s->readunsigned();

				dg = 0;
			}

			if ( f )
			{
				if ( samea < samef )
				{
					x++;
				}
				else
				{
					if ( samef > 0 )
						samea -= samef;

					x+=samef+1;
					samef = 0;
				}
			}
			else
			{
				if ( s->readbit() == 1 )
				{
					x++;
				}
				else
				{

					s->readsigned();

					s->readsigned();
					s->readsigned();

					x++;
				}
			}
		}
	}

}


void TransparentO( CStream *s, CImageWriter *image, int width, int height, dword depth )
{
	int *	pr = new int[width];
	int *	pg = new int[width];
	int *	pb = new int[width];
	int *	pa = new int[width];

	for ( int y=0; y<height; y++ )
	{
		int		r = 0;
		int		g = 0;
		int		b = 0;
		int		a = 0;
		int		dg = 0;
		int		ta = 0;
		bool	f = true;
		int		samea = 0;
		int		samef = 0;

		for ( int x=0; x<width; )
		{
			if ( --samea < 0 )
			{
				ta += s->readsigned();

				if ( ta == 0 )
				{
					int n = s->readunsigned();
					for ( int i=0; i<n+1; i++ )
						pr[x]=0, pg[x]=0, pb[x]=0, pa[x]=0,
						image->set( x++, y, 0, 0, 0, 0 );
					continue;
				}

				if ( ta == 32 )
					samea = s->readunsigned();

				a = ta * 8;
				if ( a >= 256 )
					a = 255;
			}

			if ( --samef < 0 )
			{
				f = f ? false : true;

				samef = s->readunsigned();

				dg = 0;
			}
			
			if ( f )
			{
				if ( samea < samef )
				{
					pr[x]=r, pg[x]=g, pb[x]=b, pa[x]=a;
					image->set( x++, y, r, g, b, a );
				}
				else
				{
					if ( samef > 0 )
						samea -= samef;

					for ( int i=0; i<samef+1; i++ )
						pr[x]=r, pg[x]=g, pb[x]=b, pa[x]=a,
						image->set( x++, y, r, g, b, a );
					samef = 0;
				}
			}
			else
			{
				if ( s->readbit() == 1 )
				{
					r=pr[x], g=pg[x], b=pb[x];
					pa[x] = a;
					image->set( x++, y, r, g, b, a );
					dg = 0;
				}
				else
				{
					int tg = g + dg*2;
					if ( tg < 0 )  tg = 0;
					if ( tg > 255 )  tg = 255;
					tg -= g;
					if ( tg < -128 )  tg += 256;
					if ( tg > 127 )  tg -= 256;

					dg = tg/2 + s->readsigned();
					if ( dg < -128 )  dg = -128;
					if ( dg > 127 )  dg = 127;

					r += dg*2;
					g += dg*2;
					b += dg*2;

					r += s->readsigned() * 2;
					b += s->readsigned() * 2;

					if ( r > 255 )  r = 255;
					if ( g > 255 )  g = 255;
					if ( b > 255 )  b = 255;
					if ( r < 0 )  r = 0;
					if ( g < 0 )  g = 0;
					if ( b < 0 )  b = 0;
					
					r &= 0xfe;
					g &= 0xfe;
					b &= 0xfe;

					pr[x]=r, pg[x]=g, pb[x]=b, pa[x]=a;
					image->set( x++, y, r, g, b, a );
				}
			}
		}
	}

	delete[] pr;
	delete[] pg;
	delete[] pb;
	delete[] pa;
}

