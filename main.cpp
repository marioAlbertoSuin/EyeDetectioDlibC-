#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <iostream>
#include <fstream>
#include <dlib/image_transforms.h>
#include <dlib/image_transforms/interpolation.h>
#include "HibridMedianfilter.h"
#include <stdio.h>
#include <cstdio>
#include <vector>
#include <cmath>
#include "array.h"
#include "bmp.h"
#include <stdlib.h>
#include "system_metrics.h"
#include "memcount.h"
#include <omp.h>
#include "includes.h"


unsigned long tid;

// -----------------------------------------------------------------------------------------
using namespace dlib;
using namespace std;
int tam_ventana = 3;
// FUNCION PARA CALCULAR CALIDAD DE IMAGENES
pair<double, double> calculatePSNR(unsigned char *in, unsigned char *out, int nr, int nc);

//----------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------
// ESCRIBIR METRICAS CSV
void writePerfMetricsFilter(ofstream &file, string filename, double cpu, int mem, double time, double ms, double pr, int ventana)
{
    file << filename << "," << cpu << "," << mem << "," << time << "," << ms << "," << pr << "," << ventana << endl;
}

void writePerfMetricsShape(ofstream &file, int i, string filename, double cpu, int mem, double time)
{
    file << i << "," << filename << "," << cpu << "," << mem << "," << time << endl;
}


//-----------------------------------------------------------------------------------------
// OBTENER NOMBRE IMAGENES
string tokenize(string q)
{

    std::string token1 = q.erase(0, q.find("/") + 1);
    std::string token2 = token1.substr(0, token1.find("."));
    return token2;
}


//-----------------------------------------------------------------------------------------
// FILTRO HIBIRIDO MEDIA

void filtroHM(string nom)
{
    
    string nombre=tokenize(nom);
    systemMetrics performance("perf");
    performance.resetCounters();
    ofstream metrics;

    if (true)
    {
        metrics.open("metricsFilter.csv", ios_base::app); // Append
    }
    else
    {
        metrics.open("metricsFilter.csv"); // Append
    }
    
  
        
        //median hibrid filtering
        Medianfilter Firsttest("resul/" + nombre + ".bmp", "resul/" + nombre + "filter.bmp");
        Firsttest.applyfilter(tam_ventana);

        unsigned char *ima;  // originalFile
        unsigned char *bima; // processedFile
        int nr = 0;          // originalFile image width
        int nc = 0;          // originalFile image height
        int depth = 0;       // originalFile image depth
        int nrNew = 0;       // processedFile image width
        int ncNew = 0;       // processedFile image height
        int depthNew = 0;    //processedFile image depth

        std::string nombre1 = "resul/" + nombre + ".bmp";
        const char *sinfiltro = nombre1.c_str();

        std::string nombre2 = "resul/" + nombre + "filter.bmp";
        const char *confiltro = nombre2.c_str();

        // read originalFile
        ima = Read_BMP_To_1D(sinfiltro, &nr, &nc, &depth);
        if (ima == NULL)
        {
            cerr << "cannot read file: " << sinfiltro << endl;
        }
        cout << sinfiltro << endl;
        // read processedFile
        bima = Read_BMP_To_1D(confiltro, &nrNew, &ncNew, &depthNew);
        if (bima == NULL)
        {
            cerr << "cannot read file: " << confiltro << endl;
        }
        cout << confiltro << endl;

        pair<double, double> retvalData = calculatePSNR(ima, bima, nr, nc);

        cout << "MSE: " << retvalData.first << endl
             << "PSNR: " << retvalData.second << endl;

        free(ima);
        free(bima);
        performance.calculate();
        double cpu = performance.getCpuPercent();
        int mem = getRamUsage();
        double totalSeconds = performance.getDurationInMiliseconds();
        writePerfMetricsFilter(metrics, nom, cpu, mem, totalSeconds, retvalData.first, retvalData.second, tam_ventana);
        metrics.close();
    
    
}

