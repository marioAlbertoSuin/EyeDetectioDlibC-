// #include "double_functions.h"
// #include "int_functions.h"
#include "interp2.h"
//#define PI 3.14159265358979323846
Matrix normaliseiris(double **image,int row_image,int col_image,int x_iris,int y_iris,int r_iris,int x_pupil,int y_pupil,int r_pupil,int radpixels,int angulardiv)
{

int radiuspixels = radpixels + 2;
int angledivisions = angulardiv-1;

int ox = x_pupil - x_iris;
int oy = y_pupil - y_iris;
int sgn;

if (ox <= 0)
	sgn = -1;
else
	sgn = 1;

if (ox == 0 && oy > 0)
	sgn = 1;



double phi;
if (ox == 0)
	phi = PI / 2;
else
	phi = atan(oy / ox);

int a = pow(ox,2) + pow(oy,2);
double theta[angulardiv];
double r[angulardiv];

for (int i = 0; i < angulardiv; ++i)
{
	theta[i] = (2 * PI / angledivisions) * i;
	double b = sgn * cos(PI - phi - theta[i]);
	double aux = (a * pow(b,2)) - (a - pow(r_iris,2));
	double aux_r = (sqrt(a) * b) + sqrt(aux);
	r[i] = aux_r - r_pupil;
}

double** rmat=newDoubleMatrix(radiuspixels,angulardiv);

for (int i = 0; i < angulardiv; ++i)
{	double aux_rmat = r[i];
	for (int j = 0; j < radiuspixels; ++j)
		rmat[j][i] = aux_rmat;
}


double** xo = newDoubleMatrix(radpixels,angulardiv);
double** yo = newDoubleMatrix(radpixels,angulardiv);

for (int i = 1; i < radiuspixels-1; ++i)
{	double aux = i * (1/double(radiuspixels-1));
	
	for (int j = 0; j < angulardiv; ++j)
	{
		rmat[i][j] = (rmat[i][j] * aux) + double(r_pupil);
		xo[i-1][j] = x_pupil + (rmat[i][j] * cos(theta[j]));
		yo[i-1][j] = y_pupil - (rmat[i][j] * sin(theta[j]));
	}
}

deleteDoubleMatrix(rmat,radiuspixels);

int** x = newIntMatrix(row_image,col_image);
int** y = newIntMatrix(row_image,col_image);

meshgrid(x,y, row_image, col_image);


double ** resultado_ZI = interp2(x,y,image,row_image,col_image,xo,yo,radpixels,angulardiv);
for (int i = 0; i < radpixels; ++i)
{
	for (int j = 0; j < angulardiv; ++j)
	{
		resultado_ZI[i][j]=resultado_ZI[i][j]/255;

	}
}
 

deleteIntMatrix(x,row_image);
deleteIntMatrix(y,row_image);
deleteDoubleMatrix(xo,radpixels);
deleteDoubleMatrix(yo,radpixels);
return resultado_ZI;

}