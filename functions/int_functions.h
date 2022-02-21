/*
** Universidad Politécnica Salesiana
** Ingeniería de Sistemas
** Autores: Farinango Abel
**			Sandoval Leonel
** Última modificación 2021
*/
#include <vector>
typedef int **IntMatrix;
typedef int *IntVector;
typedef int ***IntMatrix3D;
void WritwResult(IntMatrix ,int , int );
void deleteIntMatrix(IntMatrix mat, int row){
	/*
	** Mat: matriz de tipo double que va ser liberado de memoria.
	** row: número de filas que tiene la matriz
	** Nota: Esta función libera la memoria ocupada por el apuntador.
	*/
	for (short int i = 0; i < row; ++i)
	{
		delete[]mat[i];
		mat[i]=nullptr;
	}

	delete[]mat;
	mat=nullptr;
}
void deleteIntVector(IntVector vect){
	/* 
	** vect: vector puntero de tipo double
	** Esta función libera memoria del apuntador tipo vector
	*/
	delete[] vect;
	vect=nullptr;
}

IntVector newIntVector(int length,int value=0){
	/*
	** length: tamaño del vector a crear
	** value: valor con el que se inicializa el vector (opcional, defarult 0)
	** Nota: Esta función crea un vector apuntador de tipo double
	*/
	IntVector vect=new int[length];
	for (int i = 0; i < length; ++i)
	{
		vect[i]=value;
	}

	return vect;
}

IntVector fillIntVector(int start, int end){
	/*
	** start: valor inicial
	** end: valor final
	** Nota: esta función retorna un vector double llenado con los valores dentro de rango start - end
	*/
	int length=end-start;
	int cont=start;
	IntVector vect=newIntVector(length+1);
	for (int i = 0; i <=length; ++i)
	{
		vect[i]=cont;
		cont++;
	}
	return vect;
}

IntMatrix newIntMatrix(int row, int col, int value=0){
	/*
	** row: número de filas de la matriz
	** col: número de columnas de la matriz
	** value (opcional): valor con la que se inicializa, por defecto 0
	** Nota: Esta funcion devuelve 
	*/
	IntMatrix vect=new int *[row];
	for (short int i = 0; i < row; ++i)
	{
		vect[i]=new int[col];
		for (short int j= 0; j < col; ++j)
		{
			vect[i][j]=value;
		}
	}

return vect;

}


IntMatrix fillIntMatriz(int rows, int cols, double **data, int startrow=0, int startcol=0){
	/*
	** start: valor inicial
	** end: valor final
	** Nota: esta función retorna un vector double llenado con los valores dentro de rango start - end
	*/
	IntMatrix result=newIntMatrix(rows,cols);

	for (int i = startrow; i < rows+startrow; ++i)
	{
		for (int j = startcol; j < cols+startcol; j++)
		{
			result[i-startrow][j-startcol]=data[i][j];
		}
	}
	return result;
}

IntVector Intdiff(IntVector vect, int length){
	/*
	** vect: vector de datos
	** length: tamaño del vector
	** Nota: Esta función retorna un vector con los valores de las diferencias de los elementos del 
	**       vector de entrada,
	*/
	IntVector result=newIntVector(length-1);//REDUCIR -1
	int aux=0;
	for (int i = 0; i <length-1; ++i)
	{
		aux=(vect[i+1]-vect[i]);
		result[i]=aux;
	}

	return result;
}

void Intdiff2(IntVector vect, int length, IntMatrix &mat, int sizeRow, int sizeCol){
	/*
	** vect: vector de datos
	** length: tamaño del vector
	** mat: matriz de indices
	** sizerow: filas de la matriz
	** sizecol: columas de la matriz
	** Nota: esta función retorna una matriz con los valores de los indices(En Matlab diff(x)(index))
	*/
	IntVector diff=Intdiff(vect,length);
    int index=1;
	for (short int i = 0; i < sizeRow; ++i)
	{
		for (short int j = 0; j < sizeCol; ++j)
		{
			for (int z = 0; z < length; ++z)
			{
				if (mat[i][j]==index)
				{
					mat[i][j]=diff[z];
					
					break;
				}
				index ++;
			}

			index=1;
		}
	}

	deleteIntVector(diff);//debug

}

IntMatrix lookup(IntVector vect, double **mat, int sizeRow, int sizeCol,int vectLength){
	/*
	**Referencia: https://octave.sourceforge.io/octave/function/lookup.html
	*/
	IntMatrix result=newIntMatrix(sizeRow,sizeCol);
	int dato;  //dato;
	for (short int i = 0; i < sizeRow; ++i)
	{
		for (short int j = 0; j < sizeCol; ++j)
		{	dato=0;
			for (int k = 0; k < vectLength; ++k) // k=1, veclength-1
			{
				if (mat[i][j]>=vect[k]){
					dato=dato+1;	             
				}

			}
			 result[i][j]=dato; //almacenar la nueva matriz retorna mat[i][j]=(dato+1);
		}
	}
	return result;
}

