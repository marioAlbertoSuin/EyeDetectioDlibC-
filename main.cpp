#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <iostream>
#include <fstream>
#include <dlib/image_transforms.h>
#include <dlib/image_transforms/interpolation.h>
#include <stdio.h>
#include <cstdio>
#include <vector>
#include <cmath>
#include <stdlib.h>
#include <omp.h>
#include "includes.h"
#include "system_metrics.h"
#include "memcount.h"
#include "hybridmedianfilter.h"
unsigned long tid;

// -----------------------------------------------------------------------------------------
using namespace dlib;
using namespace std;
int tam_ventana = 3;

PrintImg print;


// ----------------------------------------------------------------------------------------
// ESCRIBIR METRICAS CSV
void writePerfMetricsFilter(ofstream &file, string filename, double cpu, int mem, double time, double ms, double pr, int ventana)
{
    file << filename << "," << cpu << "," << mem << "," << time << "," << ms << "," << pr << "," << ventana << endl;
}

void writePerfMetricsShape(ofstream &file, string filename, double cpu, int mem, double time)
{
    file  << filename << "," << cpu << "," << mem << "," << time << endl;
}

//-----------------------------------------------------------------------------------------
// OBTENER NOMBRE IMAGENES
string tokenize(string q)
{

    std::string token1 = q.erase(0, q.find("/") + 1);
    std::string token2 = token1.substr(0, token1.find("."));
    return token2;
}


