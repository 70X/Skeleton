#ifndef __CAMERA_HH
#define __CAMERA_HH

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
 
#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#include "trackball.hh"


class Camera {
public:

    typedef struct {
	double x,y,z;
    } recVec;

    typedef struct {
	recVec viewPos; // View position
	recVec viewDir; // View direction vector
	recVec viewUp; // View up direction
	recVec rotPoint; // Point to rotate about
	double focalLength; // Focal Length along view direction
	double aperture; // gCamera aperture
	double eyeSep; // Eye separation
	int screenWidth,screenHeight; // current window/screen height and width
    } recCamera;

    float gShapeSize;

    int gDollyPanStartPoint[2];
    float gTrackBallRotation [4];
    bool gDolly;
    bool gPan;
    bool gTrackBall;
    float gWorldRotation[4];
    
    bool gLines;
    bool gPolygons;
    
    bool gShowHelp;
    bool gShowInfo;

    recCamera gCamera;
    recVec gOrigin;

    int gLastKey;
    int gMainWindow;

    Camera();
    ~Camera();

    void gCameraReset(double diagonal, double center[3]);
    void SetLighting(unsigned int mode);

    void reshape (int w, int h);
    void display_begin (void);
    void display_end (void);
    void special(int key, int px, int py);
    void mouse_move (int x, int y);
    void mouse (int button, int state, int x, int y);
    void key(unsigned char inkey, int px, int py);

private:

    void gCameraDolly(float dolly);
    void drawGLText (int window_width, int window_height);
    void drawGLString(float x, float y, char *string);    
    void mouse_end_dolly ();
    void mouse_end_pan ();
    void mouse_end_track (int x, int y);
};


#endif
