#include<fstream>

IntMatrix hysthresh(double** img, int rows_img, int cols_img, double T1, double T2){
	IntMatrix mat_bin = newIntMatrix(rows_img,cols_img);
	int rc = rows_img * cols_img;
	int rcmr = rc - rows_img;
	int rp1 = rows_img + 1;
	int cont = 0;
	Vector bw = newDoubleVector(rc);
	std::vector<int> pix; 
	IntVector stack = newIntVector(rc);

	for (int i = 0; i < cols_img; ++i)
	{
		for (int j = 0; j < rows_img; ++j)
		{
			bw[cont] = img[j][i];
			
			stack[cont] = 0;
			if (bw[cont] > T1)
			{
				pix.push_back(cont+1); 
				
			}
			

			cont++;
		}
		
	}	


	 int npix = pix.size();
	
	for (int i = 0; i < npix; ++i)
	{
		stack[i] = pix[i];
		int index_pix = pix[i];
		bw[index_pix-1] = -1;
		
		
	}



	int O[] = {-1, 1, -rows_img-1, -rows_img, -rows_img+1, rows_img-1, rows_img, rows_img+1};	
	
    npix=npix-1;
	while (npix != 0){
		int v = stack[npix];
        
		npix--;
		if ((v > rp1) && (v < rcmr))
		{
			for (int l = 0; l < 8; ++l)
			{	int index = O[l] + v;
				int ind = index;
				if (bw[ind-1] > T2)
				{

					npix++;
					stack[npix] = ind;
					bw[ind-1] = -1;
				}
				
			}
		}
	}

int index_row = 0;
int index_col = 0;
for (int i = 0; i < rc; ++i)
{
	if (bw[i] == -1)
		bw[i] = 1;
	else
		bw[i] = 0;


	if (index_row == rows_img){
		index_row = 0;
		index_col++;
	}

	if (index_col == cols_img){
		break;
	}
	mat_bin[index_row][index_col] = int(bw[i]);
	
	index_row++;

}
pix.clear();
deleteDoubleVector(bw);
deleteIntVector(stack);
return mat_bin;

}