#include <assert.h>
#include "glib.h"

int ThreeD = 0, PPdist = 600, EyeSep = 20;

// Liang-Barsky line clipping utility routine used for both 2d and 3d

int ClipT(int denom, int num, int &tE, int &tL)
{
	int t, accept = 1;
	if (denom > 0)
	{
		t = num / denom;
		if (t > tL) accept = 0;
		else if (t > tE) tE = t;
	}
	else if (denom < 0)
	{
		t = num / denom;
		if (t < tE) accept = 0;
		else if (t < tL) tL = t;
	}
	else if (num > 0) accept = 0;
	return accept;
}

//-----------------------------------------------------------

void Point2D::Copy(int x_in, int y_in)
{
	x = x_in;
	y = y_in;
}

Point2D::Point2D()
{
}

Point2D::Point2D(int x_in, int y_in)
{
	Copy(x_in, y_in);
}

Point2D::Point2D(Point2D &p)
{
	Copy(p.x, p.y);
}

void Point2D::Set(int x_in, int y_in)
{
	Copy(x_in, y_in);
}

Point2D& Point2D::operator=(const Point2D &rhs)
{
	Copy(rhs.x, rhs.y);
	return *this;
}

int Point2D::X()
{
	return x;
}

int Point2D::Y()
{
	return y;
}

void Point2D::Save(FILE *fp)
{
	fwrite(&x, sizeof(x), 1, fp);
	fwrite(&y, sizeof(y), 1, fp);
}

void Point2D::Load(FILE *fp)
{
	fread(&x, sizeof(x), 1, fp);
	fread(&y, sizeof(y), 1, fp);
}

int Point2D::ClipLB(int xmin, int ymin, int xmax, int ymax)
{
	if (x >= xmin && x <= xmax && y >= ymin && y <= ymax)
		return 1;
	return 0;
}

//---------------------------------------------------------------------

void Line2D::Copy(const Point2D &start_in, const Point2D & end_in)
{
	start = start_in;
	end = end_in;
}

void Line2D::Copy(const Line2D &l)
{
	Copy(l.start, l.end);
}

Line2D::Line2D()
{
}

Line2D::Line2D(const Point2D &start_in, const Point2D &end_in)
{
	Copy(start_in, end_in);
}

Line2D::Line2D(const Line2D &l)
{
	Copy(l);
}

void Line2D::Set(const Point2D &start_in, const Point2D &end_in)
{
	Copy(start_in, end_in);
}

Line2D& Line2D::operator=(const Line2D &rhs)
{
	Copy(rhs);
	return *this;
}

Point2D Line2D::Midpoint()
{
	Point2D p((start.X() + end.X())/2, (start.Y()+end.Y())/2);
	return p;
}

void Line2D::Draw(Canvas *c)
{
	c->Line(start.X(), start.Y(), end.X(), end.Y());
}

void Line2D::Save(FILE *fp)
{
	start.Save(fp);
	end.Save(fp);
}

void Line2D::Load(FILE *fp)
{
	start.Load(fp);
	end.Load(fp);
}

int Line2D::ClipLB(int xmin, int ymin, int xmax, int ymax)
{
	int	visible = 0,
		tE, tL,
		dx = end.X() - start.X(),
		dy = end.Y() - start.Y();
	if (dx == 0 && dy == 0 && start.ClipLB(xmin, ymin, xmax, ymax))
		visible = 1;
	else
	{
		tE = 0;
		tL = 1;
		if (ClipT(dx, xmin - start.X(), tE, tL))
		{
			if (ClipT(-dx, start.X() - xmax, tE, tL))
			{
				if (ClipT(dy, ymin - start.Y(), tE, tL))
				{
					if (ClipT(-dy, start.Y() - ymax, tE, tL))
					{
						visible = 1;
						if (tL < 1)
						{
							end.x = start.x + tL * dx,
							end.y = start.y + tL * dy;
						}
						if (tE > 0)
						{
							start.x += tE * dx;
							start.y += tE * dy;
						}
					}
				}
			}
		}
	}
	return visible;
}

//---------------------------------------------------------------------

Polygon2D::Polygon2D()
{
	numpts = 0;
}

Polygon2D::Polygon2D(Polygon2D &p)
{
	Copy(p);
}

