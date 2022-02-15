 double convulse(Matrix img, Matrix gaussian,int i,int j,int length){
	double sum=0.0;
	int rowG=0,colG=0;
		for (int row = i; row < length+i; row++)
	{
		for (int col = j; col < length+j; col++)
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