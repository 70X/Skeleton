#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif
#include "Process.hh"

#include <Eigen/Core>
//#include <Eigen/Geometry>
#include <iostream>
//#include <fstream>
//#include <vector>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

using namespace Eigen;
using namespace std;


#ifndef _DRAWMESH_CLASS
#define _DRAWMESH_CLASS

class DrawMesh
{
public:
	DrawMesh();
	~DrawMesh();
	#define MESH_DRAW_MODE_NUM 4 
	    typedef enum {SMOOTH, FLAT, POINTS, WIRE} draw_mode_t;

	void bb(MatrixXd MeshV, MatrixXi MeshF);

	void drawCage (draw_mode_t mode);
	void drawMesh (draw_mode_t mode);
	void drawGrid ();
	void drawLinesVmapping();
	void drawSamplePointWithHisTriangle(int q, Vector2d s, int t);
	double* getCenter();
	double getDiagonal();
	void setProcess(Process &p){ this->p = &p; };
	// set command from GUI
	double thresholdMin = 0;
	double thresholdMax = 1;
	int onlyFace = 5;
	bool showGrid = true;
	int IDPolychord = -1;
private:
	void setColorError(double d, double *color);
	double center[3];
    double diagonal;

    bool initObject = false;
    Process *p;

};
#endif