// ----------------------------------------------------------------------------------------
// RECORTE DE LOS OJOS
void cutImage_eyes(const std::string &path, int LeftEyep1_x, int LeftEyep1_y, int LeftEyep2_x, int LeftEyep2_y,int RigthEyep1_x,int RigthEyep1_y,int RigthEyep2_x,int RigthEyep2_y)
{

    string nombre = tokenize(path);

    //Cargar imagen
    array2d<rgb_pixel> img;
    load_image(img, path);
    //imagenes de salida
    array2d<rgb_pixel> resul;
  
    int h=(LeftEyep2_y-LeftEyep1_y)+(100-((LeftEyep2_y-LeftEyep1_y)%100));
    int w =(LeftEyep2_x-LeftEyep1_x)+(100-((LeftEyep2_x-LeftEyep1_x)%100));
    
    //corte de los ojos
    resul.set_size(h, w);
    array2d<rgb_pixel> &crop_img = resul ;
   
    
    const std::array<dlib::dpoint, 4> &pts = 
    {{dpoint(LeftEyep1_x - 30, LeftEyep1_y - 30), 
    dpoint(LeftEyep2_x + 30, LeftEyep2_y + 30), 
    dpoint(LeftEyep1_x - 30, LeftEyep1_y + 30), 
    dpoint(LeftEyep2_x + 30, LeftEyep2_y - 30)}};

    extract_image_4points(img, crop_img, pts);
    //escala de grices
    //array2d<unsigned char> img_gray;
    //array2d<unsigned char> Salidaimg_gray;
    //assign_image(img_gray, crop_img);
    save_bmp(crop_img, "resul/" + nombre + ".bmp");
    
 
}

