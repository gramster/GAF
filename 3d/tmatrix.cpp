// 3-d transform matrices

#include <math.h>
#include "tmatrix.h"

#ifdef FIXED

int cos_table[256] =
{
     512,  511,  511,  510,  509,  508,  506,  504, 
     502,  499,  496,  493,  489,  486,  482,  477, 
     473,  468,  462,  457,  451,  445,  439,  432, 
     425,  418,  411,  403,  395,  387,  379,  370, 
     362,  353,  343,  334,  324,  315,  305,  295, 
     284,  274,  263,  252,  241,  230,  219,  207, 
     196,  184,  172,  160,  148,  136,  124,  112, 
     100,   87,   75,   63,   50,   38,   25,   12,    
       0,  -12,  -24,  -37,  -49,  -62,  -74,  -87, 
     -99, -111, -123, -136, -148, -160, -172, -183, 
    -195, -207, -218, -229, -240, -251, -262, -273, 
    -283, -294, -304, -314, -324, -333, -343, -352, 
    -361, -370, -378, -387, -395, -403, -410, -418, 
    -425, -432, -438, -445, -451, -457, -462, -467, 
    -472, -477, -481, -485, -489, -493, -496, -499, 
    -502, -504, -506, -508, -509, -510, -511, -511, 
    -511, -511, -511, -510, -509, -508, -506, -504, 
    -502, -499, -496, -493, -490, -486, -482, -478, 
    -473, -468, -463, -457, -451, -445, -439, -433, 
    -426, -419, -411, -404, -396, -388, -380, -371, 
    -362, -353, -344, -335, -325, -315, -305, -295, 
    -285, -274, -264, -253, -242, -231, -219, -208, 
    -196, -185, -173, -161, -149, -137, -125, -113, 
    -101,  -88,  -76,  -63,  -51,  -38,  -26,  -13, 
      -1,   11,   23,   36,   48,   61,   73,   86, 
      98,  110,  123,  135,  147,  159,  171,  183, 
     194,  206,  217,  229,  240,  251,  262,  272, 
     283,  293,  303,  313,  323,  333,  342,  352, 
     361,  369,  378,  386,  394,  402,  410,  417, 
     424,  431,  438,  444,  450,  456,  462,  467, 
     472,  477,  481,  485,  489,  493,  496,  499, 
     501,  504,  506,  507,  509,  510,  511,  511
};

MatElt Sine(int a)
{
	return cos_table[(a+192)&255]/2;
}

MatElt Cosine(int a)
{
	return cos_table[a&255]/2;
}

#endif

void TransformMatrix::Zero()
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			mat[i][j] = 0;
}

void TransformMatrix::Identity()
{
	Zero();
	for (int i = 0; i < 4; i++)
#ifdef FIXED
		mat[i][i] = SCALE;
#else
		mat[i][i] = 1;
#endif
}

TransformMatrix TransformMatrix::operator*(const TransformMatrix &rhs)
{
	TransformMatrix product;
	product.Zero();
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{
			for (int k=0; k<4; k++)
			{
#ifdef FIXED
				long l = mat[i][k] * rhs.mat[k][j] / SCALE;
				product.mat[i][j] += l;
#else
				product.mat[i][j] += mat[i][k] * rhs.mat[k][j];
#endif
			}
		}
	}
	return product;
}

void TransformMatrix::Multiply(TransformMatrix &m)
{
	*this = m * (*this);
}

void TransformMatrix::Scale(MatElt s)
{
	ScaleMatrix scale(s); Multiply(scale);
}

void TransformMatrix::Rotate(MatElt ax, MatElt ay, MatElt az)
{
	RotateXMatrix x_rot(ax); Multiply(x_rot);
	RotateYMatrix y_rot(ay); Multiply(y_rot);
	RotateZMatrix z_rot(az); Multiply(z_rot);
}

void TransformMatrix::Translate(MatElt xt, MatElt yt, MatElt zt)
{
	TranslateMatrix tmat(xt, yt, zt);
	*this = (*this) * tmat;
}

ScaleMatrix::ScaleMatrix(MatElt s)
	: TransformMatrix()
{
#ifdef FIXED
	s *= SCALE;
#endif
	for (int i=0; i<3; i++)
		mat[i][i] = s;
}

// Rotation by r degrees about x axis

RotateXMatrix::RotateXMatrix(MatElt r)
	: TransformMatrix()
{
	mat[1][1] =  Cosine(r);
	mat[1][2] =  Sine(r);
	mat[2][1] = -Sine(r);
	mat[2][2] =  Cosine(r);
}

// Rotation by r degrees about y axis

RotateYMatrix::RotateYMatrix(MatElt r)
	: TransformMatrix()
{
	mat[0][0] =  Cosine(r);
	mat[0][2] = -Sine(r);
	mat[2][0] =  Sine(r);
	mat[2][2] =  Cosine(r);
}

// Rotation by r degrees about z axis

RotateZMatrix::RotateZMatrix(MatElt r)
	: TransformMatrix()
{
	mat[0][0] =  Cosine(r);
	mat[0][1] =  Sine(r);
	mat[1][0] = -Sine(r);
	mat[1][1] =  Cosine(r);
}

RotateMatrix::RotateMatrix(MatElt ax, MatElt ay, MatElt az)
	: TransformMatrix()
{
	Rotate(ax, ay, az);
}

// Translation moves origin to (-xt, -yt, -zt)

TranslateMatrix::TranslateMatrix(MatElt xt, MatElt yt, MatElt zt)
	: TransformMatrix()
{
#ifdef FIXED
	mat[3][0] = xt * SCALE;
	mat[3][1] = yt * SCALE;
	mat[3][2] = zt * SCALE;
#else
	mat[3][0] = xt;
	mat[3][1] = yt;
	mat[3][2] = zt;
#endif
}

