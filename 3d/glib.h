// Gram's shape library

#ifndef _GLIB_H
#define _GLIB_H

#include <stdio.h>
#include "app.h"
#include "mouse.h"
#include "tmatrix.h"

extern int PPdist;	// projection plane distance

// Stereo view 

extern int ThreeD;
extern int EyeSep;		// eye separation distance
const color_t LeftEyeColor = Red;
const color_t RightEyeColor = Blue;

typedef canvas_t	Canvas;

class Saveable
{
public:
	virtual void Save(FILE *fp) = 0;
	virtual void Load(FILE *fp) = 0;
};

class Point2D : public Saveable
{
	void	Copy(int x_in, int y_in);
public:
	int x, y;
	Point2D();
	Point2D(int x_in, int y_in);
	Point2D(Point2D &p);

	Point2D& operator=(const Point2D &rhs);

	void	Set(int x_in, int y_in);
	int		X();
	int		Y();

	void	Save(FILE *fp);
	void	Load(FILE *fp);
	int		ClipLB(int xmin, int ymin, int xmax, int ymax);
};

//-----------------------------------------------------------------

class Line2D : public Saveable
{
	Point2D start, end;
	void	Copy(const Point2D &start_in, const Point2D & end_in);
	void	Copy(const Line2D &l);
public:
	Line2D();
	Line2D(const Point2D &start_in, const Point2D &end_in);
	Line2D(const Line2D &l);

	Line2D& operator=(const Line2D &rhs);

	void	Set(const Point2D &start_in, const Point2D &end_in);
	Point2D	Midpoint();

	void	Save(FILE *fp);
	void	Load(FILE *fp);
	void	Draw(Canvas *c);
	int		ClipLB(int xmin, int ymin, int xmax, int ymax);
};

//----------------------------------------------------------------

#define MAXPOINTS	24

class Polygon2D : public Saveable
{
	int numpts;
	Point2D pts[MAXPOINTS];
	void	Copy(const Polygon2D &p);
public:
	Polygon2D();
	Polygon2D(Polygon2D &p);
	Polygon2D& operator=(const Polygon2D &rhs);

	void	Empty();
	int		Points();

	void	Line(Canvas *c, Point2D &start, Point2D &end);
	void	AddPoint(Canvas *c, int x, int y);
	void	AddPoint(Canvas *c, Point2D &p);
	void	Close(Canvas *c);

	void	Morph(Polygon2D &p1, Polygon2D &p2);

	void	Save(FILE *fp);
	void	Load(FILE *fp);
	void	Draw(Canvas *c);
};

//---------------------------------------------------------------------
// 3-d vertices

class Point3D : public Saveable
{
	inline void	Copy(const Point3D &v);
public:
	int x, y, z, t;
	inline void	Copy(int x_in, int y_in, int z_in);
	inline Point3D(const Point3D &v);
	inline Point3D& operator=(const Point3D &rhs);
	inline Point3D(int x_in = 0, int y_in = 0, int z_in = 0);

//void	Set(int x_in, int y_in, int z_in);
	inline int		X();
	inline int		Y();
	inline int		Z();

	void	Save(FILE *fp);
	void	Load(FILE *fp);
};

//-----------------------------------------------------------------

class Line3D : public Saveable
{
	Point3D start, end;
	void	Copy(const Point3D &start_in, const Point3D & end_in);
	void	Copy(const Line3D &l);
public:
	Line3D();
	Line3D(const Point3D &start_in, const Point3D &end_in);
	Line3D(const Line3D &l);

	Line3D& operator=(const Line3D &rhs);

	void	Set(const Point3D &start_in, const Point3D &end_in);
	Point3D	Midpoint();

	void	Save(FILE *fp);
	void	Load(FILE *fp);
	void	Draw(Canvas *c);
	int		ClipLB(int zmin);
};

//------------------------------------------------------------------------
// 3-d Saveable base class for wireframes and polygon fills.
// This is mostly just a set of vertices for wireframe and polygon objects

class Shape3D : public Saveable
{
	Point3D	   *vertices;	// local vertices
	Point3D	   *points;		// world vertices
	int				nv;
	void	Copy(const Shape3D &w);
public:
	inline Shape3D();
	inline Shape3D(const int nv_in);
	inline Shape3D(const Shape3D &s);
	inline Shape3D& operator=(const Shape3D &rhs);
	~Shape3D();

