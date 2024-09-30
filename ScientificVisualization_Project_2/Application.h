
#ifndef __Application_H
#define __Application_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// put your global typedefs here

typedef struct{
	unsigned char *data;  // an image of bytes
	int *intData;         // an image of integers
    float *fdata;         // an image of floats
    int nx,ny,n;          // image dimensions
    int ncolorChannels;   // number of color channels in the image (1 or 3)
} Image;

typedef struct{
	unsigned char *data;  // a volume of bytes
    int nx,ny,nz,n;       // image dimensions (nz is for a volume, a "3D image")
} Volume;

class Application {
public:
  Application();


 
// put your application prototypes here: 


  void ReadVolumeFile();
  void XRay();
  void MIP();

  /*void GUI_SliderX();
  void GUI_SliderY();
  void GUI_SliderZ();
  void GUI_SliderWidth();
  void GUI_SliderHeight();*/
  void VolumeRendering();
};

#endif