void meshgrid(IntMatrix  &X, IntMatrix &Y, int row, int col){
	/*
	** X: es una matriz en la que cada fila es una copia de x.
	** Y: es una matriz en la que cada columna es una copia de y
	** row: número de filas de las matrices
	** col: número de columnas de las matrices
	** Nota: devuelve coordenadas de cuadrícula 2-D basadas en las coordenadas contenidas en los vectores x y y.
	** Referencia: https://es.mathworks.com/help/matlab/ref/meshgrid.html
	*/

	for (short int i = 0; i < row; ++i)
	{
		for (short int j = 0; j < col; ++j)
		{
			X[i][j]=j+1;
			Y[i][j]=i+1;
		}
		
	}
}

IntVector mat2vect(IntMatrix mat, int sizerow, int sizecol ){
	/*
	** mat: Matriz 2-D que contiene datos que van a ser pasados a un vector 1-D
	** sizerow: Número de filas de la matriz
	** sizecol: Número de columnas de la matriz
	** NOTA: Esta función retorna un vector con todos los datos de la matriz (Mathlab: matriz(:))  
	*/
	int length=sizecol*sizerow;
	IntVector vect=newIntVector(length);
	int cont=0;
	for (short int i = 0; i < sizecol; ++i)
	{
		for (short int j = 0; j < sizerow; ++j)
		{
			*(vect+cont)=*(*(mat+j)+i);
			cont++;
		}
	}

	return vect;

}

IntVector sub2ind(int sizerow, int sizecol, IntVector rows, IntVector cols, int length ){
	/*
	** sizerow: valor de n filas que se utiliza para crear la matriz indice.
	** sizecol: valor de n columnas que se utiliza para crear la matriz indice
	** rows: vector escalar que tiene los valores de posición fila
	** cols: vector escalar que tiene los valores de posición columna
	** length: tamaño de los vectores rows, cols
	** NOTA: Esta funcion devuelve un vector de una dimención con los valores encontrados
	**       en el vector indice.
	** Referencia: https://www.mathworks.com/help/matlab/ref/sub2ind.html#mw_b3e2d533-ee9e-4d02-b23c-c1ba4c48244b
	*/
	int pos=0,index=1;
	IntMatrix  matrix=newIntMatrix(sizerow,sizecol);
	IntVector result=newIntVector(length);
	int x,y;
	while (pos<sizecol){
		for (int i = 0; i < sizerow; ++i)
		{
			*(*(matrix+i)+pos)=index;
			index++;

		}
		pos++;
	}

	for (int i = 0; i < length; ++i)
	{
		x=*(cols+i);
		y=*(rows+i);
		*(result+i)=*(*(matrix+(y-1))+(x-1));
	}
	//WritwResult(matrix,sizerow,sizecol);

	deleteIntMatrix(matrix, sizerow);
	return result;
}

IntMatrix vect2mat(IntVector vect,int sizeVect, int nCols){
	/*
	** vect: Vector 1-D que tiene los datos que van a ser colocados en una matriz 2-D
	** sizeVect: Tamaño del vector de datos.
	** nCols: El número de columnas que se necesita
	** NOTA: Esta función retorna una matriz transpuesta.
	*/
	IntMatrix mat;
	int nrows=nCols,cont=0;
	if (sizeVect % nCols==0)
	{
		nCols=sizeVect/nrows;
		mat=newIntMatrix( nrows,nCols);
		for (short int i = 0; i < nCols; ++i)
		{
			for (short int j = 0; j < nrows; ++j)
			{
				*(*(mat+j)+i)=*(vect+cont);
				cont++;
			}

		}
	}
	return mat;
}

IntMatrix sub2ind2D(IntMatrix matdata, int row, int col, IntMatrix matindex, int indexrow, int indexcol){
	/*
	**	matdata: matriz que contiene los datos
	**  row: número de filas de matdat
	**  col: columnas de matadata
	**  matindex: matriz de indices
	**  indexrow: filas de la matriz indice
	**  indexcol: columnas de la matriz
	**  Nota: Esta función retorna los valores de los indices encontrados. 
	*/
	IntVector vect_matdata=mat2vect(matdata,row,col);
	IntVector vect_index=mat2vect(matindex,indexrow,indexcol);
	IntVector values=newIntVector(indexcol*indexrow);
	for (int i = 0; i < indexcol*indexrow; ++i)
	{
		values[i]=vect_matdata[vect_index[i]-1];
	}

	IntMatrix result=vect2mat(values,indexcol*indexrow,indexrow);
	deleteIntVector(vect_index);
	deleteIntVector(vect_matdata);
	deleteIntVector(values);
	return result;


}