Polygon2D& Polygon2D::operator=(const Polygon2D &rhs)
{
	Copy(rhs);
	return *this;
}

void Polygon2D::Line(Canvas *c, Point2D &start, Point2D &end)
{
	c->Line(start.X(), start.Y(), end.X(), end.Y());
}

void Polygon2D::Empty()
{
	numpts = 0;
}

void Polygon2D::AddPoint(Canvas *c, Point2D &p)
{
	AddPoint(c, p.X(), p.Y());
}

void Polygon2D::Close(Canvas *c)
{
	Line(c, pts[numpts-1], pts[0]);
}

int Polygon2D::Points()
{
	return numpts;
}

void Polygon2D::Copy(const Polygon2D &p)
{
	numpts = p.numpts;
	for (int i = 0; i < numpts; i++)
		pts[i] = p.pts[i];
}

void Polygon2D::Draw(Canvas *c)
{
	for (int i = 1; i < numpts; i++)
		Line(c, pts[i-1], pts[i]);
	Line(c, pts[i-1], pts[0]);
}

void Polygon2D::AddPoint(Canvas *c, int x, int y)
{
	pts[numpts].Set(x, y);
	if (numpts)
		Line(c, pts[numpts], pts[numpts-1]);
	numpts++;
	assert(numpts <= MAXPOINTS);
}

void Polygon2D::Morph(Polygon2D &p1, Polygon2D &p2)
{
	numpts = p1.numpts;
	for (int i = 0; i < numpts; i++)
	{
		Line2D l;
		l.Set(p1.pts[i], p2.pts[i]);
		pts[i] = l.Midpoint();
	}
}

void Polygon2D::Save(FILE *fp)
{
	fwrite(&numpts, sizeof(numpts), 1, fp);
	for (int i = 0; i < numpts; i++)
		pts[i].Save(fp);
}

void Polygon2D::Load(FILE *fp)
{
	fread(&numpts, sizeof(numpts), 1, fp);
	for (int i = 0; i < numpts; i++)
		pts[i].Load(fp);
}

//---------------------------------------------------------------------

void Point3D::Save(FILE *fp)
{
	fwrite(&x, sizeof(x), 1, fp);
	fwrite(&y, sizeof(y), 1, fp);
	fwrite(&z, sizeof(z), 1, fp);
}

void Point3D::Load(FILE *fp)
{
	fread(&x, sizeof(x), 1, fp);
	fread(&y, sizeof(y), 1, fp);
	fread(&z, sizeof(z), 1, fp);
	t = 1;
}

//---------------------------------------------------------------------

void Line3D::Copy(const Point3D &start_in, const Point3D & end_in)
{
	start = start_in;
	end = end_in;
}

void Line3D::Copy(const Line3D &l)
{
	Copy(l.start, l.end);
}

Line3D::Line3D()
{
}

Line3D::Line3D(const Point3D &start_in, const Point3D &end_in)
{
	Copy(start_in, end_in);
}

Line3D::Line3D(const Line3D &l)
{
	Copy(l);
}

void Line3D::Set(const Point3D &start_in, const Point3D &end_in)
{
	Copy(start_in, end_in);
}

Line3D& Line3D::operator=(const Line3D &rhs)
{
	Copy(rhs);
	return *this;
}

Point3D Line3D::Midpoint()
{
	Point3D p((start.X() + end.X())/2,
		  (start.Y()+end.Y())/2,
		  (start.Z()+end.Z())/2);
	return p;
}

void Line3D::Draw(Canvas *c)
{
	c->Line(start.X(), start.Y(), end.X(), end.Y());
}

void Line3D::Save(FILE *fp)
{
	start.Save(fp);
	end.Save(fp);
}

void Line3D::Load(FILE *fp)
{
	start.Load(fp);
	end.Load(fp);
}