	void	SetVertices(const int nv_in, const int (&vertices_in)[][3]);
	inline void Init(); // copies local coords to world coords
	void	Transform(const TransformMatrix &m);
	void	Project(const int distance = 0);
	void	ProjectStereo(int distance = 0, int sep = 0);
	void	Frame(Point2D &tl, Point2D &br);
	inline Point3D& Vertex(int v);
	inline Point3D& Point(int v);
	inline void	Scale(const MatElt s);
	inline void	Rotate(const MatElt ax, const MatElt ay, const MatElt az);
	inline void	Translate(const MatElt xt, const MatElt yt, const MatElt zt);

	virtual void	Save(FILE *fp) = 0;
	virtual void	Load(FILE *fp) = 0;
	virtual void	Draw(Canvas *c, int x = 0, int y = 0) = 0;

	friend class WireFrame;
	friend class Object3D;
};

//------------------------------------------------------------------------
// 3-d Wire Frame Saveables

class WireFrame : public Shape3D
{
	int			  (*edges)[2];
	int			  ne;

	void	Copy(const WireFrame &w);
public:
	inline WireFrame();
	inline WireFrame(const int nv_in, const int ne_in);
	inline WireFrame(const WireFrame &w);
	inline WireFrame& operator=(const WireFrame &rhs);
	inline ~WireFrame();

	void	SetEdges(const int ne_in, const int (&edges_in)[][2]);

	void	Save(FILE *fp);
	void	Load(FILE *fp);

	void	Draw(Canvas *c, int x = 0, int y = 0);
	void	DrawStereo(Canvas *c, int x = 0, int y = 0);
};

//---------------------------------------------------------------------

class Polygon3D: public Saveable
{
	int	nv;				// Number of vertices
	int	color;			// Color of polygon
	int	zmax,zmin;		// Maximum and minimum coordinates of polygon
	int xmax,xmin;
	int ymax, ymin;
	Point3D **vertices;// Array of pointers to vertices
	int	sortflag;		// For hidden surface sorts
	void	Copy(const Polygon3D &p, Point3D *newbase, Point3D *oldbase);
	void	Copy(const Polygon3D &p);
	void	DrawEdge(Canvas *c, int &count, int &errorterm, int &xstart,
				int ystart, int xdiff, int ydiff, int xunit, int x, int y);
	void	CheckEdge(int &errorterm, int &xstart,
					int &count, int xdiff, int ydiff, int xunit);
public:
	inline Polygon3D();
	inline Polygon3D(const Polygon3D &p);
	inline Polygon3D& operator=(const Polygon3D &rhs);
	inline ~Polygon3D();

	void	Save(FILE *fp);
	void	Load(FILE *fp);

	int		Backface();
	void	Draw(Canvas *c, int x, int y);

	friend class Object3D; // because these aren't general polygons
		// but are specific to Object3Ds as they use vertex pointers.
};

//-----------------------------------------------------------------

class Object3D: public Shape3D
{
	int		np;				// Number of polygons
	int		x,y,z;			// World3D coordinates of object's local origin
	Polygon3D *polygons;	// Array of polygons in object
	int		convex;			// Is it a convex polyhedron?
	void	Copy(const Object3D &o);
public:
	void	SetPolygons(int npoly, Polygon3D *polys);
	void	SetPolygons(int npoly, int *(polys)[]);

	Object3D(int npoly = 0);
	inline Object3D(const Object3D &o);
	inline Object3D& operator=(const Object3D &rhs);
	inline ~Object3D();

	void Color(int color);
	void Convex(int cv)
	{
		convex = cv;
	}
	void	Save(FILE *fp);
	void	Load(FILE *fp);
	void	Draw(Canvas *c, int x = 0, int y = 0);
	void	DrawStereo(Canvas *c, int x = 0, int y = 0);
};

//---------------------------------------------------------------------

class World3D : public Saveable
{
	Point3D			viewloc;
	MatElt			xangle, yangle, zangle;
	int				no;				// Number of wireframe objects

	void	Copy(const World3D &w);
public:
	WireFrame	   *objects;		// Array of wireframe objects
	World3D(int nobj = 0);
	World3D(const World3D &w);
	World3D& operator=(const World3D &rhs);
	~World3D();

	void	Save(FILE *fp);
	void	Load(FILE *fp);

