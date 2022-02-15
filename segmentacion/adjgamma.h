//#include<math.h>
double max_val(double** mat,int rows_mat, int cols_mat){
	double max=0.0;
	for (int i = 0; i < rows_mat; ++i)
	{
		for (int j = 0; j < cols_mat; ++j)
		{
			if (max <= mat[i][j])
				max=mat[i][j];
			
		}
	}
	return max;
}
double min_val(double** mat,int rows_mat, int cols_mat){
	double min=255.0;
	for (int i = 0; i < rows_mat; ++i)
	{
		for (int j = 0; j < cols_mat; ++j)
		{
			if (min >= mat[i][j])
				min=mat[i][j];
		}
	}
	return min;
}

void adjgamma(double** &img,int rows_img,int cols_img,double g){
	double min = min_val(img,rows_img,cols_img);
	double max = 0;
	for (int i = 0; i < rows_img; ++i)
	{
		for (int j = 0; j < cols_img; ++j){
			img[i][j] = img[i][j] - min;
			if(img[i][j] >= max)
				max=img[i][j];
		}
	}
	for (int i = 0; i < rows_img; ++i)
	{
		for (int j = 0; j < cols_img; ++j){
			img[i][j] = img[i][j] / max;
			img[i][j] = pow(img[i][j],(1/g));
		}
	}

}