int Line3D::ClipLB(int zmin)
{
	int	accept = 0,
		tmin = 0, tmax = 1,
		x0 = start.x,
		y0 = start.y,
		z0 = start.z,
		x1 = end.x,
		y1 = end.y,
		z1 = end.z,
		dx = x1 - x0,
		dz = z1 - z0;
	if (ClipT(-dx-dz, x0+z0, tmin, tmax))
	{
		if (ClipT(dx-dz, -x0+z0, tmin, tmax))
		{
			int dy = y1 - y0;
			if (ClipT(dy-dz, -y0+z0, tmin, tmax))
			{
				if (ClipT(-dy-dz, y0+z0, tmin, tmax))
				{
					if (ClipT(-dz, z0-zmin, tmin, tmax))
					{
						if (ClipT(dz, -z0-1, tmin, tmax))
						{
							accept = 1;
							if (tmax < 1)
							{
								end.x = x0 + tmax * dx;
								end.y = y0 + tmax * dy;
								end.z = z0 + tmax * dz;
							}
							if (tmin > 0)
							{
								start.x += tmin * dx;
								start.y += tmin * dy;
								start.z += tmin * dz;
							}
						}
					}
				}
			}
		}
	}
	return accept;
}

//------------------------------------------------------------------------

void Shape3D::Copy(const Shape3D &s)
{
	nv = s.nv;
	if (vertices)
		delete [] vertices;
	if (points)
		delete [] points;
	vertices = new Point3D[nv];
	assert(vertices);
	points = new Point3D[nv];
	assert(points);
	for (int v = 0; v < nv; v++)
	{
		vertices[v] = s.vertices[v];
		points[v] = s.points[v];
	}
}

void Shape3D::Transform(const TransformMatrix &m)
{
	for (int v=0; v < nv; v++)
	{
		Point3D &wp = points[v];
#ifdef FIXED
		long x, y, z;
		x = wp.x * m.mat[0][0] + wp.y * m.mat[1][0]
			+ wp.z * m.mat[2][0] + m.mat[3][0];

		y = wp.x * m.mat[0][1] + wp.y * m.mat[1][1]
			+ wp.z * m.mat[2][1] + m.mat[3][1];

		z = wp.x * m.mat[0][2] + wp.y * m.mat[1][2]
			+ wp.z * m.mat[2][2] + m.mat[3][2];
		wp.x = x / SCALE;
		wp.y = y / SCALE;
		wp.z = z / SCALE;
#else
		int x, y, z;
		x = wp.x * m.mat[0][0] + wp.y * m.mat[1][0]
			+ wp.z * m.mat[2][0] + m.mat[3][0];

		y = wp.x * m.mat[0][1] + wp.y * m.mat[1][1]
			+ wp.z * m.mat[2][1] + m.mat[3][1];

		z = wp.x * m.mat[0][2] + wp.y * m.mat[1][2]
			+ wp.z * m.mat[2][2] + m.mat[3][2];
		wp.x = x;
		wp.y = y;
		wp.z = z;
#endif
	}
}

// Divide x & y coords by z coords

void Shape3D::Project(const int distance)
{
	for (int v=0; v < nv; v++)
	{
		Point3D &wp = points[v];
		assert(distance < wp.z);
		wp.x = (int)((long)distance * (long)wp.x / (long)wp.z);
		wp.y = (int)((long)distance * (long)wp.y / (long)wp.z);
 	}
}

void Shape3D::SetVertices(const int nv_in, const int (&v_in)[][3])
{
	if (vertices)
		delete [] vertices;
	if (points)
		delete [] points;
	nv = nv_in;
	vertices = new Point3D[nv];
	assert(vertices);
	points = new Point3D[nv];
	assert(points);
	for (int v=0; v < nv; v++)
		vertices[v].Copy(v_in[v][0], v_in[v][1], v_in[v][2]);
}

void Shape3D::Frame(Point2D &tl, Point2D &br)
{
	// finds a bounding box

	tl.x = br.x = points[0].x;
	tl.y = br.y = points[0].y;
	for (int i = nv; --i ;)
	{
		Point3D p = points[i];
		if (p.y < tl.y) tl.y = p.y;
		if (p.x > br.x) br.x = p.x;
		if (p.y > br.y) br.y = p.y;
		if (ThreeD) p.x -= PPdist*EyeSep/p.z;
		if (p.x < tl.x) tl.x = p.x;
	}
	// step up by one for an actual frame
	tl.x--;
	tl.y--;
	br.x++;
	br.y++;
}

void Shape3D::ProjectStereo(int distance, int sep)
{
	if (distance==0) distance = PPdist;
	if (sep==0) sep = EyeSep;
	for (int v = nv; --v >= 0;)
	{
		assert(distance < points[v].z);
		points[v].x -= distance*sep/points[v].z;
	}
}

