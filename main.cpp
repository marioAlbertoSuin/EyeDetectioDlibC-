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

unsigned long tid;

// -----------------------------------------------------------------------------------------
using namespace dlib;
using namespace std;
int tam_ventana = 3;
// FUNCION PARA CALCULAR CALIDAD DE IMAGENES
pair<double, double> calculatePSNR(unsigned char *in, unsigned char *out, int nr, int nc);

// ----------------------------------------------------------------------------------------
// ESCRIBIR METRICAS CSV
void writePerfMetricsFilter(ofstream &file, int i, string filename, double cpu, int mem, double time, double ms, double pr, int ventana)
{
    file << i << "," << filename << "," << cpu << "," << mem << "," << time << "," << ms << "," << pr << "," << ventana << endl;
}

void writePerfMetricsShape(ofstream &file, int i, string filename, double cpu, int mem, double time)
{
    file << i << "," << filename << "," << cpu << "," << mem << "," << time << endl;
}

//-----------------------------------------------------------------------------------------
// FILTRO HIBIRIDO MEDIA

void filtroHM(string nombre)
{
    systemMetrics performance("perf");
    ofstream metrics;

    if (true)
    {
        metrics.open("metricsFilter.csv", ios_base::app); // Append
    }
    else
    {
        metrics.open("metricsFilter.csv"); // Append
    }
    performance.resetCounters();
    try
    {

        //median hibrid filtering
        HibridMedianfilter Firsttest("resul/" + nombre + ".bmp", "resul/" + nombre + "filter.bmp");
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
        writePerfMetricsFilter(metrics, 1, nombre, cpu, mem, totalSeconds, retvalData.first, retvalData.second, tam_ventana);
        metrics.close();
    }
    catch (const std::exception &)
    {
    }
}

//-----------------------------------------------------------------------------------------
// OBTENER NOMBRE IMAGENES
string tokenize(string q)
{

    std::string token1 = q.erase(0, q.find("/") + 1);
    std::string token2 = token1.substr(0, token1.find("."));
    return token2;
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
    //array2d<rgb_pixel> eyes;
    //array2d<rgb_pixel> resul2;
    
    //int h1 =(RigthEyep2_y+25)-(LeftEyep1_y);
    //int w1 =(RigthEyep2_x+40)-(LeftEyep1_x);
    //
    
    

    const std::array<dlib::dpoint, 4> &ptsOjosCompletos = 
    {{dpoint(LeftEyep1_x -15, LeftEyep1_y - 15), 
    dpoint(RigthEyep2_x + 7, RigthEyep2_y + 15), 
    dpoint(LeftEyep1_x - 15, LeftEyep1_y + 15), 
    dpoint(RigthEyep2_x + 20, RigthEyep2_y - 15)}};

    
    /*
    int h1 =(RigthEyep2_y+15)-(RigthEyep1_y-15);
    int w1 =(RigthEyep2_x+15)-(RigthEyep1_x-15);
    resul2.set_size(w1, h1);
    */
    int h =(LeftEyep2_y)-(LeftEyep1_y);
    int w =(LeftEyep2_x)-(LeftEyep1_x);
    
    //corte de los ojos
    resul.set_size(w, h);
    array2d<rgb_pixel> &crop_img = resul ;
    //array2d<rgb_pixel> &crop_img2 = resul2;
    
    const std::array<dlib::dpoint, 4> &pts = 
    {{dpoint(LeftEyep1_x - 15, LeftEyep1_y - 15), 
    dpoint(LeftEyep2_x + 7, LeftEyep2_y + 15), 
    dpoint(LeftEyep1_x - 15, LeftEyep1_y + 15), 
    dpoint(LeftEyep2_x + 20, LeftEyep2_y - 15)}};

    dlib::extract_image_4points(img, crop_img, pts);
    save_bmp(crop_img, "resul/" + nombre + ".bmp");
    
    /*
    const std::array<dlib::dpoint, 4> &pts1 = 
    {{dpoint(RigthEyep1_x - 10, RigthEyep1_y - 10), 
    dpoint(RigthEyep2_x + 10, RigthEyep2_y + 10), 
    dpoint(RigthEyep1_x - 20, RigthEyep1_y + 27), 
    dpoint(RigthEyep2_x + 20, RigthEyep2_y - 27)}};

    dlib::extract_image_4points(img, crop_img2, pts1);
    save_bmp(crop_img2, "resul/Rigth" + nombre + ".bmp");
    */
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
                LeftEyep1_y = d.part(37).y();
                LeftEyep2_x = d.part(40).x();
                LeftEyep2_y = d.part(40).y();

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
                    cout << "pixel 43:  " << shape.part(43) << endl;
                    cout << "pixel 46: " << shape.part(46) << endl;
                    // You get the idea, you can get all the face part locations if
                    // you want them.  Here we just store them in shapes so we can
                    // put them on the screen.
                    shapes.push_back(shape);
                }
            
            //ojos
            win.clear_overlay();
            win.set_image(img);
            win.add_overlay(Extraccion_puntos_ojos(shapes, argv[i]));
            string nombre = tokenize(argv[i]);
            filtroHM(nombre);

            }

            cout << "Hit enter to process the next image..." << endl;
            //cin.get();
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
