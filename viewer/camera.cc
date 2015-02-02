#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
 
#include "camera.hh"

#define DTOR 0.0174532925
#define CROSSPROD(p1,p2,p3) \
   p3.x = p1.y*p2.z - p1.z*p2.y; \
   p3.y = p1.z*p2.x - p1.x*p2.z; \
   p3.z = p1.x*p2.y - p1.y*p2.x
   
#ifndef MIN
	#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

Camera::Camera()
{
    gShapeSize = 11.0f;

    gDollyPanStartPoint[0] = gDollyPanStartPoint[1] = 0.0;
    gTrackBallRotation [0] = gTrackBallRotation [1] =
	gTrackBallRotation [2] = gTrackBallRotation [3] = 0.0;
    gDolly = false;
    gPan = false;
    gTrackBall = false;
    gWorldRotation [0] = 0.0;
    gWorldRotation [1] = 0.0;
    gWorldRotation [2] = 0.0;
    gWorldRotation [3] = 0.0;
    
    gLines = false;
    gPolygons = true;
    gShowHelp = true;
    gShowInfo = true;
    gOrigin.x = gOrigin.y = gOrigin.z = 0.0;
    gLastKey = ' ';
    gMainWindow = 0;
}

Camera::~Camera(){}

static const float INIT_APERTURE = 15.0;

void Camera::gCameraReset(double diagonal, double center[3])
{   
    gShapeSize = diagonal;
    gCamera.aperture = 40;
    gCamera.focalLength = INIT_APERTURE;
    gCamera.rotPoint = gOrigin;
	
    gCamera.viewPos.x = 0.0;
    gCamera.viewPos.y = 0.0;
    gCamera.viewPos.z = -gCamera.focalLength;
    gCamera.viewDir.x = -gCamera.viewPos.x; 
    gCamera.viewDir.y = -gCamera.viewPos.y; 
    gCamera.viewDir.z = -gCamera.viewPos.z;
	
    gCamera.viewUp.x = 0;  
    gCamera.viewUp.y = 1; 
    gCamera.viewUp.z = 0;

    diagonal = diagonal * 1.5;

    if (INIT_APERTURE < diagonal)
    	gCameraDolly (-diagonal);
    if (INIT_APERTURE > diagonal)
	gCameraDolly (-(diagonal - INIT_APERTURE));

    gOrigin.x = center[0];
    gOrigin.y = center[1];
    gOrigin.z = center[2];
}    

void Camera::SetLighting(unsigned int mode)
{
    float mat_specular[] = {1.0, 1.0, 1.0, 1.0};
    float mat_shininess[] = {90.0};
	
    // float position[4] = {7.0,-7.0,12.0,0.0};
    // float ambient[4]  = {0.2,0.2,0.2,1.0};
    // float diffuse[4]  = {1.0,1.0,1.0,1.0};
    // float specular[4] = {1.0,1.0,1.0,1.0};
    float position[4] = {7.0, -7.0,  12.0, 0.0};
    float ambient[4]  = {0.1,  0.1,  0.1,  1.0};
    float diffuse[4]  = {0.9,  0.9,  0.9,  1.0};
    float specular[4] = {0.4,  0.4,  0.4,  1.0};
	
    glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv (GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);

    switch (mode)
    {
    case 0:
	break;
    case 1:
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_FALSE);
	break;
    case 2:
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
	break;
    case 3:
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_FALSE);
	break;
    case 4:
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
	break;
    }
	
    glLightfv(GL_LIGHT0,GL_POSITION,position);
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
    glEnable(GL_LIGHT0);
}

void Camera::drawGLString(float x, float y, char *string)
{
    int len, i;
	
    glRasterPos2f(x, y);
    len = (int) strlen(string);
    for (i = 0; i < len; i++)
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, string[i]);
}