void Shape3D::Save(FILE *fp)
{
	fwrite(&nv, sizeof(nv), 1, fp);
	for (int v = 0; v < nv; v++)
		vertices[v].Save(fp);
}

void Shape3D::Load(FILE *fp)
{
	if (vertices) delete [] vertices;
	if (points) delete [] points;
	fread(&nv, sizeof(nv), 1, fp);
	vertices = new Point3D[nv];
	assert(vertices);
	points = new Point3D[nv];
	assert(points);
	for (int v = 0; v < nv; v++)
		vertices[v].Load(fp);
}

//---------------------------------------------------------------------

void WireFrame::Copy(const WireFrame &w)
{
	Shape3D::Copy(w);
	ne = w.ne;
	if (edges) delete [] edges;
	edges = new int[ne][2];
	assert(edges);
	for (int e = 0; e < ne; e++)
	{
		edges[e][0] = w.edges[e][0];
		edges[e][1] = w.edges[e][1];
	}
}

void WireFrame::SetEdges(const int ne_in, const int (&e_in)[][2])
{
	if (edges)
		delete [] edges;
	ne = ne_in;
	edges = new int[ne][2];
	for (int e = 0; e < ne; e++)
	{
		edges[e][0] = e_in[e][0];
		edges[e][1] = e_in[e][1];
	}
}

void WireFrame::Save(FILE *fp)
{
	Shape3D::Save(fp); // save vertices
	fwrite(&ne, sizeof(ne), 1, fp);
	for (int e = 0; e < ne; e++)
	{
		fwrite(&edges[e][0], sizeof(edges[e][0]), 1, fp);
		fwrite(&edges[e][1], sizeof(edges[e][1]), 1, fp);
	}
}

void WireFrame::Load(FILE *fp)
{
	Shape3D::Load(fp); // load vertices
	if (edges)
		delete [] edges;
	fread(&ne, sizeof(ne), 1, fp);
	edges = new int[ne][2];
	assert(edges);
	for (int e = 0; e < ne; e++)
	{
		fread(&edges[e][0], sizeof(edges[e][0]), 1, fp);
		fread(&edges[e][1], sizeof(edges[e][1]), 1, fp);
	}
}

void WireFrame::Draw(Canvas *c, int x, int y)
{
	for (int e = 0; e < ne; e++)
	{
		Point3D &s = Point(edges[e][0]);
		Point3D &f = Point(edges[e][1]);
		c->Line(x+s.X(), y+s.Y(), x+f.X(), y+f.Y());
	}
}

void WireFrame::DrawStereo(Canvas *c, int x, int y)
{
	for (int e = 0; e < ne; e++)
	{
		Point3D &s = Point(edges[e][0]);
		Point3D &f = Point(edges[e][1]);
		c->Foreground(LeftEyeColor);
		c->Line(x+s.X(), y+s.Y(), x+f.X(), y+f.Y());
		c->Foreground(RightEyeColor);
		c->Line(x+s.X() - PPdist*EyeSep/s.Z(), y+s.Y(),
			x+f.X() - PPdist*EyeSep/f.Z(), y+f.Y());
	}
}

//-----------------------------------------------------------------

void Polygon3D::Copy(const Polygon3D &p)
{
	nv = p.nv;
	color = p.color;
	xmax = p.xmax;
	xmin = p.xmin;
	ymax = p.ymax;
	ymin = p.ymin;
	zmax = p.zmax;
	zmin = p.zmin;
	sortflag = p.sortflag;
	if (vertices) delete [] vertices;
	vertices = new Point3D* [nv];
	assert(vertices);
	for (int v = 0; v < nv; v++)
		vertices[v] = p.vertices[v];
}

void Polygon3D::Copy(const Polygon3D &p, Point3D *newbase, Point3D *oldbase)
{
	Copy(p);
	// fix up vertices
	for (int v = 0; v < nv; v++)
		vertices[v] = newbase + (p.vertices[v]-oldbase);
}

int Polygon3D::Backface()
{
	// Returns 0 if convex polygon is visible, -1 if not.
	Point3D &v0 = *vertices[0];
	Point3D &v1 = *vertices[1];
	Point3D &v2 = *vertices[2];
	int z = (v1.x-v0.x) * (v2.y-v0.y) - (v1.y-v0.y) * (v2.x-v0.x);
	return(z>=0);
}

