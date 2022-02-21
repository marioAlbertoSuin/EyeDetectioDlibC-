   //   hybridmedianfilter.h - declarations for 
   //   2D hybrid median filter routines
   //
   //   The code is property of LIBROW
   //   You can use it on your own
   //   When utilizing credit LIBROW site

#ifndef _HYBRIDMEDIANFILTER_H_
#define _HYBRIDMEDIANFILTER_H_

//   Image element type
typedef double element;

//   2D HYBRID MEDIAN FILTER, window size 3x3
//     image  - input image
//     result - output image, NULL for inplace processing
//     N      - width of the image
//     M      - height of the image
void hybridmedianfilter(element* image, element* result, int N, int M);

#endif