// ----------------------------------------------------------------------------------------
// EXTRACCION CORDENADAS DE OJOS
inline std::vector<image_window::overlay_line> Extraccion_puntos_ojos(
    const std::vector<full_object_detection> &dets,
    const std::string &path,
    const rgb_pixel color = rgb_pixel(17, 255, 0))
{
    systemMetrics performance("perf");
    ofstream metrics;

    if (true)
    {
        metrics.open("metricsShapePredictor.csv", ios_base::app); // Append
    }
    else
    {
        metrics.open("metricsShapePredictor.csv"); // Append
    }
    performance.resetCounters();
    std::vector<image_window::overlay_line> lines;

        for (unsigned long i = 0; i < dets.size(); ++i)
        {
            DLIB_CASSERT(dets[i].num_parts() == 68 || dets[i].num_parts() == 5,
                         "\t std::vector<image_window::overlay_line> render_face_detections()"
                             << "\n\t You have to give either a 5 point or 68 point face landmarking output to this function. "
                             << "\n\t dets[" << i << "].num_parts():  " << dets[i].num_parts());

            const full_object_detection &d = dets[i];

            if (d.num_parts() == 5)
            {
                lines.push_back(image_window::overlay_line(d.part(0), d.part(1), color));
                lines.push_back(image_window::overlay_line(d.part(1), d.part(4), color));
                lines.push_back(image_window::overlay_line(d.part(4), d.part(3), color));
                lines.push_back(image_window::overlay_line(d.part(3), d.part(2), color));
            }
            else
            {

                int LeftEyep1_x = 0;
                int LeftEyep1_y = 0;
                int LeftEyep2_x = 0;
                int LeftEyep2_y = 0;
                
                
                tid=37;
                // Left eye

              for (unsigned long i = 37; i <= 41; ++i)
                {
                    
                    lines.push_back(image_window::overlay_line(d.part(i), d.part(i - 1), color));
                }
                
                LeftEyep1_x = d.part(37).x();
                LeftEyep1_y = d.part(38).y();
                LeftEyep2_x = d.part(40).x();
                

                lines.push_back(image_window::overlay_line(d.part(36), d.part(41), color));

                // Right eye
                int RigthEyep1_x = 0;
                int RigthEyep1_y = 0;
                int RigthEyep2_x = 0;
                int RigthEyep2_y = 0;

                for (unsigned long i = 43; i <= 47; ++i)
                {
                    lines.push_back(image_window::overlay_line(d.part(i), d.part(i - 1), color));
                }

                RigthEyep1_x = d.part(43).x();
                RigthEyep1_y = d.part(43).y();
                RigthEyep2_x = d.part(46).x();
                RigthEyep2_y = d.part(46).y();

                LeftEyep2_y = d.part(42).y();

                lines.push_back(image_window::overlay_line(d.part(42), d.part(47), color));
                try
                {
                    cutImage_eyes(path, LeftEyep1_x, LeftEyep1_y, LeftEyep2_x, LeftEyep2_y,RigthEyep1_x,RigthEyep1_y,RigthEyep2_x,RigthEyep2_y);
                }
                catch (const std::exception &)
                {
                    
                }
                performance.calculate();
                double cpu = performance.getCpuPercent();
                int mem = getRamUsage();
                double totalSeconds = performance.getDurationInMiliseconds();
                writePerfMetricsShape(metrics, 1, path, cpu, mem, totalSeconds);
                metrics.close();
            }
        }
    
    return lines;
}
// -----------------------------------------------------------------------------------------
int main(int argc, char **argv)
{

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
            cout << "processing image " << argv[i] << endl;
            array2d<rgb_pixel> img;
            load_image(img, argv[i]);
            // Make the image larger so we can detect small faces.
            //pyramid_up(img);

            // Now tell the face detector to give us a list of bounding boxes
            // around all the faces in the image.
            std::vector<rectangle> dets = detector(img);
            cout << "Number of faces detected: " << dets.size() << endl;

            if(dets.size()==0){
                continue;
            }else{
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
                }
       
            //ojos
            
            win.clear_overlay();
            win.set_image(img);
            win.add_overlay(Extraccion_puntos_ojos(shapes, argv[i]));


            
            
            chip_details();
            try{
                filtroHM(argv[i]);
            }catch(const std::exception &){

            }

            
            }
            /*
            string nombre = tokenize(argv[i]);
            array2d<rgb_pixel> image;
            
            load_image(image,"resul/"+nombre+"filter.bmp");
            array2d<unsigned char> gaussianbur;
            gaussian_blur(image,gaussianbur);
            array2d<short> horz_gradient,vert_gradient;
            array2d<unsigned char> edge_image;
            sobel_edge_detector(gaussianbur,horz_gradient,vert_gradient);
            suppress_non_maximum_edges(horz_gradient,vert_gradient,edge_image);
            
            save_bmp(edge_image,"resul/"+nombre+"edges.nmp");
            */
   
            cout << "Hit enter to process the next image..." << endl;
            cin.get();
        }
    PrintImg print;
    int id_user = 0;
    DIR *dir;
    struct dirent *diread;
    const char *path = "/home/marcelo/Documentos/tesis/EyeDetectioDlibC-/build/resul/";

    if ((dir = opendir(path)) != nullptr) {
        while ((diread = readdir(dir)) != nullptr) {

            std::string name_file = diread->d_name;
            if (name_file != "." && name_file != "..") {
                std::string val=path+name_file;
                int imgRow,imgCol;
                float *auximg;
                Matrix img3;

                std::cout<<name_file<<std::endl;
                loadImage2(val.c_str(),imgRow,imgCol,auximg,img3);
                id_user = nombre_ubiris(name_file.c_str()); 
                
                int outrow=imgRow,outcol=imgCol;
                scaling(outrow,outcol,400);
                Matrix img=imgResize(auximg,imgRow,imgCol,outrow,outcol);
                Matrix mat_gradient=newDoubleMatrix(outrow,outcol);
                Matrix mat_or=newDoubleMatrix(outrow,outcol);
                
                //ubiris session1 2.3,1.5,0.34,0.27
                //ubiris session2 2.2,1.5,0.29,0.24
                //casia lamp 2.2,1.5,0.23,0.19
                //personal 2.01,2.3,0.24,0.18
                double g=2.2; double borde=1.5; double histmax=0.34; double histmin=0.27;

                canny(img,mat_gradient,mat_or,outrow,outcol,1,1,1);
                adjgamma(mat_gradient,outrow,outcol,g);           
                Matrix i4=nonmaxsup(mat_gradient,outrow,outcol, mat_or,outrow,outcol, borde); 
                IntMatrix final=hysthresh(i4,outrow,outcol,histmax,histmin);  
                
    

                 //ubiris sesion1 y session2 rmin 88 rmax 105
                 //casia lamp rmin 55  rmax 77
                 //personal rmin 15 rmax 45
                int r_min = 88;
                int r_max = 105;
 
    
                IntVector pcoor=detectar_circulo(final,outrow,outcol,r_min,r_max,0.1);
                
                /* escalar el radio*/
                //int raux=pcoor[2];
                int rx, ry,r;
                resizeExternalCoor(rx,ry,r,imgCol,outcol,imgRow,outrow,pcoor);
      
                //---------------
                deleteDoubleMatrix(mat_gradient,outrow);
                deleteDoubleMatrix(mat_or,outrow);
                deleteDoubleMatrix(i4,outrow);
                deleteIntMatrix(final,outrow);

                int fila = pcoor[1];//x
                int columna = pcoor[0];//y
                int radio = pcoor[2];//r
                int fil_col = radio * 2;
                scaling(outrow,outcol,400);
                Matrix img2=imgResize(auximg,imgRow,imgCol,outrow,outcol);
                Matrix mat_ojo = newDoubleMatrix(fil_col,fil_col);
                int conti = 0;
                int contj = 0;
                for (int i = fila-radio; i < fila+radio; ++i)
                {
                    for (int j = columna-radio; j < columna+radio; ++j)
                    {
                        mat_ojo[conti][contj] = img2[i][j];
                        contj++;
                    }contj=0;conti++;
                }

                deleteDoubleMatrix(img2,outrow);

                Matrix mat_gradient2=newDoubleMatrix(fil_col,fil_col);
                Matrix mat_or2=newDoubleMatrix(fil_col,fil_col);

                 //--ubiris session1 y session2 3.4, 1.5 0.34,0.27
                 //casia lamp 2.2,1.5,0.34,0.27
                 //prsonal ojo 3.5,1.5 , 0.34,0.27
                double g2=3.4; double borde2=1.5; double histmax2=0.34; double histmin2=0.27;
                canny(mat_ojo,mat_gradient2,mat_or2,fil_col,fil_col,1,1,1,2);
                adjgamma(mat_gradient2,fil_col,fil_col,g2);  
                Matrix i42=nonmaxsup(mat_gradient2,fil_col,fil_col, mat_or2,fil_col,fil_col, borde2); 
                IntMatrix final2=hysthresh(i42,fil_col,fil_col,histmax2, histmin2); 

                //----------------------
                //ubiris session1 y sssion2 rmin=radio*0.18 rmax=radio*0.20
                //casia lamp rmin=radio*0.18 rmax=radio*0.18
                int r_min2 = round(radio*0.18);
                int r_max2 = radio-round(radio*0.2);
                IntVector pcoor2=detectar_circulo(final2,fil_col,fil_col,r_min2,r_max2,0.20,1);
 
                int rx2,ry2,r2;
                resizeInternalCoor( rx2,ry2,r2,imgCol,outcol,imgRow,outrow,pcoor2,columna, fila,radio); 
 

               
                Matrix gausfilter=newDoubleMatrix(5,5);
                gausfilter[0][0]=0.0121;gausfilter[0][1]=0.0261;gausfilter[0][2]=0.0337;gausfilter[0][3]=0.0261;gausfilter[0][4]=0.0121;
                gausfilter[1][0]=0.0261;gausfilter[1][1]=0.0561;gausfilter[1][2]=0.0724;gausfilter[1][3]=0.0561;gausfilter[1][4]=0.0261;
                gausfilter[2][0]=0.0337;gausfilter[2][1]=0.0724;gausfilter[2][2]=0.0935;gausfilter[2][3]=0.0724;gausfilter[2][4]=0.0337;
                gausfilter[3][0]=0.0261;gausfilter[3][1]=0.0561;gausfilter[3][2]=0.0724;gausfilter[3][3]=0.0561;gausfilter[3][4]=0.0261;
                gausfilter[4][0]=0.0121;gausfilter[4][1]=0.0261;gausfilter[4][2]=0.0337;gausfilter[4][3]=0.0261;gausfilter[4][4]=0.0121;
                Matrix img4=filter2(img3,gausfilter,imgRow,imgCol,5);
                deleteDoubleMatrix(gausfilter,5);
               
                 correctSegmentation(rx,ry,rx2, ry2, r2);

                 paintCircle(img4,imgRow, imgCol, rx, ry,rx2,ry2, r, r2);
                 print.PrintImgs(img4,imgRow,imgCol,name_file.c_str());



                //-----------
                //fase de normalizacion
                int nrow=20,nrcol=240;
                Matrix normalize=normaliseiris(img4,imgRow,imgCol,rx,ry,r,rx2,ry2,r2,nrow,nrcol);
                //----fin fase

                //-----fase extraccion de caracteristicas- codificacion
            
           
                //-----------------------------------------------------

                delete[]auximg; 
                auximg=nullptr; 
                deleteIntVector(pcoor2);
                deleteIntVector(pcoor);
                deleteDoubleMatrix(normalize,nrow);
                deleteDoubleMatrix(img3,imgRow);

                deleteDoubleMatrix(mat_gradient2,fil_col);
                deleteDoubleMatrix(mat_or2,fil_col);
                deleteDoubleMatrix(i42,fil_col);
                deleteIntMatrix(final2,fil_col);

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

pair<double, double> calculatePSNR(unsigned char *in, unsigned char *out, int nr, int nc)
{
    int i, j;
    int max = 0;
    int sum;
    double MSE;
    double PSNR;

    sum = max = 0;
    for (i = 0; i < nr; i++)
    {
        for (j = 0; j < nc; j++)
        {
            if (in[(i * nc) + j] > max)
                max = in[(i * nc) + j];
            int diff = (in[(i * nc) + j] - out[(i * nc) + j]);
            sum += diff * diff;
        }
    }
    //MSE = (double)sum / (nr * nc);
    MSE = abs((double)sum / (nr * nc));
    PSNR = 10.0f * log10((double)max * max / MSE);

    return make_pair(MSE, PSNR);
}

