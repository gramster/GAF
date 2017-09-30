// 3-d transform matrices

#ifndef _TMATRIX_H
#define _TMATRIX_H

// Matrix element type. Ideally this would be a class
// but that introduces too much overhead.

#ifdef FIXED

typedef long		MatElt;
#define SCALE		256L

MatElt Sine(int a);
MatElt Cosine(int a);

#else

typedef float		MatElt;

#define Sine	sin
#define Cosine	cos

#endif

class TransformMatrix
{
public:
	MatElt	mat[4][4];
	void Zero();
	void Identity();
	TransformMatrix()
	{
		Identity();
	}
	TransformMatrix operator*(const TransformMatrix &rhs);
	void Multiply(TransformMatrix &m);
	void Scale(MatElt s);
	void Rotate(MatElt ax, MatElt ay, MatElt az);
	void Translate(MatElt xt, MatElt yt, MatElt zt);
};

class RotateXMatrix : public TransformMatrix
{
public:
		RotateXMatrix(MatElt r);
};

class RotateYMatrix : public TransformMatrix
{
public:
		RotateYMatrix(MatElt r);
};

class RotateZMatrix : public TransformMatrix
{
public:
		RotateZMatrix(MatElt r);
};

class RotateMatrix : public TransformMatrix
{
public:
		RotateMatrix(MatElt ax, MatElt ay, MatElt az);
};

class ScaleMatrix : public TransformMatrix
{
public:
		ScaleMatrix(MatElt s);
};

class TranslateMatrix : public TransformMatrix
{
public:
		TranslateMatrix(MatElt xt, MatElt yt, MatElt zt);
};

#endif