void Polygon3D::Save(FILE *fp)
{
	(void)fp;
}

void Polygon3D::Load(FILE *fp)
{
	(void)fp;
}

void Polygon3D::DrawEdge(Canvas *c, int &count, int &errorterm,
	int &xstart, int ystart, int xdiff, int ydiff, int xunit, int xoff, int yoff)
{
	// while not finished with edge...
	int sx = xoff + xstart;
	int y = yoff + ystart;
	if (count > 0 && errorterm < xdiff)
	{
		int cnt;
		if (ydiff > 0)
		{
			cnt = (xdiff - errorterm + ydiff - 1) / ydiff;
			if (cnt > count) cnt = count;
		}
		else cnt = count;
		int ex = sx + cnt * xunit;
		c->Line(sx+xunit, y, ex, y);
		xstart += cnt * xunit;
		errorterm += cnt * ydiff;
		count -= cnt;
	}
	// If time to increment X, restore error term
	errorterm -= xdiff;
}

void Polygon3D::CheckEdge(int &errorterm, int &xstart,
	int &count, int xdiff, int ydiff, int xunit)
{
	// Increment error term
	errorterm += xdiff;
	// If time to increment Y...
	if (errorterm >= ydiff)
	{
		// ...restore error term
		errorterm -= ydiff;
		// ...and advance offset to next pixel
		xstart += xunit;
  	}
	--count;
}

