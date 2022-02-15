// #include "int_functions.h"
// #include "double_functions.h"
#include <math.h>

/*
** IntMatriz: Matriz apuntador de enteros
** Matrix: Matriz apuntador de tipo double
*/

double **interp2(IntMatrix x, IntMatrix y, Matrix img, 
	int sizeImgRow,int sizeImgCol, Matrix xo, Matrix yo, int sizeXOrow, int sizeXOcol){
	/*
	** x: matriz tipo int
	** y: matriz tipo int
	** img: matriz tipo double
	** xo: matriz tipo double
	** yo: matriz tiṕo double 
	*/
	
	int zr=sizeImgRow,zc=sizeImgCol;
	IntVector X=newIntVector(sizeImgCol);
	IntVector Y=newIntVector(sizeImgRow);
	int **a,**b,**c,**d;//dx,dy;
	IntMatrix xidx, yidx,xidx2, yidx2;
	for (int i = 0; i < sizeImgCol; ++i)
	{
		X[i]=x[0][i];
	}
    

	for (int i = 0; i < sizeImgRow; ++i)
	{
		Y[i]=y[i][0];
	}

    //pueda que sirva
    
 //    dx=Intdiff(X,sizeImgCol);
	// dy=Intdiff(Y,sizeImgRow);


	//xidx=doubleMatrix(631,720);
	xidx=lookup(X,xo,sizeXOrow,sizeXOcol,sizeImgCol);
	yidx=lookup(Y,yo,sizeXOrow,sizeXOcol,sizeImgRow);

	//duplicar
	xidx2=lookup(X,xo,sizeXOrow,sizeXOcol,sizeImgCol);
	yidx2=lookup(Y,yo,sizeXOrow,sizeXOcol,sizeImgRow);
	//____//



	a=fillIntMatriz(zr-1,zc-1,img);
	b=fillIntMatriz(zr-1,zc-1,img,0,1);
	for (int i = 0; i < zr-1; ++i)
	{
		for (int j =0; j < zc-1; j++)
		{
			b[i][j]=b[i][j]-a[i][j];
			if(b[i][j]<0){
				b[i][j]=0;
			}
		}
	}

	c=fillIntMatriz(zr-1,zc-1,img,1);
   for (int i = 0; i < zr-1; ++i)
	{
		for (int j =0; j < zc-1; j++)
		{
			c[i][j]=c[i][j]-a[i][j];
			if (c[i][j]<0)
			{
				c[i][j]=0;
			}
		}
	}

    d=fillIntMatriz(zr-1,zc-1,img,1,1);
    for (int i = 0; i < zr-1; ++i)
	{
		for (int j =0; j < zc-1; j++)
		{
			d[i][j]=d[i][j]-a[i][j]-b[i][j]-c[i][j];
			if (d[i][j]<0)
			{
				d[i][j]=0;
			}
		}
	}
	   
   IntMatrix aux=cpIntMatrix(xidx,sizeXOrow,sizeXOcol);
   IntMatrix auxy=cpIntMatrix(yidx,sizeXOrow,sizeXOcol);

   Matrix Xsc=newDoubleMatrix(sizeXOrow,sizeXOcol);
   Intdiff2(X,sizeImgCol,xidx,sizeXOrow,sizeXOcol);
   Intdiff2(Y,sizeImgRow,yidx,sizeXOrow,sizeXOcol);
  
   valInd(X,sizeImgCol,aux,sizeXOrow,sizeXOcol);
  		
	for (int i = 0; i < sizeXOrow; ++i)
	{
		for (int j = 0; j < sizeXOcol; ++j)
		{
			Xsc[i][j]=(xo[i][j]-aux[i][j])/xidx[i][j];
		}
	}

  valInd(Y,sizeImgRow,auxy,sizeXOrow,sizeXOcol);

 Matrix Ysc=newDoubleMatrix(sizeXOrow,sizeXOcol);

	for (int i = 0; i < sizeXOrow; ++i)
	{
		for (int j = 0;  j< sizeXOcol; ++j)
		{
			Ysc[i][j]=(yo[i][j]-auxy[i][j])/yidx[i][j];
		}
	}

	  
	IntVector x2=mat2vect(xidx2,sizeXOrow,sizeXOcol);
	IntVector y2=mat2vect(yidx2,sizeXOrow,sizeXOcol);
	IntVector subind=sub2ind(zr-1,zc-1,y2,x2,sizeXOrow*sizeXOcol);

	IntMatrix idx=vect2mat(subind,sizeXOrow*sizeXOcol,sizeXOrow);
	
   //{{{}}}
   IntMatrix forientA=sub2ind2D(a,sizeImgRow-1,sizeImgCol-1,idx,sizeXOrow,sizeXOcol);
   IntMatrix forientB=sub2ind2D(b,sizeImgRow-1,sizeImgCol-1,idx,sizeXOrow,sizeXOcol);
   IntMatrix forientC=sub2ind2D(c,sizeImgRow-1,sizeImgCol-1,idx,sizeXOrow,sizeXOcol);
   IntMatrix forientD=sub2ind2D(d,sizeImgRow-1,sizeImgCol-1,idx,sizeXOrow,sizeXOcol);
   
   //printf("A %d B %d  C %d  D%d  Xsc %lf Ysc%lf\n",forientA[0][0],forientB[0][0],forientC[0][0],forientD[0][0],Xsc[0][0],Ysc[0][0]);
   Matrix ZI=newDoubleMatrix(sizeXOrow,sizeXOcol);
   for (int i = 0; i < sizeXOrow; ++i)
   {
   	for (int j = 0; j < sizeXOcol; ++j)
   	{
   		ZI[i][j]=round(forientA[i][j]+
   				(forientB[i][j]*Xsc[i][j])+
   				(forientC[i][j]*Ysc[i][j])+
   				(forientD[i][j]*Xsc[i][j]*Ysc[i][j]));
   		

   		if (X[0]<X[sizeImgCol-1])
		{
			if (Y[0]<Y[sizeImgRow-1])
			{

				if ((ZI[i][j]<X[0])||(xo[i][j]>X[sizeImgCol-1])|| (yo[i][j]<Y[0])||(yo[i][j]>Y[sizeImgRow-1]))
				{
					ZI[i][j]=0;
				}
			
			}
		}
   	}
  }

  // printf("%lf\n",sum/(sizeXOrow*sizeXOcol));
  // WritwResult2(ZI,sizeXOrow,sizeXOcol);
  // WritwResult2(ZI,sizeXOrow,sizeXOcol);
 
 /*
 ** Liberar memoria punteros.
 */
  	deleteDoubleMatrix(img,sizeImgRow);//debug

  deleteIntVector(X);
  deleteIntVector(Y);
  deleteIntMatrix(a,zr-1);
  deleteIntMatrix(b,zr-1);
  deleteIntMatrix(c,zr-1);
  deleteIntMatrix(d,zr-1);
  deleteIntMatrix(xidx,sizeXOrow);
  deleteIntMatrix(yidx,sizeXOrow);
  deleteIntMatrix(xidx2,sizeXOrow);
  deleteIntMatrix(yidx2,sizeXOrow);
  deleteIntMatrix(aux,sizeXOrow);
  deleteIntMatrix(auxy,sizeXOrow);
  deleteDoubleMatrix(Xsc,sizeXOrow);
  deleteDoubleMatrix(Ysc,sizeXOrow);
  deleteIntVector(x2);
  deleteIntVector(y2);
  deleteIntVector(subind);
  deleteIntMatrix(idx,sizeXOrow);
  deleteIntMatrix(forientA,sizeXOrow);
  deleteIntMatrix(forientB,sizeXOrow);
  deleteIntMatrix(forientC,sizeXOrow);
  deleteIntMatrix(forientD,sizeXOrow);

 return ZI;

}