/*
** Universidad Politécnica Salesiana
** Ingeniería de Sistemas
** Autores: Farinango Abel
**			Sandoval Leonel
** Última modificación 2021
*/

#include <vector>
#include <fstream>
#include <iostream>
#include "stb_image.h"
extern "C" {
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
}
typedef double **Matrix;
typedef double *Vector;

void deleteDoubleMatrix(Matrix mat, int row){
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
void deleteDoubleVector(Vector vect){
	/* 
	** vect: vector puntero de tipo double
	** Esta función libera memoria del apuntador tipo vector
	*/
	delete[] vect;
	vect=nullptr;
}

Vector newDoubleVector(int length,double value=0){
	/*
	** length: tamaño del vector a crear
	** value: valor con el que se inicializa el vector (opcional, defarult 0)
	** Nota: Esta función crea un vector apuntador de tipo double
	*/
	Vector vect=new double[length];
	for (int i = 0; i < length; ++i)
	{
		vect[i]=value;
	}

	return vect;
}

Vector fillDoubleVector(int start, int end){
	/*
	** start: valor inicial
	** end: valor final
	** Nota: esta función retorna un vector double llenado con los valores dentro de rango start - end
	*/
	int length=end-start;
	double cont=start;
	Vector vect=newDoubleVector(length+1);
	for (int i = 0; i <=length; ++i)
	{
		vect[i]=cont;
		cont++;
	}
	return vect;
}

Matrix newDoubleMatrix(int row, int col, double value=0){
	/*
	** row: número de filas de la matriz
	** col: número de columnas de la matriz
	** value (opcional): valor con la que se inicializa, por defecto 0
	** Nota: Esta funcion devuelve 
	*/
	Matrix vect=new double*[row];
	for (short int i = 0; i < row; ++i)
	{
		vect[i]=new double[col];
		for (short int j= 0; j < col; ++j)
		{
			vect[i][j]=value;
		}
	}

return vect;

}
 
double convulse(Matrix img, Matrix gaussian,int i,int j,int length){
	/*
	** img: Matriz con los datos de la imagen
	** gaussian: Matriz con el kernel gaussiano
	** i: Indice de donde va ha recorrer la matriz de imagen(fila)
	** j: Indice de donde va ha recorrer la matriz de imagen(columnas)
	** length: tamaño del kernel.
	** Nota: Esta función devuelve el valor de la convulcion.
	*/
	double sum=0.0;
	int rowG=0,colG=0;
		for (short int row = i; row < length+i; ++row)
	{
		for (short int col = j; col < length+j; ++col)
		{
			sum=sum+(img[row][col]*gaussian[rowG][colG]);	
			colG++;

		}
		colG=0;
		rowG++;
	}
	
	return sum;    
}

Matrix filter2(Matrix img, Matrix gauss, int imgRows,int imgCols,int length){
     /*
     ** img: Matriz de imagen al que se le va a aplicar el filtro
     ** gauss: Matriz filtro (gaussian) "KERNEL"
     ** imgRows: Tamaño de filas de la matriz de imagen(img)
     ** imgCols: Tamaño de columnas de la matriz de iamgen (img)
     ** length: tamaño de la matriz gauss(kernel)
     ** Nota: Esta función devuelve la matriz de imagen aplicada el filtro gaussiano
     */  
	int padding=(length/2)*2;
	int index=length/2;
	Matrix aux=newDoubleMatrix(imgRows+padding,imgCols+padding);
	Matrix result=newDoubleMatrix(imgRows,imgCols);
         
		 for (int i = index; i < imgRows+index; ++i)
		 {
		 	for (int j = index; j < imgCols+index; ++j)
		 	{ 	
		 		aux[i][j]=img[i-index][j-index];
			
		 	}
		
		 }

	      for (int i = 0; i <=(imgRows+padding)-length; ++i)
	      {
	      	for (int j = 0; j <=(imgCols+padding)-length; ++j)
	      	{
	    		
	     		 result[i][j]=convulse(aux,gauss,i,j,length);
	     	}
	    	
	     }
	     deleteDoubleMatrix(aux,imgRows+padding);

	  return result ;

}

Matrix transpose(Matrix mat, int sizeRow, int sizeCol){
	/*
	** mat: matriz de datos a transponerse
	** siseRow: número de filas de la matriz(mat)
	** sizeCol: número de columnas de la matriz(mat)
	** Nota: esta función devuelve una matriz transpuesta
	*/
	Matrix trans=newDoubleMatrix(sizeCol, sizeRow);
	for (short int i = 0; i < sizeRow; ++i)
	{
		for (short int j = 0; j < sizeCol; ++j)
		{
			trans[j][i]=mat[i][j];
		}
	}
return trans;
}

Vector diff(Vector vect, int length){
	/*
	** vect: Vector que contiene los datos
	** length: tamaño del vector
	** Nota: esta función retorna un vector(double) con los valores de la diferencia 
	**       entre los valores del vector original
	*/
	Vector  result=newDoubleVector(length);
	double aux=0.0;
	for (int i = 0; i <length-1; ++i)
	{
		aux=(vect[i+1]-vect[i]);
		result[i]=aux;
	}

	return result;
}

Matrix getDoubleRows(Matrix img, int i, int sizeRow){
	/*
	** img: matriz de datos de las que se va a extraer la fila
	** sizerRow: el número de elementos que tiene la fila.
	** Nota: esta función retorna Matriz de 2D con todos los elementos de la fila especificada,
	**        agregando una columna mas de 0.
	*/
	Matrix rows=newDoubleMatrix(sizeRow,2);
	for (int j = 0; j < sizeRow; ++j)
	{
		rows[j][0]=img[i][j];
	}
	return rows;
}

Matrix loadImage(char const *nombre_img, int &sizeImgRow,int &sizeImgCol){
	/*
	** nombre_img: La ruta de la imgen a leer, incluido el nombre
	** sizeImgRow: variable que va a almacenar el numero de filas
	** sizeImgCol: variable que va a almacenar el número de columnas
	** Nota: Esta función retorna una matriz double con los datos de la imagen.
	*/
	int n;
	unsigned char *data = stbi_load(nombre_img, &sizeImgCol, &sizeImgRow, &n, 1);
	Matrix img=newDoubleMatrix(sizeImgRow,sizeImgCol);
	int cont=0;

	for (int i=0;i<sizeImgRow;i++){
	for (int j=0;j<sizeImgCol;j++){ 
	*(*(img+i)+j)=double(*(data+cont));
	cont++;
	}
	}

	stbi_image_free(data);

	return img;

}

void loadImage2(char const *nombre_img, int &sizeImgRow,int &sizeImgCol,float *&img, Matrix &img2){
	/*
	** nombre_img: La ruta de la imgen a leer, incluido el nombre
	** sizeImgRow: variable que va a almacenar el numero de filas
	** sizeImgCol: variable que va a almacenar el número de columnas
	** img: Vector de tipo float, donde se va a carga la imagen
	** img2: Matriz de tipo double donde se va a cargar la imagen.
	** Nota: Esta función retorna una matriz double con los datos de la imagen, también, retorna
	**       un vector de tipo double con los datos de la imagen.
	*/
    int n;
    unsigned char *data = stbi_load(nombre_img, &sizeImgCol, &sizeImgRow, &n, 1);
    img=new float[sizeImgRow*sizeImgCol];
    img2=newDoubleMatrix(sizeImgRow,sizeImgCol);
    int row=0,col=0;

    for (int i=0;i<sizeImgRow*sizeImgCol;++i){
        img[i]=float(data[i]);
        if(col==sizeImgCol){
            col=0;
            row++;
        }
        img2[row][col]=double(data[i]);
        col++;
    }
    stbi_image_free(data);
    data=nullptr;
}

//--------------------------------------------------
Vector float2Double(float * vect,int length){
	/*
	** vect: vector de tipo float
	** length: tamaño del vector
	** Nota: esta función retorna un vector de tipo double.
	*/
	Vector result=newDoubleVector(length);
	for(int i=0;i<length;++i){
		result[i]=double(vect[i]);
	}
	delete[]vect; //debug
	vect=nullptr;

	return result;
}

Matrix vect2matN(Vector vect,int nRow ,int nCols){
	/*
	** vect: vector que contiene los datos
	** nRow: número de filas de la matriz
	** nCols: número de columnas de la matriz
	** Nota: esta función retorna una matriz de tipo double con los datos del vector de entrada.
	*/
	Matrix mat=newDoubleMatrix(nRow,nCols);
	int cont=0;
	for (short int i = 0; i < nRow; ++i)
	{
		for (short int j = 0; j < nCols; ++j)
		{
			mat[i][j]=vect[cont];
			cont++;
		}
	}
    deleteDoubleVector(vect);
	return mat;
}

Matrix imgResize(float *img, int row,int col,int outrow,int outcol){
	/*
	** img: vector con los datos de la imagen
	** row: número de filas de la imagen original
	** col: número de columnas de la imagen orifinal
	** outrow: número de filas de la imagen redimensionada
	** outcol: número de columnas de la imagen redimensionada
	** Nota: Esta función retorna una matriz de tipo double con la imagen redimensionada.
	*/
	float *result=new float[outrow*outcol];
	stbir_resize_float(img,col,row,0,result,outcol,outrow,0,1);
	Vector aux2=float2Double(result,outrow*outcol);
	Matrix resize=vect2matN(aux2,outrow,outcol);
	//deleteDoubleVector(aux2); debug

	


	return resize;

	/**/


}

Vector mat2vectD(Matrix mat, int sizerow, int sizecol ){
	/*
	** mat: Matriz 2-D que contiene datos que van a ser pasados a un vector 1-D
	** sizerow: Número de filas de la matriz
	** sizecol: Número de columnas de la matriz
	** NOTA: Esta función retorna un vector con todos los datos de la matriz (Mathlab: matriz(:))
	*/
	int length=sizecol*sizerow;
	Vector vect=newDoubleVector(length);
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

void writeCsvEncode( Matrix img, int row, int col,int label=0){
	/*
	** img: Matriz con los datos de la imagen
	** row: número de filas de la imagen
	** col: número de columnas de la imagen
	** label: etiqueta(nombre) con la se va a crear el archivo csv
	** Esta función escribe un archivo csv en la memoria interna del dispositivo
	*/

	std::string path="";
	if (label!=0){
		path="feature.txt";
	}else{
		path="test.txt";
	}
	std::ofstream fichero;
	if(label==0){
		fichero.open(path);
	}else{
		fichero.open(path,std::ios_base::app);
	}

	Vector vectImg=mat2vectD(img,row,col);
	int cont=1;
    fichero<<std::to_string(label)<<"\t";
	for (int i=0;i<row*col;++i){
			fichero << cont<<":"<<vectImg[i] <<"\t";
			cont++;
	}
	fichero<<std::endl;
	fichero.close();
	deleteDoubleVector(vectImg); //debug
	deleteDoubleMatrix(img,row);

}

Matrix eyelashes(Matrix img,int x,int y, int r1, int r2, int side,int &outrow,int &outcol){
	//comentar..
	outrow=r1-r2;
	outcol=r1*2;
    Matrix subimg=newDoubleMatrix(r1-r2,r1*2);
    int cont1=0,cont2=0;
    int i=(side==0)?(y-r1):(y+r2);
    int __imax=(side==0)?((y-r1)+(r1-r2)):(y+r1);
    for ( i; i < __imax; ++i)
        {
            for (int j = x-r1; j < x+r1; ++j)
            {
                subimg[cont1][cont2]=img[i][j];
                cont2++;
            }
            cont2=0;
            cont1++;
        }
   return subimg;
}

void writeData(const char *name,double data, int op){
	std::string path="metrics.txt";
	std::ofstream fichero;
	fichero.open(path,std::ios_base::app);
	if (op==0)
	{
		fichero<<name;
	}
	else if (op==1)
	{
		fichero<<"\n"<<name<<",";
	}
	else if(op==2 && data!=0.0){
    // printf("op 2\n");
	fichero <<data <<",";
	}
	//fichero<<std::endl;
	fichero.close();

}
// Funciones temporales solo para test 
void printMatrix(Matrix mat, int row, int col){
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			printf("%f ", mat[i][j]);
		}
		printf("\n");
	}
	deleteDoubleMatrix(mat,row);
}

void printVector(Vector v,int length){
	for (int i = 0; i < length; ++i)
	{
		printf("%lf\n",v[i] );
	}

}
char  *int2char(int **data, int row, int col){
    char *img=new char[row*col];
    int cont=0;
    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < col; ++j)
        {
            img[cont]=char(data[i][j]*255);
            cont++;
        }
        
    }
    return img;

}