void Camera::drawGLText (int window_width, int window_height)
{
    char outString [256] = "";
    int matrixMode;
    int vp[4];
    int lineSpacing = 13;
    int line = 0;
    int startOffest = 7;
	
    // save and reset viewport
    glGetIntegerv(GL_VIEWPORT, vp);
    glViewport(0, 0, window_width, window_height);
	// save and reset matrix mode
    glGetIntegerv(GL_MATRIX_MODE, &matrixMode);
    // save and reset projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
	// save and reset modelview
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glScalef(2.0f / window_width, -2.0f / window_height, 1.0f);
    glTranslatef(-window_width / 2.0f, -window_height / 2.0f, 0.0f);
	
    // draw 
    glDisable(GL_LIGHTING);
    glColor3f (0.0, 0.0, 0.0);
    if (gShowInfo)
    {
	sprintf (outString, "Camera Position: (%0.1f, %0.1f, %0.1f)",
		 gCamera.viewPos.x, gCamera.viewPos.y, gCamera.viewPos.z);
	drawGLString (window_width - 10 - (strlen(outString) * 8),
		      window_height - (lineSpacing * line++) - startOffest,
		      outString);

	sprintf (outString, "Trackball Rotation: (%0.1f, %0.2f, %0.2f, %0.2f)",
		 gTrackBallRotation[0], gTrackBallRotation[1],
		 gTrackBallRotation[2], gTrackBallRotation[3]);
	drawGLString (window_width - 10 - (strlen(outString) * 8),
		      window_height - (lineSpacing * line++) - startOffest,
		      outString);

	sprintf (outString, "World Rotation: (%0.1f, %0.2f, %0.2f, %0.2f)",
		 gWorldRotation[0], gWorldRotation[1],
		 gWorldRotation[2], gWorldRotation[3]);
	drawGLString (window_width - 10 - (strlen(outString) * 8),
		      window_height - (lineSpacing * line++) - startOffest,
		      outString);

	// sprintf (outString, "Aperture: %0.1f", gCamera.aperture);
	// drawGLString (window_width - 10 - (strlen(outString) * 8),
	// 	      window_height - (lineSpacing * line++) - startOffest,
	// 	      outString);

	// sprintf (outString, "Focus Distance: %0.1f", gCamera.focalLength);
	// drawGLString (window_width - 10 - (strlen(outString) * 8),
	// 	      window_height - (lineSpacing * line++) - startOffest,
	// 	      outString);
    }
	
    if (gShowHelp)
    {
	line = 1;

	// sprintf (outString, "Controls:");
	// drawGLString (window_width - 10 - (strlen(outString) * 8),
	// 	      (lineSpacing * line++) + startOffest, outString);

	sprintf (outString, "left button drag: rotate camera");
	drawGLString (window_width - 10 - (strlen(outString) * 8),
		      (lineSpacing * line++) + startOffest, outString);

	sprintf (outString, "crtl-left button drag: zoom camera");
	drawGLString (window_width - 10 - (strlen(outString) * 8),
		      (lineSpacing * line++) + startOffest, outString);

	sprintf (outString, "ctrl-shift-left button drag: pan camera");
	drawGLString (window_width - 10 - (strlen(outString) * 8),
		      (lineSpacing * line++) + startOffest, outString);

	sprintf (outString, "arrows: aperture & focal length");
	drawGLString (window_width - 10 - (strlen(outString) * 8),
		      (lineSpacing * line++) + startOffest, outString);

	// sprintf (outString, "H: toggle help");
	// drawGLString (window_width - 10 - (strlen(outString) * 8),
	// 	      (lineSpacing * line++) + startOffest, outString);

	// sprintf (outString, "I: toggle info");
	// drawGLString (window_width - 10 - (strlen(outString) * 8),
	// 	      (lineSpacing * line++) + startOffest, outString);

	// sprintf (outString, "W: toggle wire frame");
	// drawGLString (window_width - 10 - (strlen(outString) * 8),
	// 	      (lineSpacing * line++) + startOffest, outString);
    }
	
    // restore viewing parameters
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(matrixMode);	
    glViewport(vp[0], vp[1], vp[2], vp[3]);
}

// GG RESHAPE?
void Camera::reshape (int w, int h)
{
    gCamera.screenWidth = w;
    gCamera.screenHeight = h;
}
    
