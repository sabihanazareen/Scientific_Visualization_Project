#define _CRT_SECURE_NO_DEPRECATE
#include "Application.h"
#include <FL/fl_file_chooser.H>
#include "Gui.h"
#include <iostream>
#include <list>
#include <algorithm>
#include "Matrix.h"

extern Gui *gui;
Image curImage; // make inImage a global, need it in other classes
Volume vol;

std::list <Image> ImageHistoryList; 


// these are declared in transFunc.cpp
extern TransferFunction transFunc[4];
extern int maxTransFunc;              
extern int activeTransFunc;       
extern float transFuncColor[4][3]; 
int histogram[3][256];

Matrix rotX(3, 3), rotY(3, 3), rotZ(3, 3);
int volumeSwitch = 0; 
int h, w;

// help functions

int calculatePixelIndex(int i, int j) {
	return (i * curImage.nx + j) * curImage.ncolorChannels;
}

double retrieveVoxelDensity(int i, int j, int k) {
	return vol.data[i * (vol.ny * vol.nz) + j * (vol.nz) + k];
}

void calculateRotationMatrix() {

	double rotationAngleX = gui->sliderX->value();
	double rotationAngleY = gui->sliderY->value();
	double rotationAngleZ = gui->sliderZ->value();
	
	rotationAngleX = (rotationAngleX * acos(-1.0)) / 180.0;
	rotationAngleY = (rotationAngleY * acos(-1.0)) / 180.0;
	rotationAngleZ = (rotationAngleZ * acos(-1.0)) / 180.0;
	
	rotX.data[rotX.getIndex(0, 0)] = 1;
	rotX.data[rotX.getIndex(0, 1)] = 0;
	rotX.data[rotX.getIndex(0, 2)] = 0;

	rotX.data[rotX.getIndex(1, 0)] = 0;
	rotX.data[rotX.getIndex(1, 1)] = cos(rotationAngleX);
	rotX.data[rotX.getIndex(1, 2)] = -sin(rotationAngleX);

	rotX.data[rotX.getIndex(2, 0)] = 0;
	rotX.data[rotX.getIndex(2, 1)] = sin(rotationAngleX);
	rotX.data[rotX.getIndex(2, 2)] = cos(rotationAngleX);

	rotY.data[rotY.getIndex(0, 0)] = cos(rotationAngleY);
	rotY.data[rotY.getIndex(0, 1)] = 0;
	rotY.data[rotY.getIndex(0, 2)] = sin(rotationAngleY);

	rotY.data[rotY.getIndex(1, 0)] = 0;
	rotY.data[rotY.getIndex(1, 1)] = 1;
	rotY.data[rotY.getIndex(1, 2)] = 0;

	rotY.data[rotY.getIndex(2, 0)] = -sin(rotationAngleY);
	rotY.data[rotY.getIndex(2, 1)] = 0;
	rotY.data[rotY.getIndex(2, 2)] = cos(rotationAngleY);

	rotZ.data[rotZ.getIndex(0, 0)] = cos(rotationAngleZ);
	rotZ.data[rotZ.getIndex(0, 1)] = -sin(rotationAngleZ);
	rotZ.data[rotZ.getIndex(0, 2)] = 0;

	rotZ.data[rotZ.getIndex(1, 0)] = sin(rotationAngleZ);
	rotZ.data[rotZ.getIndex(1, 1)] = cos(rotationAngleZ);
	rotZ.data[rotZ.getIndex(1, 2)] = 0;

	rotZ.data[rotZ.getIndex(2, 0)] = 0;
	rotZ.data[rotZ.getIndex(2, 1)] = 0;
	rotZ.data[rotZ.getIndex(2, 2)] = 1;
}



// the constructor method for the Application class

Application::Application()
{
  // initialize the image data structure
  curImage.nx=curImage.ny=curImage.n=curImage.ncolorChannels=0;

  // add more initialization here:

}

void Application::ReadVolumeFile() {
	FILE* fp;
	char imageType[20], str[100];

char* file = fl_file_chooser("Pick a file to READ from", "*.{pgm,ppm,vol}", "");
if (file == NULL)
return;

fp = fopen(file, "rb");
fgets(str, 100, fp);
sscanf(str, "%s", imageType);
if (!strncmp(imageType, "P7", 2)) { // volume data
	// skip comments embedded in header
	fgets(str, 100, fp);
	while (str[0] == '#')
		fgets(str, 100, fp);

	// read volume dimensions 
	sscanf(str, "%d %d %d", &vol.nx, &vol.ny, &vol.nz);
	vol.n = vol.nx * vol.ny * vol.nz;

	fgets(str, 100, fp);

	vol.data = (unsigned char*)malloc(vol.n * sizeof(unsigned char));
	fread(vol.data, sizeof(unsigned char), vol.n, fp);
}
fclose(fp);

std::cout << "welcome to the club";
std::cout << vol.data;

VolumeRendering();
};



void Application::XRay() {
	volumeSwitch = 0;
	VolumeRendering();
};
void Application::MIP() {
	volumeSwitch = 1;
	VolumeRendering();
};