void Polygon3D::Draw(Canvas *c, int xoff, int yoff)
{
	c->Foreground((color_t)color);
#if 0
	// Using BGI fillpoly...
	int *pts = new int [nv*2];
	for (int i = 0; i < nv; i++)
	{
		pts[i*2] = xoff+vertices[i]->x;
		pts[i*2+1] = yoff+vertices[i]->y;
	}
	c->FillPoly(nv, pts, (color_t)color);
	delete [] pts;
#else
	// Uninitialized variables:

	int ydiff1,ydiff2,      // Difference between starting x and ending x
	    xdiff1,xdiff2,      // Difference between starting y and ending y
	    start,              // Starting offset of line between edges
	    length,             // Distance from edge 1 to edge 2
	    errorterm1,
	    errorterm2,		// Error terms for edges 1 & 2
	    count1,
	    count2,		// Increment count for edges 1 & 2
	    xunit1,
	    xunit2;		// Unit to advance x offset for edges 1 & 2

	// Initialize count of number of edges drawn:

	int edgecount = nv - 1;

	// Determine which vertex is at top of polygon:

	int firstvert = 0;           // Start by assuming vertex 0 is at top
	int min_amt = vertices[0]->y; // Find y coordinate of vertex 0
	// Search thru vertices
	for (int i=1; i < nv; i++)
	{
		// Is another vertex higher?
		if ((vertices[i]->y) < min_amt)
		{
			firstvert = i;                   // If so, replace previous top vertex
			min_amt = vertices[i]->y;
		}
	}
	// Finding starting and ending vertices of first two edges:

	int startvert1 = firstvert;      	// Get start vertex of edge 1
	int startvert2 = firstvert;      	// Get start vertex of edge 2
	int xstart1 = vertices[startvert1]->x;
	int ystart1 = vertices[startvert1]->y;
	int xstart2 = vertices[startvert2]->x;
	int ystart2 = vertices[startvert2]->y;
	int endvert1 = startvert1 - 1;		// Get end vertex of edge 1
	if (endvert1 < 0)
		endvert1 = nv - 1;		// Check for wrap
	int xend1 = vertices[endvert1]->x;	// Get x & y coordinates
	int yend1 = vertices[endvert1]->y;	// of ending vertices
	int endvert2 = startvert2 + 1;		// Get end vertex of edge 2
	if (endvert2 == nv) endvert2 = 0;  	// Check for wrap
	int xend2 = vertices[endvert2]->x;      	// Get x & y coordinates
	int yend2 = vertices[endvert2]->y;      	// of ending vertices

	// draw until all edges drawn

 	while (edgecount>0)
	{
		errorterm1 = 0;			// Initialize error terms
		errorterm2 = 0;			// for edges 1 & 2
		if ((ydiff1 = yend1-ystart1) < 0)
			ydiff1 = -ydiff1;	// Get absolute value of
		if ((ydiff2 = yend2-ystart2) < 0)
			ydiff2 = -ydiff2;	// x & y lengths of edges
		// Get value of length
 		if ((xdiff1 = xend1-xstart1)<0)
		{
			xunit1 = -1;		// Calculate X increment
			xdiff1 = -xdiff1;
		}
		else xunit1 = 1;
		// Get value of length
		if ( (xdiff2 = xend2-xstart2) < 0)
		{
			xunit2 = -1;		// Calculate X increment
			xdiff2 = -xdiff2;
		}
		else xunit2 = 1;

		// Choose which of four routines to use:

		int type;
		// If X length of edge 1 is greater than y length
		if (xdiff1 > ydiff1)
		{
			// If X length of edge 2 is greater than y length
			if (xdiff2 > ydiff2)
			{
				// Increment edge 1 on X and edge 2 on X:
				count1 = xdiff1;
				count2 = xdiff2;
				type = 0;
			}
			else
			{
				// Increment edge 1 on X and edge 2 on Y:
				count1 = xdiff1;
				count2 = ydiff2;
				type = 1;
			}
		}
		else
		{
			if (xdiff2 > ydiff2)
			{
				// Increment edge 1 on Y and edge 2 on X:
				count1 = ydiff1;
				count2 = xdiff2;
				type = 2;
			}
			else
			{
				// Increment edge 1 on Y and edge 2 on Y:
				count1 = ydiff1;
				count2 = ydiff2;
				type = 3;
			}
		}
		while (count1 && count2)
		{
			// first edge
			if (type<2)
			{
				DrawEdge(c, count1, errorterm1,
					xstart1, ystart1,
					xdiff1, ydiff1, xunit1,
					xoff, yoff);
			}
			else CheckEdge(errorterm1, xstart1, count1,
					xdiff1, ydiff1, xunit1);
			// second edge
			if (type%2 == 0)
				DrawEdge(c, count2, errorterm2,
					xstart2, ystart2,
					xdiff2, ydiff2, xunit2,
					xoff, yoff);
			else CheckEdge(errorterm2, xstart2, count2,
					xdiff2, ydiff2, xunit2);
			c->Line(xoff + xstart1, yoff + ystart1,
				xoff + xstart2, yoff + ystart2);
			ystart1++;
			ystart2++;
		}
		// Another edge (at least) is complete. Start next edge, if any.
		if (!count1)
		{           // If edge 1 is complete...
			--edgecount;           // Decrement the edge count
			startvert1=endvert1;   // Make ending vertex into start vertex
			--endvert1;            // And get new ending vertex
			if (endvert1<0) endvert1=nv-1; // Check for wrap
			xend1=vertices[endvert1]->x;  // Get x & y of new end vertex
			yend1=vertices[endvert1]->y;
		}
		if (!count2)
		{          // If edge 2 is complete...
			--edgecount;          // Decrement the edge count
			startvert2=endvert2;  // Make ending vertex into start vertex
			endvert2++;           // And get new ending vertex
			if (endvert2==nv) endvert2=0; // Check for wrap
			xend2=vertices[endvert2]->x;  // Get x & y of new end vertex
			yend2=vertices[endvert2]->y;
		}
	}
#endif
}

//-----------------------------------------------------------------

void Object3D::SetPolygons(int npoly, Polygon3D *polys)
{
	if (polygons) delete [] polygons;
	np = npoly;
	polygons = polys;
}

void Object3D::SetPolygons(int npoly, int *(polys)[])
{
	if (polygons) delete [] polygons;
	np = npoly;
	polygons = new Polygon3D[np];
	for (int p = 0; p < np; p++)
	{
		int nv;
		nv = polygons[p].nv = polys[p][0];
		polygons[p].color = polys[p][1];
		polygons[p].vertices = new Point3D* [nv];
		for (int v = 0; v < nv; v++)
		{
			polygons[p].vertices[v] = &points[polys[p][2+v]];
		}
	}
}