void  valInd(IntVector vect, int length , IntMatrix &mat, int row, int col){
	/*
	**  en matlab vect(mat);
	** vect: vector de datos
	** length: tamaño del vector
	** mat: Matriz de indices
	** row: filas de las matrices
	** col: número de columnas de la matriz
	** Nota: esta función retorna una matriz con los valores encontrados en el vector de acuerdo al indice de la matriz
	*/
	for (short int i = 0; i < row; ++i)
	{
		for (short int j = 0; j < col; ++j)
		{
			int index=1;
			for (int z = 0; z < length; ++z)
			{
				if (mat[i][j]==index)
				{
					mat[i][j]=vect[z];
					break;
				}
				index ++;
			}
		}
	}
}

IntMatrix cpIntMatrix(IntMatrix mat, int row, int col){
	/*
	** mat: matriz de datos origen
	** row: número de filas de la matriz
	** col: número de columnas de la matriz
	** Nota: Esta función retorna la una copia de la matriz mat.
	*/
	IntMatrix result=newIntMatrix(row,col);
	for (short int i = 0; i < row; ++i)
	{
		for (short int j = 0; j < col; ++j)
		{
			result[i][j]=mat[i][j];
		}
	}

	return result;

}

void deleteIntMatrix3D(short int*** mat, int row, int col){
	/*
	** mat: matriz a liberar memoria
	** row: número de filas de la matriz
	** col: número de columnas de la matriz
	** Nota: esta función libera memoria del puntero matriz 3d
	*/
	for (short int i = 0; i < row; ++i)
	{
		for (short int j = 0; j < col; ++j)
		{
			delete[]mat[i][j];
			mat[i][j]=nullptr;
		}

		delete[]mat[i];
		mat[i]=nullptr;
	}

	delete[]mat;
	mat=nullptr;

}

void resizeExternalCoor(int &rx, int &ry,int &r,int imgCol,int outcol, int imgRow,int outrow,int* pcoor){
	/*
	** rx, ry, r : Variables donde se va a almacenar los resultados
	** imgCol: Valor del número de columnas de la imagen original
	** imgRow: valor del numero de filas de la imagen oru¿iginal
	** outcol: valor del número de columnas de la imagen redimensionada
	** outrow: valor del número de filas de la imagen redimensionada
	** pcoor: vector que contien las coodenadas x, y, r.
	** Nota: Esta función cálcula los valores x,y,r a la escala original.
	*/
	double auxcol=(1+(double)(imgCol- outcol)/outcol);
	double auxrow=1+(double)(imgRow - outrow)/outrow;
	rx=pcoor[0]*auxcol; //se incremento 3px
	ry=pcoor[1]*auxrow;
	r=(double)auxcol*pcoor[2];
}

void resizeInternalCoor(int &rx2, int &ry2,int &r2,int imgCol,int outcol, int imgRow,int outrow,int* pcoor2,int columna, int fila, int radio){
	/*
	** rx2, ry2, r2 : Variables donde se va a almacenar los resultados
	** imgCol: Valor del número de columnas de la imagen original
	** imgRow: valor del numero de filas de la imagen oru¿iginal
	** outcol: valor del número de columnas de la imagen redimensionada
	** outrow: valor del número de filas de la imagen redimensionada
	** pcoor2: vector que contien las coodenadas x, y, r.
	** Nota: Esta función cálcula los valores x,y,r a la escala original.
	*/
	double auxcol=(1+(double)(imgCol- outcol)/outcol);
	double auxrow=1+(double)(imgRow - outrow)/outrow;
	r2=(double)auxcol*(pcoor2[2])+2;
	rx2=((columna-radio)+pcoor2[0])*auxcol;
	ry2=((fila-radio)+pcoor2[1])*auxrow;
}

void scaling(int &row, int &col, int maxscal){
	/*
	** row: Número de filas de la imagen original
	** col: Número de columnas de la imagen original
	** maxscal: La escala maxima de la imagen
	** Nota: Esta función cálcula los valores de las filas y columnas a las cuales se va a redimensionar
	**		 la imagen.
	*/
	float p=0.0;
	if (row>col)
	{
		p=(100*maxscal)/row;
		row=maxscal;
		col=round(col*(p/100));
	}else if (col>row)
	{
		p=(100*maxscal)/col;
		col=maxscal;
		row=round(row*(p/100));
	}else if (col==row)
	{
		row=maxscal;
		col=maxscal;
	}
}