void Application::VolumeRendering() {

	h = gui->sliderWidth->value();
	w = gui->sliderHeight->value();


	double scaleVector[] = { 0,0,2 };
	double initialPoint[3];
	double rayDirection[] = { 0,0,-1 };
	double uVector[] = { 1,0,0 };
	double vVector[] = { 0,1,0 };

	calculateRotationMatrix();
	Matrix rotatedU(1, 3), rotatedV(1, 3), rotatedRay(1, 3), rotatedScale(1, 3);
	for (int i = 0; i < 3; i++) {
		rotatedU.data[rotatedU.getIndex(0, i)] = uVector[i];
		rotatedV.data[rotatedV.getIndex(0, i)] = vVector[i];
		rotatedRay.data[rotatedRay.getIndex(0, i)] = rayDirection[i];
		rotatedScale.data[rotatedScale.getIndex(0, i)] = scaleVector[i];
	}
	rotatedU = ((rotatedU.multiply(rotX)).multiply(rotY)).multiply(rotZ);
	rotatedV = ((rotatedV.multiply(rotY)).multiply(rotZ)).multiply(rotX);
	rotatedRay = ((rotatedRay.multiply(rotX)).multiply(rotY)).multiply(rotZ);
	rotatedScale = ((rotatedScale.multiply(rotX)).multiply(rotY)).multiply(rotZ);
	double uv[3];

	for (int i = 0; i < 3; i++) {
		uVector[i] = rotatedU.data[rotatedU.getIndex(0, i)];
		vVector[i] = rotatedV.data[rotatedV.getIndex(0, i)];
		rayDirection[i] = rotatedRay.data[rotatedRay.getIndex(0, i)];
		scaleVector[i] = rotatedScale.data[rotatedScale.getIndex(0, i)];
		uv[i] = -(uVector[i] + vVector[i]);
	}

	for (int i = 0; i < 3; i++) {
		initialPoint[i] = scaleVector[i] - (((double)(w / 2.0) / (double)(w))) * uVector[i];
	}
	for (int i = 0; i < 3; i++) {
		initialPoint[i] = initialPoint[i] - (((double)(h / 2.0) / (double)(h))) * vVector[i];
	}

	delete[] curImage.data;

	curImage.ncolorChannels = 1;
	curImage.nx = h;
	curImage.ny = w;
	curImage.n = curImage.nx * curImage.ny;
	curImage.data = new unsigned char[curImage.ncolorChannels * curImage.n];

	for (int i = 0; i < curImage.nx; i++) {
		for (int j = 0; j < curImage.ny; j++) {
			double pij[3];
			double tempu[3];
			double tempv[3];
			double temp[3];
			for (int k = 0; k < 3; k++) {
				tempu[k] = ((double)(i) / h) * uVector[k];
			}

			for (int k = 0; k < 3; k++) {
				tempv[k] = ((double)(j) / w) * vVector[k];
			}

			for (int k = 0; k < 3; k++) {
				temp[k] = tempu[k] + tempv[k];;
			}

			for (int k = 0; k < 3; k++) {
				pij[k] = initialPoint[k] + temp[k];
			}


			double tbegin, tend;
			bool f = false;

			double sum = 0;
			double maxval = 0;
			double step = 0.1;

			for (double t = 0; t <= 50; t += step) {
				double sampleloc[3];
				double lx, ly, lz, p;
				double w, v, u;
				double fdens;

				sampleloc[0] = pij[0] + t * rayDirection[0];
				sampleloc[1] = pij[1] + t * rayDirection[1];
				sampleloc[2] = pij[2] + t * rayDirection[2];

				lx = (1.00 / (vol.nx - 1));
				ly = (1.00 / (vol.ny - 1));
				lz = (1.00 / (vol.nz - 1));
				p = ((sampleloc[0] + 0.5) / lx) * lx;
				int i = p * (vol.nx - 1);
				p = ((sampleloc[1] + 0.5) / ly) * ly;
				int j = p * (vol.ny - 1);
				p = ((sampleloc[2] + 0.5) / lz) * lz;
				int k = p * (vol.nz - 1);
				if (i < 0 || j < 0 || k < 0) {
					if (f)	break;
					continue;
				}
				if (i >= vol.nx - 1 || j >= vol.ny - 1 || k >= vol.nz - 1) {
					if (f)	break;
					continue;
				}
				w = sampleloc[0] - (i * lx);
				v = sampleloc[1] - (j * ly);
				u = sampleloc[2] - (k * lz);
				f = true;
				fdens =
					retrieveVoxelDensity(i, j, k) * (1 - u) * (1 - v) * (1 - w) +
					retrieveVoxelDensity(i + 1, j, k) * u * (1 - v) * (1 - w) +
					retrieveVoxelDensity(i + 1, j, k + 1) * u * (1 - v) * (w)+
					retrieveVoxelDensity(i, j, k + 1) * (1 - u) * (1 - v) * w +
					retrieveVoxelDensity(i, j + 1, k) * (1 - u) * v * (1 - w) +
					retrieveVoxelDensity(i + 1, j + 1, k) * v * u * (1 - w) +
					retrieveVoxelDensity(i + 1, j + 1, k + 1) * v * w * u +
					retrieveVoxelDensity(i, j + 1, k + 1) * (1 - u) * v * w;

				if (volumeSwitch == 0) {
					sum = sum + fdens * step;
				}
				else if (volumeSwitch == 1) {
					maxval = max(maxval, fdens);
				}
			}
			if (volumeSwitch == 0) {
				curImage.data[calculatePixelIndex(j, i)] = round(sum);
			}
			else if (volumeSwitch == 1) {
				curImage.data[calculatePixelIndex(j, i)] = round(maxval);
			}
		}
	}
	gui->DisplayWindow->redraw();
}