void Object3D::Copy(const Object3D &o)
{
	Shape3D::Copy(o);
	np = o.np;
	x = o.x;
	y = o.y;
	z = o.z;
	convex = o.convex;
	if (polygons) delete [] polygons;
	polygons = new Polygon3D[np];
	assert(polygons);
	for (int p = 0; p < np; p++)
	{
		polygons[p].Copy(o.polygons[p], points, o.points);
	}
}

Object3D::Object3D(int npolys)
	: Shape3D()
{
	np = npolys;
	if (np)
	{
		polygons = new Polygon3D[np];
		assert(polygons);
	}
	else polygons = NULL;
}

void Object3D::Save(FILE *fp)
{
	(void)fp;
}

void Object3D::Load(FILE *fp)
{
	(void)fp;
}

void Object3D::Color(int color)
{
	for (int p = 0; p < np; p++)
		polygons[p].color = color;
}

void Object3D::Draw(Canvas *c, int x, int y)
{
	for (int p=0; p < np; p++)
	{
		if (!convex || !polygons[p].Backface())
			polygons[p].Draw(c, x, y);
	}
}

void Object3D::DrawStereo(Canvas *c, int x, int y)
{
	Color(LeftEyeColor);
	//c->Foreground(LeftEyeColor);
	Draw(c, x, y);
	Object3D rv = *this;
	rv.Color(RightEyeColor);
	rv.ProjectStereo();
	//c->Foreground(RightEyeColor);
	rv.Draw(c, x, y);
}

//-----------------------------------------------------------------

void World3D::Copy(const World3D &w)
{
	no = w.no;
	if (objects) delete [] objects;
	objects = new WireFrame[no];
	assert(objects);
	for (int o = 0; o < no; o++)
		objects[o] = w.objects[o];
	xangle = w.xangle;
	yangle = w.yangle;
	zangle = w.zangle;
	viewloc = w.viewloc;
}

World3D::World3D(int nobj)
{
	no = nobj;
	if (no)
	{
		objects = new WireFrame[no];
		assert(objects);
	}
	else objects = NULL;
	xangle = yangle = zangle = 0;
	viewloc.x = viewloc.y = viewloc.z = 0;
}

World3D::World3D(const World3D &w)
{
	objects = NULL;
	Copy(w);
}

World3D& World3D::operator=(const World3D &rhs)
{
	Copy(rhs);
	return *this;
}

World3D::~World3D()
{
	if (objects) delete [] objects;
}

void World3D::Save(FILE *fp)
{
	viewloc.Save(fp);
	fwrite(&xangle, sizeof(xangle), 1, fp);
	fwrite(&yangle, sizeof(xangle), 1, fp);
	fwrite(&zangle, sizeof(xangle), 1, fp);
	fwrite(&no, sizeof(no), 1, fp);
	for (int o = 0; o < no; o++)
		objects[o].Save(fp);
}

void World3D::Load(FILE *fp)
{
	viewloc.Load(fp);
	fread(&xangle, sizeof(xangle), 1, fp);
	fread(&yangle, sizeof(xangle), 1, fp);
	fread(&zangle, sizeof(xangle), 1, fp);
	fread(&no, sizeof(no), 1, fp);
	for (int o = 0; o < no; o++)
		objects[o].Load(fp); // !!can't work as this is a base class
}

void World3D::TransformAll(TransformMatrix &tm)
{
	for (int o = 0; o < no; o++)
		objects[o].Transform(tm);
}

void World3D::Move(MatElt dx, MatElt dy, MatElt dz)
{
	TranslateMatrix tm(-dx, -dy, -dz);
	TransformAll(tm);
}

void World3D::Turn(MatElt dax, MatElt day, MatElt daz)
{
	RotateMatrix rm(-dax, -day, -daz);
	TransformAll(rm);
}

void World3D::Draw(Canvas *c, int xpos, int ypos)
{
	for (int o = 0; o < no; o++)
		objects[o].Draw(c, xpos, ypos);
	// Draw horizon
	c->Foreground(White);
//	c->Line(0, ypos, 640, ypos);
}

void World3D::DrawStereo(Canvas *c, int xpos, int ypos)
{
	for (int o = 0; o < no; o++)
		objects[o].DrawStereo(c, xpos, ypos);
}

void World3D::Project(const int distance)
{
	for (int o = 0; o < no; o++)
		objects[o].Project(distance);
}

//-----------------------------------------------------------------