//-------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------
// EXTRACCION CORDENADAS DE OJOS
inline std::vector<image_window::overlay_line> Extraccion_puntos_ojos(
    const std::vector<full_object_detection> &dets,
    const std::string &path,
    const rgb_pixel color = rgb_pixel(17, 255, 0))
{
    
    std::vector<image_window::overlay_line> lines;

    for (unsigned long i = 0; i < dets.size(); ++i)
    {
        DLIB_CASSERT(dets[i].num_parts() == 68 || dets[i].num_parts() == 5,
                     "\t std::vector<image_window::overlay_line> render_face_detections()"
                         << "\n\t You have to give either a 5 point or 68 point face landmarking output to this function. "
                         << "\n\t dets[" << i << "].num_parts():  " << dets[i].num_parts());

        const full_object_detection &d = dets[i];

  

        for (unsigned long i = 37; i <= 41; ++i)
        {

            lines.push_back(image_window::overlay_line(d.part(i), d.part(i - 1), color));
        }

        

        lines.push_back(image_window::overlay_line(d.part(36), d.part(41), color));

        // Right eye
 

        for (unsigned long i = 43; i <= 47; ++i)
        {
            lines.push_back(image_window::overlay_line(d.part(i), d.part(i - 1), color));
        }

     
        lines.push_back(image_window::overlay_line(d.part(42), d.part(47), color));
      
    }

    return lines;
}
// -----------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    systemMetrics performance("perf");
    ofstream metrics;

    try
    {

        // This example takes in a shape model file and then a list of images to
        // process.  We will take these filenames in as command line arguments.
        // Dlib comes with example images in the examples/faces folder so give
        // those as arguments to this program.
        if (argc == 1)
        {
            cout << "Call this program like this:" << endl;
            cout << "./face_landmark_detection_ex shape_predictor_68_face_landmarks.dat faces/*.jpg" << endl;
            cout << "\nYou can get the shape_predictor_68_face_landmarks.dat file from:\n";
            cout << "http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
            return 0;
        }

        // We need a face detector.  We will use this to get bounding boxes for
        // each face in an image.
        frontal_face_detector detector = get_frontal_face_detector();
        // And we also need a shape_predictor.  This is the tool that will predict face
        // landmark positions given an image and face bounding box.  Here we are just
        // loading the model from the shape_predictor_68_face_landmarks.dat file you gave
        // as a command line argument.
        shape_predictor sp;
        deserialize(argv[1]) >> sp;

        image_window win;
        // Loop over all the images provided on the command line.

        for (int i = 2; i < argc; ++i)
        {

            if (true)
            {
                metrics.open("metricsShapePredictor.csv", ios_base::app); // Append
            }
            else
            {
                metrics.open("metricsShapePredictor.csv"); // Append
            }
            performance.resetCounters();
            cout << "processing image " << argv[i] << endl;
            array2d<rgb_pixel> img;
            load_image(img, argv[i]);
            // Make the image larger so we can detect small faces.
            // pyramid_up(img);

            // Now tell the face detector to give us a list of bounding boxes
            // around all the faces in the image.
            std::vector<rectangle> dets = detector(img);
            cout << "Number of faces detected: " << dets.size() << endl;

            if (dets.size() == 0)
            {
                continue;
            }
            else
            {

                int pt37[2], pt40[2],y12[2];

                // Now we will go ask the shape_predictor to tell us the pose of
                // each face we detected.
                std::vector<full_object_detection> shapes;
                for (unsigned long j = 0; j < dets.size(); ++j)
                {
                    full_object_detection shape = sp(img, dets[j]);
                    cout << "number of parts: " << shape.num_parts() << endl;
                    cout << "pixel 37:  " << shape.part(37) << endl;
                    cout << "pixel 40:  " << shape.part(40) << endl;
                    cout << "pixel 38:  " << shape.part(38) << endl;
                    cout << "pixel 42: " << shape.part(42) << endl;
                    // You get the idea, you can get all the face part locations if
                    // you want them.  Here we just store them in shapes so we can
                    // put them on the screen.
                    shapes.push_back(shape);
                    pt37[0] = shape.part(37).x()-(shape.part(37).x()/5);
                    pt37[1] = shape.part(37).y();
                    pt40[0] = shape.part(40).x()+(shape.part(40).x()/5);
                    pt40[1] = shape.part(40).y();
                    y12[0] = shape.part(38).y()-(shape.part(38).y()/8);
                    y12[1] = shape.part(42).y()+(shape.part(42).y()/8);
                }

                // ojos

                win.clear_overlay();
                win.set_image(img);
                win.add_overlay(Extraccion_puntos_ojos(shapes, argv[i]));

                int imgRow, imgCol,imgrowE,imgecolE;
                float *auximg;
                Matrix imgface;

                imgecolE = (pt40[0]-pt37[0]);
                imgrowE = (y12[1]-y12[0]);
     
                Matrix cropeye = newDoubleMatrix(imgrowE,imgecolE);
                Matrix cropeyefil = newDoubleMatrix(imgrowE,imgecolE);
                
                loadImage2(argv[i], imgRow, imgCol, auximg, imgface);
                
                for(int y=y12[0],i=0;y<y12[1];y++,i++){
                    for (int x=pt37[0],j=0;x<pt40[0];x++,j++){
                                            
                        cropeye[i][j]=imgface[y][x];
                    }
                   
                }

               
                string token = tokenize(argv[i]);
                string  namepath= "resul/"+token+".bmp";
                const char *nombre =namepath.c_str();
               
            
                print.PrintImgs(cropeye, imgrowE, imgecolE, nombre);

                double *imgvect = new double[imgecolE*imgrowE];
                double *imgout = new double[imgecolE*imgrowE]; 
                int cont=0;

                for(int y=0;y<imgrowE;y++){
                    for (int x=0;x<imgecolE;x++){
                           imgvect[cont]=cropeye[y][x];
                           cont=cont+1;                 
                        
                    }
                   
                }

        performance.calculate();
        double cpu = performance.getCpuPercent();
        int mem = getRamUsage();
        double totalSeconds = performance.getDurationInMiliseconds();
        writePerfMetricsShape(metrics, argv[i], cpu, mem, totalSeconds);
        metrics.close();

        cpu=0;
        mem=0;
        totalSeconds=0;
//----------------------------------------------------------------------------------------


        if (true)
            {
                metrics.open("metricsFilter.csv", ios_base::app); // Append
            }
            else
            {
                metrics.open("metricsFilter.csv"); // Append
            }
            performance.resetCounters();



//-----------------------------------------------------------------------------------------
// FILTRO HIBIRIDO MEDIA

                hybridmedianfilter(imgvect,imgout,imgrowE,imgecolE);

                cont=0;
                for(int y=0;y<imgrowE;y++){
                    for (int x=0;x<imgecolE;x++){
                           cropeyefil[y][x]=imgout[cont];
                           cont=cont+1;                 
                        
                    }
                   
                }

//-----------------------------------------------------------------------------------------
// MSE PSNR
                
                namepath= "resultado/"+token+".bmp";
                nombre =namepath.c_str();
                print.PrintImgs(cropeyefil,imgrowE,imgecolE,nombre);
                double summse=0,mse,psnr;
                for (int i=0;i<imgrowE;i++){
                    for(int j=0;j<imgecolE;j++){
                        summse=pow(cropeye[i][j]-cropeyefil[i][j],2)+summse;
                    }

                }
                mse = summse/(imgrowE*imgecolE);
                psnr = 10*log10(pow(255,2)/mse);

                cout <<"\n MSE: "<<mse<<"\n PSNR: "<<psnr<<"\n";


                delete[] auximg;
                auximg = nullptr;
                deleteDoubleMatrix(imgface, imgRow);
                deleteDoubleMatrix(cropeye, imgrowE);
                deleteDoubleMatrix(cropeyefil,imgrowE);

                performance.calculate();
                cpu = performance.getCpuPercent();
                mem = getRamUsage();
                totalSeconds = performance.getDurationInMiliseconds();
                writePerfMetricsFilter(metrics, argv[i], cpu, mem, totalSeconds,mse,psnr,3);
                metrics.close();
                

            }

            cout << "Hit enter to process the next image..." << endl;
            // cin.get();
        }
        
        int id_user = 0;
        DIR *dir;
        struct dirent *diread;
        const char *path = "/home/marcelo/Documentos/tesis/EyeDetectioDlibC-/build/resultado/";

        if ((dir = opendir(path)) != nullptr)
        {
            while ((diread = readdir(dir)) != nullptr)
            {

                std::string name_file = diread->d_name;
                if (name_file != "." && name_file != "..")
                {
                    std::string val = path + name_file;
                    int imgRow, imgCol;
                    float *auximg;
                    Matrix img3;

                    std::cout << name_file << std::endl;
                    loadImage2(val.c_str(), imgRow, imgCol, auximg, img3);
                    id_user = nombre_ubiris(name_file.c_str());

                    int outrow = imgRow, outcol = imgCol;
                    scaling(outrow, outcol, 400);
                    Matrix img = imgResize(auximg, imgRow, imgCol, outrow, outcol);
                    Matrix mat_gradient = newDoubleMatrix(outrow, outcol);
                    Matrix mat_or = newDoubleMatrix(outrow, outcol);

 
                    double g = 2.2;
                    double borde = 1.5;
                    double histmax = 0.34;
                    double histmin = 0.27;

                    canny(img, mat_gradient, mat_or, outrow, outcol, 1, 1, 1);
                    adjgamma(mat_gradient, outrow, outcol, g);
                    Matrix i4 = nonmaxsup(mat_gradient, outrow, outcol, mat_or, outrow, outcol, borde);
                    IntMatrix final = hysthresh(i4, outrow, outcol, histmax, histmin);

                    // ubiris sesion1 y session2 rmin 88 rmax 105
                    // casia lamp rmin 55  rmax 77
                    // personal rmin 15 rmax 45
                    int r_min = 88;
                    int r_max = 105;

                    IntVector pcoor = detectar_circulo(final, outrow, outcol, r_min, r_max, 0.1);

                    /* escalar el radio*/
                    // int raux=pcoor[2];
                    int rx, ry, r;
                    resizeExternalCoor(rx, ry, r, imgCol, outcol, imgRow, outrow, pcoor);

                    //---------------
                    deleteDoubleMatrix(mat_gradient, outrow);
                    deleteDoubleMatrix(mat_or, outrow);
                    deleteDoubleMatrix(i4, outrow);
                    deleteIntMatrix(final, outrow);

                    int fila = pcoor[1];    // x
                    int columna = pcoor[0]; // y
                    int radio = pcoor[2];   // r
                    int fil_col = radio * 2;
                    scaling(outrow, outcol, 400);
                    Matrix img2 = imgResize(auximg, imgRow, imgCol, outrow, outcol);
                    Matrix mat_ojo = newDoubleMatrix(fil_col, fil_col);
                    int conti = 0;
                    int contj = 0;
                    for (int i = fila - radio; i < fila + radio; ++i)
                    {
                        for (int j = columna - radio; j < columna + radio; ++j)
                        {
                            mat_ojo[conti][contj] = img2[i][j];
                            contj++;
                        }
                        contj = 0;
                        conti++;
                    }

                    deleteDoubleMatrix(img2, outrow);

                    Matrix mat_gradient2 = newDoubleMatrix(fil_col, fil_col);
                    Matrix mat_or2 = newDoubleMatrix(fil_col, fil_col);

                    //--ubiris session1 y session2 3.4, 1.5 0.34,0.27
                    // casia lamp 2.2,1.5,0.34,0.27
                    // prsonal ojo 3.5,1.5 , 0.34,0.27
                    double g2 = 3.4;
                    double borde2 = 1.5;
                    double histmax2 = 0.34;
                    double histmin2 = 0.27;
                    canny(mat_ojo, mat_gradient2, mat_or2, fil_col, fil_col, 1, 1, 1, 2);
                    adjgamma(mat_gradient2, fil_col, fil_col, g2);
                    Matrix i42 = nonmaxsup(mat_gradient2, fil_col, fil_col, mat_or2, fil_col, fil_col, borde2);
                    IntMatrix final2 = hysthresh(i42, fil_col, fil_col, histmax2, histmin2);

                    //----------------------
                    // ubiris session1 y sssion2 rmin=radio*0.18 rmax=radio*0.20
                    // casia lamp rmin=radio*0.18 rmax=radio*0.18
                    int r_min2 = round(radio * 0.18);
                    int r_max2 = radio - round(radio * 0.2);
                    IntVector pcoor2 = detectar_circulo(final2, fil_col, fil_col, r_min2, r_max2, 0.20, 1);

                    int rx2, ry2, r2;
                    resizeInternalCoor(rx2, ry2, r2, imgCol, outcol, imgRow, outrow, pcoor2, columna, fila, radio);

                    Matrix gausfilter = newDoubleMatrix(5, 5);
                    gausfilter[0][0] = 0.0121;
                    gausfilter[0][1] = 0.0261;
                    gausfilter[0][2] = 0.0337;
                    gausfilter[0][3] = 0.0261;
                    gausfilter[0][4] = 0.0121;
                    gausfilter[1][0] = 0.0261;
                    gausfilter[1][1] = 0.0561;
                    gausfilter[1][2] = 0.0724;
                    gausfilter[1][3] = 0.0561;
                    gausfilter[1][4] = 0.0261;
                    gausfilter[2][0] = 0.0337;
                    gausfilter[2][1] = 0.0724;
                    gausfilter[2][2] = 0.0935;
                    gausfilter[2][3] = 0.0724;
                    gausfilter[2][4] = 0.0337;
                    gausfilter[3][0] = 0.0261;
                    gausfilter[3][1] = 0.0561;
                    gausfilter[3][2] = 0.0724;
                    gausfilter[3][3] = 0.0561;
                    gausfilter[3][4] = 0.0261;
                    gausfilter[4][0] = 0.0121;
                    gausfilter[4][1] = 0.0261;
                    gausfilter[4][2] = 0.0337;
                    gausfilter[4][3] = 0.0261;
                    gausfilter[4][4] = 0.0121;
                    Matrix img4 = filter2(img3, gausfilter, imgRow, imgCol, 5);
                    deleteDoubleMatrix(gausfilter, 5);

                    correctSegmentation(rx, ry, rx2, ry2, r2);

                    paintCircle(img4, imgRow, imgCol, rx, ry, rx2, ry2, r, r2);
                    // print.PrintImgs(img4,imgRow,imgCol,name_file.c_str());

                    /*
                     int mayor = 120,acum=0,acum2=0,cont=0,cont2=0,promedio,max;

                     for (int i=0;i<imgRow;i++){
                         for(int j=0;j<imgCol;j++){

                              int d = (pow((rx-j),2))+(pow((ry-i),2));
                             int rad = pow(r,2);

                             if ( d < rad ){

                             acum=acum+img4[i][j];
                             cont=cont+1;
                             if(img4[i][j]>mayor){
                                 mayor=img4[i][j];
                                 cont2=cont2+1;
                                 acum2=acum2+mayor;
                             }}
                         }
                    }
                     max = acum2 / cont2;
                     promedio=acum/cont;
                     int uref=promedio+0.9*(max-promedio);



                     //cout <<"\n"<<xdash1<<"\t"<<ydash1<<"\n";

                     for (int i=ry-r;i<ry+r;i++){
                         for(int j=rx-r;j<rx+r;j++){

                             int d = (pow((rx-j),2))+(pow((ry-i),2));
                             int rad = pow(r,2);

                             if ( d < rad ){
                                 if(img4[i][j] > uref){
                                     //img4[i][j]=(img4[i-15][j+15]+img4[i][j+15]+img4[i+15][j+15]+img4[i+15][j]+img4[i+15][j-15]+img4[i][j-15]+img4[i-15][j-15]+img4[i-15][j])/8;
                                     img4[i][j]=255;
                                 }
                             }
                         }
                    }



                     //gausfilter[0][0]=0;gausfilter[0][1]=255;gausfilter[0][2]=0;
                     //gausfilter[1][0]=255;gausfilter[1][1]=255;gausfilter[1][2]=255;
                     //gausfilter[2][0]=0;gausfilter[2][1]=255;gausfilter[2][2]=0;

                      for (int x=rx-r ; x<rx+r ; x++){
                         for(int y=ry-r;y<ry+r;y++){

                             int d = (pow((rx-y),2))+(pow((ry-x),2));
                             int rad = pow(r,2);

                             if ( d <= rad ){

                              if(img4[x][y] > uref ){
                                  img4[x-1][y+1]=255;
                                  img4[x-1][y]=255;
                                  img4[x-1][y-1]=255;
                                  img4[x][y-1]=255;

                                  img4[x-2][y+2]=255;
                                  img4[x-2][y]=255;
                                  img4[x-2][y-2]=255;
                                  img4[x][y-2]=255;

                                  img4[x-3][y+3]=255;
                                  img4[x-3][y]=255;
                                  img4[x-3][y-3]=255;
                                  img4[x][y-3]=255;

                              }


                             }
                         }
                    }






                    //---------------------------------------------------------------------------

                     cout <<"\n promedio: "<<promedio<<"\n mayor: "<<mayor<<"\n humbral: "<<uref<<"\n";

                      cout <<"\n"<<imgCol<<"\t"<<imgRow;
                     cout<<"\n rx:"<<rx<<"\n ry:"<<ry<<"\n ry-r:"<<ry-r<<"\n rx-r:"<<rx-r<<"\n"<<"\n ry+r:"<<ry+r<<"\n rx+r:"<<rx+r<<"\n r: "<<r;
                     */
                    print.PrintImgs(img4, imgRow, imgCol, name_file.c_str());

                    //-----------
                    // fase de normalizacion
                    int nrow = 20, nrcol = 240;
                    Matrix normalize = normaliseiris(img4, imgRow, imgCol, rx, ry, r, rx2, ry2, r2, nrow, nrcol);
                    //----fin fase

                    delete[] auximg;
                    auximg = nullptr;
                    deleteIntVector(pcoor2);
                    deleteIntVector(pcoor);
                    deleteDoubleMatrix(normalize, nrow);
                    deleteDoubleMatrix(img3, imgRow);
                    // deleteDoubleMatrix(img4,imgRow);
                    deleteDoubleMatrix(mat_gradient2, fil_col);
                    deleteDoubleMatrix(mat_or2, fil_col);
                    deleteDoubleMatrix(i42, fil_col);
                    deleteIntMatrix(final2, fil_col);
                }
            }
        }
    }
    catch (exception &e)
    {
        cout << "\nexception thrown!" << endl;
        cout << e.what() << endl;
    }
}

