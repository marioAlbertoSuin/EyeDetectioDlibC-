int pointDist(int x1,int y1, int x2, int y2){ 
return round(sqrt(pow(x2-x1,2)+pow(y2-y1,2))); 
} 
void correctSegmentation(int x1, int y1, int &x2, int &y2, int r){ 
if (pointDist(x1,y1,x2,y2)>round(r/2))
 { x2=x1; y2=y1; } } 