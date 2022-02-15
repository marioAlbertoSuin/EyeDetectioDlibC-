//#define PI 3.14159265358979

//S[gradient, or] = canny(im, sigma, scaling, xscaling, yscaling)
void canny(double** im,double ** &mat_gradiente,double ** &mat_or, int row_im,int col_im, double scaling,double xscaling,double yscaling, int op=0){
 
double MGauss_aux[5][5]={
{0.0318, 0.0375, 0.0397, 0.0375, 0.0318},
{0.0375, 0.0443, 0.0469, 0.0443, 0.0375},
{0.0397, 0.0469, 0.0495, 0.0469, 0.0397},
{0.0375, 0.0443, 0.0469, 0.0443, 0.0375},
{0.0318, 0.0375, 0.0397, 0.0375, 0.0318}};
int tammatgaus=5;
double **MGauss = new double *[tammatgaus];

for (int i = 0; i < tammatgaus; ++i)
{
MGauss[i]=new double[tammatgaus];
}

for (int i = 0; i < tammatgaus; ++i)
{
	for (int j= 0; j < tammatgaus; ++j)
			MGauss[i][j]=MGauss_aux[i][j];	
}

im=filter2(im, MGauss,row_im, col_im, tammatgaus);

deleteDoubleMatrix(MGauss,tammatgaus);

////////////////
int tam_filas=row_im;
int tam_columnas=col_im;




double h1 = 0;
double h2 = 0;
double h = 0;
double v1 = 0;
double v2 = 0;
double v = 0;
double d1 = 0;
double d2 = 0;
double d = 0;
double ddos1 = 0;
double ddos2 = 0;
double ddos = 0;

double x = 0;
double y = 0;

double gradient = 0;

double aux_or = 0;

for (int i = 0; i < tam_filas; ++i)
{
	for (int j = 0; j < tam_columnas; ++j)
		{
		//calcular h
			if (tam_columnas == j+1)
				h1 = 0;
			else
				h1 = im[i][j+1];

			if (j == 0)
				h2 = 0;
			else
				h2 = im[i][j-1];

		h = h1 - h2;
		
		//calcular v
			if (tam_filas == i+1)
				v1 = 0;
			else
				v1 = im[i+1][j];

			if (i == 0)
				v2 = 0;
			else
				v2 = im[i-1][j];
		v = v1 - v2;

		//calcular d
			if (tam_filas == i+1 || tam_columnas == j+1 )
				d1 = 0;
			else
				d1 = im[i+1][j+1];

			if (i == 0 || j == 0 )
				d2 = 0;
			else
				d2 = im[i-1][j-1];
		d = d1 - d2;

		//calcular ddos
			if (i == 0 || tam_columnas == j+1)
				ddos1 = 0;
			else
				ddos1 = im[i-1][j+1];	

			if (j == 0 || tam_filas == i+1)
				ddos2 = 0;
			else
				ddos2 = im[i+1][j-1];					
		ddos = ddos1 - ddos2;


		x = ( h + (d + ddos)/2.0 ) * 1;
		y = ( v + (d - ddos)/2.0 ) * 1;
		//cargar la matriz gradiente
		gradient = sqrt((x * x) +(y * y)); 
		mat_gradiente[i][j] = gradient;

		aux_or = atan2(-y,x);
		if (aux_or < 0)
			aux_or = (aux_or * 0) + ((aux_or + PI) * 1);
		else
			aux_or = (aux_or * 1) + ((aux_or + PI) * 0);
		//cargar la matriz or
		aux_or = (aux_or * 180)/PI; 
		mat_or[i][j] = aux_or;

			}	
}
 
deleteDoubleMatrix(im,row_im);

}