	void	TransformAll(TransformMatrix &tm);
	void	Move(MatElt dx, MatElt dy, MatElt dz);
	inline void	MoveTo(MatElt x, MatElt y, MatElt z);
	void	Turn(MatElt dax, MatElt day, MatElt daz);
	void	TurnTo(MatElt ax, MatElt ay, MatElt az);
	void	Project(const int distance);
	void	Draw(Canvas *c, int xpos = 0, int ypos = 0);
	void	DrawStereo(Canvas *c, int x = 0, int y = 0);
};

//-----------------------------------------------------------------
// Inline functions
//-----------------------------------------------------------------

void Point3D::Copy(int x_in, int y_in, int z_in)
{
	x = x_in;
	y = y_in;
	z = z_in;
	t = 1;
}

void Point3D::Copy(const Point3D &v)
{
	Copy(v.x, v.y, v.z);
}

Point3D::Point3D(const Point3D &v)
{
	Copy(v);
}

Point3D& Point3D::operator=(const Point3D &rhs)
{
	Copy(rhs);
	return *this;
}

Point3D::Point3D(int x_in, int y_in, int z_in)
{
	Copy(x_in, y_in, z_in);
}

int Point3D::X()
{
	return x;
}

int Point3D::Y()
{
	return y;
}

int Point3D::Z()
{
	return z;
}

//----------------------------------------------

Shape3D::Shape3D()
{
	nv = 0;
	points = vertices = NULL;
}

Shape3D::Shape3D(const int nv_in)
{
	nv = nv_in;
	vertices = new Point3D[nv];
	points = new Point3D[nv];
}

Shape3D::Shape3D(const Shape3D &s)
{
	vertices = points = NULL;
	Copy(s);
}

Shape3D& Shape3D::operator=(const Shape3D &rhs)
{
	Copy(rhs);
	return *this;
}

Shape3D::~Shape3D()
{
	if (vertices)
		delete [] vertices;
	if (points)
		delete [] points;
}

void Shape3D::Init() // copies local coords to world coords
{
	for (int i = 0; i < nv; i++)
		points[i] = vertices[i];
}

void Shape3D::Scale(const MatElt s)
{
	ScaleMatrix sm(s);
	Transform(sm);
}

void Shape3D::Rotate(const MatElt ax, const MatElt ay, const MatElt az)
{
	RotateMatrix rm(ax, ay, az);
	Transform(rm);
}

void Shape3D::Translate(const MatElt xt, const MatElt yt, const MatElt zt)
{
	TranslateMatrix tm(xt, yt, zt);
	Transform(tm);
}

Point3D& Shape3D::Vertex(int v)
{
	return vertices[v];
}

Point3D& Shape3D::Point(int v)
{
	return points[v];
}

//------------------------------------------------------

WireFrame::WireFrame()
	: Shape3D()
{
	ne = 0;
	edges = NULL;
}

WireFrame::WireFrame(const int nv_in, const int ne_in)
	: Shape3D(nv_in)
{
	ne = ne_in;
	edges = new int[ne][2];
}

WireFrame::WireFrame(const WireFrame &w)
	: Shape3D()
{
	edges = NULL;
	Copy(w);
}

WireFrame& WireFrame::operator=(const WireFrame &rhs)
{
	Copy(rhs);
	return *this;
}

WireFrame::~WireFrame()
{
	if (edges)
		delete [] edges;
}

//------------------------------------------------------

Polygon3D::Polygon3D()
{
	vertices = NULL;
	nv = 0;
}

Polygon3D::Polygon3D(const Polygon3D &p)
{
	vertices = NULL;
	Copy(p);
}

Polygon3D& Polygon3D::operator=(const Polygon3D &rhs)
{
	Copy(rhs);
	return *this;
}

Polygon3D::~Polygon3D()
{
	if (vertices) delete [] vertices;
}

//---------------------------------------------

Object3D::Object3D(const Object3D &o)
	: Shape3D()
{
	polygons = NULL;
	Copy(o);
}

Object3D& Object3D::operator=(const Object3D &rhs)
{
	Copy(rhs);
	return *this;
}

Object3D::~Object3D()
{
	if (polygons) delete [] polygons;
}

void World3D::MoveTo(MatElt x, MatElt y, MatElt z)
{
	Move(x - viewloc.x, y - viewloc.y, z - viewloc.z);
}

void World3D::TurnTo(MatElt ax, MatElt ay, MatElt az)
{
	Turn(ax - xangle, ay - yangle, az - zangle);
}

extern int ThreeD;

#endif