/* Funciones temporales solo con funes de ver los resultados*/

void printIntMatrix(IntMatrix mat, int row, int col){
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			printf("%d,\t",mat[i][j] );
		}
		printf("\n");
	}


}

void printIntVector(IntVector vect, int length){
	for (int i = 0; i < length; ++i)
	{
		printf("%d\n", vect[i]);
	}

}

void WritwResult(IntMatrix mat,int row, int col){
	std::ofstream fichero("test.csv");
	     for (int i = 0; i < row; ++i)
	     {
	     	for (int j = 0; j < col; ++j)
	     	{
	     		fichero << mat[i][j] <<",";
	     	}
	     	fichero << std::endl;
	     }
		 
		//printf("Impresion finalizado ......");
        fichero.close();
}


void WritwResult2(double **mat,int row, int col){
	std::ofstream fichero("test.csv");
	     for (int i = 0; i < row; ++i)
	     {
	     	for (int j = 0; j < col; ++j)
	     	{
	     		fichero << mat[i][j] <<",";
	     	}
	     	fichero << std::endl;
	     }
		 
	   //	printf("Impresion finalizado ......");
        fichero.close();
}

void WritwResultV(int *mat,int row){
	std::ofstream fichero("test.csv");
	     for (int i = 0; i < row; ++i)
	     {
	     
	     		fichero << mat[i] <<"\n";
	     	
	     	
	     }
	     fichero << std::endl;
		 
		printf("Impresion finalizado ......");
        fichero.close();
}


void paintCircle(Matrix &img,int row, int col, int x, int y,int x2, int y2, int r1, int r2){
	for (int i = 1; i < 361; ++i)
	{
		int xdash1=round(r1*cos(i));
		int ydash1=round(r1*sin(i));


		if (((x+xdash1)<col)&&((x+xdash1)>0)&&((y+ydash1)<row)&&((y+ydash1)>0)){
			img[y+ydash1][x+xdash1]=double(100);
		}


	}

}

void binary(Matrix img, int row, int col,Matrix &imgOriginal,int x, int y,int y2, int r1, int r2){
  IntMatrix result=newIntMatrix(row,col,1);
  int fila=(y-r1);
  int max_row=(y2-r2);
  int columna=x-r1;
/*  double m1=imgOriginal[fila+int(row/2)][columna+int(col/2)];
  double m2=imgOriginal[fila+int(row/2)][columna+int(col/3)];
  double m3=imgOriginal[fila+int(row/2)][columna+int(col/4)];
  double m4=imgOriginal[fila+int(row/2)][columna+int(col/5)];
  double m=(m1+m2+m3+m4)/4;*/
   double m=0;
  for (int i = columna; i <columna+(r1-r2) ; ++i)
  {
  	m=m+imgOriginal[fila+int(row/2)][i];
  }
  m=m/(r1-r2);
  
  int max_y=0;

  for (int i = 0; i < row; ++i)
  {
  	for (int j = 0; j < col; ++j)
  	{
  		//result[i][j]=(img[i][j]<127)?0:1;
  		if (img[i][j]>round(85))
  		{
  			if (max_y<i)
  			{
  				max_y=i;
  			}

  			//imgOriginal[fila+i][columna+j]=0;
  		}
  	}
  }

 for (int i = 0; i < max_y; ++i)
  {
  	for (int j = 0; j < col; ++j)
  	{
  		//result[i][j]=(img[i][j]<127)?0:1;
  		if(max_row>(fila+i)){
  			imgOriginal[fila+i][columna+j]=0;
  		}
  	}
  }
  
}

void binary2(Matrix img, int row, int col,Matrix &imgOriginal,int x, int y, int r1, int r2){
  IntMatrix result=newIntMatrix(row,col,1);
  int fila=(y+r2);
  int columna=x-r1;
  int max_y=0;
  for (int i = row-1; i>=0; --i)
  {
  	for (int j = col-1; j>= 0; --j)
  	{
  		if (img[i][j]>round(85))
  		{
  			if (max_y<i)
  			{
  				max_y=i;
  			}
  		}
  	}
  }

 for (int i =max_y; i < row; ++i)
  {
  	for (int j = 0; j < col; ++j)
  	{
  		imgOriginal[fila+i][columna+j]=0;
  		
  	}
  }
  
}


int** double2Int(Matrix img, int row, int col){
	IntMatrix mat=newIntMatrix(row,col);
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			mat[i][j]=255-int(img[i][j]);
		}
	}
	return mat;
}