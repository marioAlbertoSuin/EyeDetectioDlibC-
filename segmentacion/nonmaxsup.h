
Matrix  nonmaxsup(double**inimage,int rows_inimage,int cols_inimage,double** orient,int rows_orient,int cols_orient,double radius){

double iradius = ceil(radius);
double xoff[181];
double yoff[181];
double hfrac[181];
double vfrac[181];
Matrix resultado = newDoubleMatrix(rows_inimage, cols_inimage);

for (int i = 0; i <= 180; ++i)
{
	double angle = ((i * PI )/ 180);
	xoff[i] = radius * cos(angle);
	yoff[i] = radius * sin(angle);

	hfrac[i] = xoff[i] - floor(xoff[i]);
	vfrac[i] = yoff[i] - floor(yoff[i]);

}

for (int i = 0; i < rows_orient; ++i)
{
	for (int j = 0; j < cols_orient; ++j)
	{	int fix = int(orient[i][j]);
		orient[i][j] = double(fix) + 1;
	}
	
}



for (int i = int(iradius); i < int(rows_inimage - iradius) ; ++i)
{
	for (int j = int(iradius); j < int(cols_inimage - iradius); ++j)
	{
		int or1 = int(orient[i][j]);
		double x = double(j+ xoff[or1-1]+1);
		double y = double(i - yoff[or1 - 1 ] + 1);
		
		int fx = int(floor(x));
		int cx = int(ceil(x));
		int fy = int(floor(y));
		int cy = int(ceil(y));

		double tl = inimage[fy - 1][fx - 1];
		double tr = inimage[fy - 1][cx - 1];
		double bl = inimage[cy - 1][fx - 1];
		double br = inimage[cy - 1][cx - 1];

		double upperavg = tl + hfrac[or1 - 1] * (tr - tl);
		double loweravg = bl + hfrac[or1 - 1] * (br - bl);
		double v1 = upperavg + vfrac[or1 - 1] * (loweravg - upperavg);


		if (inimage[i][j] > v1)
		{


		 x = double((j+1) -xoff[or1-1]);
		 y = double(i + yoff[or1 - 1 ] + 1);
		
		 fx = int(floor(x));
		 cx = int(ceil(x));
		 fy = int(floor(y));
		 cy = int(ceil(y));

		 tl = inimage[fy - 1][fx - 1];
		 tr = inimage[fy - 1][cx - 1];
		 bl = inimage[cy - 1][fx - 1];
		 br = inimage[cy - 1][cx - 1];

		 upperavg = tl + hfrac[or1 - 1] * (tr - tl);
		 loweravg = bl + hfrac[or1 - 1] * (br - bl);
		 double v2 = upperavg + vfrac[or1 - 1] * (loweravg - upperavg);

			if (inimage[i][j] > v2)
				resultado[i][j] = inimage[i][j];

		}
	}
}

return resultado;
}