void Camera::display_begin ()
{
    double xmin, xmax, ymin, ymax;
    // far frustum plane
    double zFar = -gCamera.viewPos.z + gShapeSize * 0.5;
    // near frustum plane clamped at 1.0
    double zNear = MIN (-gCamera.viewPos.z - gShapeSize * 0.5, 1.0);
    // GG HACK!
    zNear = 0.1;
    zFar = 1000.0; //zNear + gShapeSize * 2.0;
    // window aspect ratio
    double aspect = gCamera.screenWidth / (double)gCamera.screenHeight; 

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    if (aspect > 1.0) {
	ymax = zNear * tan (gCamera.aperture * 0.5 * DTOR);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;
    } else {
	xmax = zNear * tan (gCamera.aperture * 0.5 * DTOR);
	xmin = -xmax;
	ymin = xmin / aspect;
	ymax = xmax / aspect;
    }
    glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
	
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    gluLookAt (gCamera.viewPos.x, gCamera.viewPos.y, gCamera.viewPos.z,
	       gCamera.viewPos.x + gCamera.viewDir.x,
	       gCamera.viewPos.y + gCamera.viewDir.y,
	       gCamera.viewPos.z + gCamera.viewDir.z,
	       gCamera.viewUp.x, gCamera.viewUp.y ,gCamera.viewUp.z);
			
    // track ball rotation
    glRotatef (gTrackBallRotation[0], gTrackBallRotation[1],
	       gTrackBallRotation[2], gTrackBallRotation[3]);
    glRotatef (gWorldRotation[0], gWorldRotation[1],
	       gWorldRotation[2], gWorldRotation[3]);
}

void Camera::display_end ()
{	
    drawGLText (gCamera.screenWidth, gCamera.screenHeight);
}

void Camera::special(int key, int px, int py)
{
    (void) px; (void) py;
    gLastKey = key;
    switch (key)
    {
    case GLUT_KEY_UP: // arrow forward, close in on world
	gCamera.focalLength -= 0.5f;
	if (gCamera.focalLength < 0.0f)
	    gCamera.focalLength = 0.0f;
	glutPostRedisplay();
	break;
    case GLUT_KEY_DOWN: // arrow back, back away from world
	gCamera.focalLength += 0.5f;
	glutPostRedisplay();
	break;
    case GLUT_KEY_LEFT: // arrow left, smaller aperture
	gCamera.aperture -= 0.5f;
	if (gCamera.aperture < 0.0f)
	    gCamera.aperture = 0.0f;
	glutPostRedisplay();
	break;
    case GLUT_KEY_RIGHT: // arrow right, larger aperture
	gCamera.aperture += 0.5f;
	glutPostRedisplay();
	break;
    }
}

void Camera::gCameraDolly (float dolly)
{
	gCamera.focalLength += gCamera.focalLength / gCamera.viewPos.z * dolly; 

	if (gCamera.focalLength < 1.0)
	    gCamera.focalLength = 1.0;

	// gCamera.eyeSep = gCamera.focalLength * eyeRelative;
	gCamera.viewPos.z += dolly;

	if (gCamera.viewPos.z == 0.0) // do not let z = 0.0
	    gCamera.viewPos.z = 0.0001;
}

void Camera::mouse_move (int x, int y)
{
    if (gDolly)
    {
	float dolly = (gDollyPanStartPoint[1] - y) * -gCamera.viewPos.z / 200.0f;
	gCameraDolly (dolly);

	gDollyPanStartPoint[0] = x;
	gDollyPanStartPoint[1] = y;

	glutPostRedisplay();
    }

    if (gPan)
    {
	float panX = (gDollyPanStartPoint[0] - x) / (900.0f / -gCamera.viewPos.z);
	float panY = (gDollyPanStartPoint[1] - y) / (900.0f / -gCamera.viewPos.z);

	gCamera.viewPos.x -= panX;
	gCamera.viewPos.y -= panY;

	gDollyPanStartPoint[0] = x;
	gDollyPanStartPoint[1] = y;

	glutPostRedisplay();
    }

    if (gTrackBall)
    {
	rollToTrackball (x, y, gTrackBallRotation);
	glutPostRedisplay();
    }
}

