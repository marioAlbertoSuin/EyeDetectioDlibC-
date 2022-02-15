
class PrintImg
{
private:
  int **img;
  int row;
  int col;
  const char*name;
  char *imgc;
public:
	void PrintImgs(IntMatrix img,int row,int col,const char *name){
		this->name=name;
		this->row=row;
		this->col=col;
		this->img=cpIntMatrix(img, row, col);
		this->writeImageJPG(this->img,this->row,this->col,this->name);
		this->freePointers();

	}
	void PrintImgs(Matrix img, int row, int col,const char *name){
		this->name=name;
		this->row=row;
		this->col=col;
		this->img=double2Int(img,row,col);
		this->writeImageJPG(this->img,this->row,this->col,this->name);
		this->freePointers();
	}
	void freePointers(){
		deleteIntMatrix(this->img,this->row);
		this->img=nullptr;
		/*delete[]this->name;
		this->name=nullptr;*/
		delete[]this->imgc;
		this->imgc=nullptr;
 
	}


	void writeImageJPG(int **img, int row, int col, const char* name){
    this->imgc=int2char(img,row,col);
    stbi_write_jpg(name,col,row,1,this->imgc,100);
	}
	
};