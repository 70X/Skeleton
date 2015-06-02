#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif
#include "Process.hh"

#include <Eigen/Core>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MESH_DRAW_MODE_NUM 4 

using namespace Eigen;
using namespace std;


#ifndef _DRAWMESH_CLASS
#define _DRAWMESH_CLASS

class DrawMesh
{
public:
	DrawMesh();
	~DrawMesh();
	typedef enum {SMOOTH, FLAT, POINTS, WIRE} draw_mode_t;

	void bb(MatrixXd MeshV, MatrixXi MeshF);

	void drawCage (draw_mode_t mode);
	void drawMesh (draw_mode_t mode);
	void drawDebug (draw_mode_t mode);


	double* getCenter();
	double getDiagonal();
	void setProcess(Process &p){ this->p = &p; };
	
	// set command GUI
	double IDTriangle = -1;
	int IDQuad = -1;

	bool showMesh = true;
	bool showCage = true;
	bool showDebug = true;
	bool showGrid = false;
	bool showLines = false;
	int IDPolychord = -1;
	int IDCageSubDomain = -1;
	int IDPartialTriangle = -1;
private:
	//--------------------DEBUG----------------------
	void drawCageSubDomain();
	void drawPolychords();
	void drawGrid();
	void drawLinesVmapping();
	void drawSamplePointWithHisTriangle(int q, Vector2d s, int t);
	void drawTriangleAndShadow(int idT, bool shadow = true);
	//-----------------------------------------------

	void setColorError(double d, double *color);
	double center[3];
    double diagonal;

    bool initObject = false;
    Process *p;

};
#endif