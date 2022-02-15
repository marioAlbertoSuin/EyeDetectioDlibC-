
IntVector detectar_circulo(int** matimg,int numer_filas,int numero_columnas,int minr,int maxr, float reduccion = 0,int op=0){
 
	IntVector result=newIntVector(3);
	float reduccion_filas_columnas = reduccion;
	short int recorte_theta=2;

	short int recortex = numero_columnas * reduccion_filas_columnas;//opcional reducir columanas o dejar recortex=0;
	short int recortey = numer_filas * reduccion_filas_columnas;   //opcional reducir filas o dejar recortey=0;
	int dimZ = int(maxr - minr + 1);

	short int ***accum;

	accum = new short int**[numer_filas];
	for(short int x = 0; x < numer_filas; ++x) {
		accum[x] = new short int*[numero_columnas];
		for(short int y = 0; y < numero_columnas; ++y) {
			accum[x][y] = new short int[dimZ];
			for(short int z = 0; z < dimZ; ++z) {
				accum[x][y][z] = 0;
			}
		}
	}

	short int maximo=0;
	short int r=0;
	short int ejex=0;
	short int ejey=0;


	float  *veccos=new float[360];
	float  *vecsin=new float[360];






	for (short int i = 1; i <= 360; i+=recorte_theta)
	{
		veccos[i] = cos(i*PI/180);
		vecsin[i] = sin(i*PI/180);
	}


	short int indice1=0;
	short int indice2=0;
	short int indice3=0;
	for (short int x = recortex; x < numero_columnas - recortex ; ++x)
	{
		for (short int y = recortey; y < numer_filas - recortey; ++y)
		{
			for (short int rad = minr; rad < maxr; ++rad)
			{
				if (*(*(matimg+y)+x)==1){
					for (short int theta = 1; theta <= 360; theta+=recorte_theta)
					{
						indice2=(veccos[theta] * rad)+x;
						indice1=(vecsin[theta] * rad)+y;
						if (((indice2)<numero_columnas)&&((indice2)>0)&&((indice1)<numer_filas)&&((indice1)>0)){
							indice3 = (rad-minr+1);
							accum[indice1][indice2][indice3] = accum[indice1][indice2][indice3] + 1;

						}
					}
				}
			}
		}
	}






	for (short int x = 0; x < numer_filas; ++x)
	{
		for (short int y = 0; y < numero_columnas; ++y)
		{
			for (short int z = 0; z < dimZ; ++z)
			{
				if (maximo < accum[x][y][z]){
					r=z;
					ejey=x;
					ejex=y;
					maximo=accum[x][y][z];
				}
			}
		}
	}


	r= r+minr;
  
	deleteIntMatrix3D(accum,numer_filas,numero_columnas);
	delete[]veccos;
	veccos=nullptr;
	delete[]vecsin;
	vecsin=nullptr;
//////imrpimr imrprime x,y,r,maximo
//std::cout<<"<<imprecion del archivo cicle_hough.h>>  "<<ejex+1<<" , "<<ejey+1<<" "<<r<<" maximo "<<maximo<<std::endl;
	result[0]=ejex+1;result[1]=ejey+1;result[2]=r-1;
	return result;

}