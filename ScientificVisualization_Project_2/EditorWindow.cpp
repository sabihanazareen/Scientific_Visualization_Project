#include "EditorWindow.h"
#include "Gui.h"
#include <list>
#include <iostream>
#include <algorithm>

extern Gui *gui;
extern Image curImage;
extern std::list <Image> ImageHistoryList;
int lastx;

// for images we only have one transfer function (ie, maxTransFunc=1)
// for volumes we will have 4 transfer function (ie, maxTransFunc=4) 

TransferFunction transFunc[4];  // can have up to 4 transfer functions
int maxTransFunc;               // number of transfer functions that can be edited 
int activeTransFunc;            // indicates which transfer function is currently edited
float transFuncColor[4][3];     // holds the color triple for each transfer function

extern int histogram[3][256];
// the constructor method

CEditorWindow::CEditorWindow(int x,int y,int w,int h,const char *l)
        : Fl_Gl_Window(x,y,w,h,l)
{
  // allocate and initialize your data structures here:

    for(int j=0;j<4;j++)
	{
       for(int i=0;i<256;i++)
	   {
		  transFunc[j][i]=i;
	   }
	}
}

// the drawing method: it draws the transFunc into the window

void CEditorWindow::draw() {
   if (!valid()) {
      glLoadIdentity(); glViewport(0,0,w(),h()); gluOrtho2D(0,w(),0,h());
      make_current();
   }

   int i,j;


// clear the window

   glClearColor(0.0,0.0,0.0,0.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   

// draw the histogram in white here:
// ( h() gives you the window height, w() the width)
   double hightPercentage;
   //std::cout << curImage.n;
   //if (curImage.n > 0) {
	  // hightPercentage = (double)h() / ((double)curImage.n / 3);
	  // std::cout << hightPercentage;
   //}
   //else
	  // hightPercentage = 0;

   glBegin(GL_LINES);
	   for (int i = 0; i < 3; i++) {
		   if (i == 0) glColor3f(1.0, 0.0, 0.0); // R
		   if (i == 1) glColor3f(0.0, 1.0, 0.0); // G
		   if (i == 2) glColor3f(0.0, 0.0, 1.0); // B
		   int* temp1 = std::max_element(histogram[i], histogram[i] + 256);
		   int temp2 = *temp1;
		   hightPercentage = 300.0 / temp2;

		   for (int j = 0; j < 256; j++)
		   {
			   glVertex2d((w() / 255.0) * j, histogram[i][j] * hightPercentage);
			   glVertex2d((w() / 255.0) * (j + 1), histogram[i][j + 1] * hightPercentage);
		   }
	   }
   glEnd();


// draw the maxTransFunc transFunc's

   for(j=0;j<maxTransFunc;j++)
   {
	  glColor3fv(transFuncColor[j]);  // a 3-vector of color

// connect the transFunc points by lines and connect the end and start to 0

      glBegin(GL_LINES);
        for(i=0;i<255;i++)
		{
  	      glVertex2i(i,transFunc[j][i]);
          glVertex2i(i+1,transFunc[j][i+1]);
		} 
 	    glVertex2i(255,transFunc[j][255]);
	    glVertex2i(256,0);
      glEnd();
   }
}


// the event handler that processes the mouse events in the transfer function editor window
// you will not have to change anything here

int CEditorWindow::handle(int eventType)
{
  int curx,cury,button,i,dxAbs,ix,lasty,xincr;
  float u;

  button=Fl::event_button();

  // mouse moves with button depressed
  if(eventType==FL_DRAG)
  {
    curx=Fl::event_x();

    // FLTK's origin is at the top of the window, we would like it at the bottom
	// subtract returned y coordinate from the window's height
    cury=h()-Fl::event_y();  

    if((curx>255)||(curx<0)) // point outside the x-range [0, 255]
		return(1);

	if(cury>255) // make sure transFunc always in the range [0, 255]
	    cury=255;
	else if(cury<0)
	    cury=0;

    transFunc[activeTransFunc][curx]=cury;  // assign the mouse value to transFunc
 
/* When the user moves the mouse along a continuous path, FLTK may not catch all 
   mouse events along the x-axis. The result is a jaggy curve where some of
   the old curve points survived, with intermittend updated curve points. This code
   draws a bilinearly interpolated line between the curve points of the 
   currently detected mouse event and the last known one since the mouse 
   push. It also aupdates the transfer function accordingly.
  */

	if(lastx>=0) 
	{
      dxAbs=abs(lastx-curx);
      if(dxAbs>1) 
	  {
	     xincr=dxAbs/(curx-lastx);
	     lasty=transFunc[activeTransFunc][lastx];
	     ix=lastx;
	     i=0;
	     while(ix!=curx)
		 {
	  	    u=(float)i/dxAbs;
		    transFunc[activeTransFunc][ix]=(int)(u*cury+(1-u)*lasty);
            ix+=xincr;
		    i+=1;
		 }
	  }
	}
    lastx=curx;
  }

  // depressing right mouse button resets the transFunc array 
  else if((eventType==FL_PUSH)&&(button==3))
  {
    for(i=0;i<255;i++)
	{
	  transFunc[activeTransFunc][i]=i;
	}		
  }
  else if((eventType==FL_PUSH)&&(button==1))
  {
     lastx=Fl::event_x();
  }


  // redraw the transFunc into window
  redraw();

  return(1);
}