static bool mouse_press_track (int button)
{
    return button == GLUT_LEFT_BUTTON &&
	!(glutGetModifiers() & GLUT_ACTIVE_CTRL);
}
static bool mouse_press_zoom (int button)
{
    return button == GLUT_LEFT_BUTTON &&
	(glutGetModifiers() & GLUT_ACTIVE_CTRL) &&
	!(glutGetModifiers() & GLUT_ACTIVE_SHIFT);
}
static bool mouse_press_pan (int button)
{
    return (button == GLUT_LEFT_BUTTON) &&
	(glutGetModifiers() & GLUT_ACTIVE_CTRL) &&
	(glutGetModifiers() & GLUT_ACTIVE_SHIFT);
}

void Camera::mouse_end_dolly ()
{
    if (gDolly) 
    {
	// mouse_move (x, y);
	gDolly = false;
	gTrackBallRotation [0] = gTrackBallRotation [1] =
	    gTrackBallRotation [2] = gTrackBallRotation [3] = 0.0f;
    }
}

void Camera::mouse_end_pan ()
{
    if (gPan)
    {
	// mouse_move (x, y);
	gPan = false;
	gTrackBallRotation [0] = gTrackBallRotation [1] =
	    gTrackBallRotation [2] = gTrackBallRotation [3] = 0.0f;
    }
}

void Camera::mouse_end_track (int x, int y)
{
    if (gTrackBall)
    {
	gTrackBall = false;
	rollToTrackball (x, y, gTrackBallRotation);

	if (gTrackBallRotation[0] != 0.0)
	    addToRotationTrackball (gTrackBallRotation, gWorldRotation);

	gTrackBallRotation [0] = gTrackBallRotation [1] =
	    gTrackBallRotation [2] = gTrackBallRotation [3] = 0.0f;
    }
}

void Camera::mouse (int button, int state, int x, int y)
{
    if (state == GLUT_UP)
    {
	mouse_end_track (x, y);
	mouse_end_dolly();
	mouse_end_pan();
    }

    if (state == GLUT_DOWN && mouse_press_track (button))
    {
	mouse_end_dolly();
	mouse_end_pan();

	startTrackball (x, y, 0, 0, gCamera.screenWidth, gCamera.screenHeight);
	gTrackBall = true;
    }

    if (state == GLUT_DOWN && mouse_press_zoom (button))
    {
	mouse_end_track (x, y);
	mouse_end_pan ();

	gDollyPanStartPoint[0] = x;
	gDollyPanStartPoint[1] = y;
	gDolly = true;
    }

    if (state == GLUT_DOWN && mouse_press_pan (button))
    {
	mouse_end_track (x, y);
	mouse_end_dolly();

	gDollyPanStartPoint[0] = x;
	gDollyPanStartPoint[1] = y;
	gPan = true;
    }


    // else if (state == GLUT_UP && mouse_press_track (button))
    // {
    // 	gTrackBall = false;

    // 	if (gTrackBallRotation[0] != 0.0)
    // 	    addToRotationTrackball (gTrackBallRotation, gWorldRotation);

    // 	gTrackBallRotation [0] = gTrackBallRotation [1] =
    // 	    gTrackBallRotation [2] = gTrackBallRotation [3] = 0.0f;
    // }

    else if (state == GLUT_UP && mouse_press_zoom (button))
    {
	mouse_move (x, y);
	gDolly = false;
	gTrackBallRotation [0] = gTrackBallRotation [1] =
	    gTrackBallRotation [2] = gTrackBallRotation [3] = 0.0f;
    }
    else if (state == GLUT_UP && mouse_press_pan (button))
    {
	mouse_move (x, y);
	gPan = false;
	gTrackBallRotation [0] = gTrackBallRotation [1] =
	    gTrackBallRotation [2] = gTrackBallRotation [3] = 0.0f;
    }
}

void Camera::key(unsigned char inkey, int px, int py)
{
    (void) px; (void) py;
    gLastKey = inkey;
    switch (inkey)
    {
    case 27:
	exit(0);
	break;
    case 'h': // help
    case 'H':
	gShowHelp =  1 - gShowHelp;
	glutPostRedisplay();
	break;
    case 'i': // info
    case 'I':
	gShowInfo =  1 - gShowInfo;
	glutPostRedisplay();
	break;
    case 'w': // toggle wire
    case 'W':
	gLines = 1 - gLines;
	gPolygons = 1 - gPolygons;
	glutPostRedisplay();
	break;
    }
}

