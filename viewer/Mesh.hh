#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#define ON 1
#define OFF 0

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

using namespace Eigen;
using namespace std;


#ifndef _MESH_CLASS
#define _MESH_CLASS

#ifndef MIN
	#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
	#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

class Mesh
{
public:
	Mesh();
	~Mesh();
	#define VF_DRAW_MODE_NUM 4 
		typedef enum {SMOOTH, FLAT, POINTS, WIRE} draw_mode_t;

	void read(char *str);
	void readOFF (char* meshfile);
	void readPLY (char* meshfile);

	
	void bb();
	void drawCage (draw_mode_t mode);
	void drawMesh (draw_mode_t mode, double thresholdMin, double thresholdMax);
	void distanceBetweenMeshCage();
	// Variable 
	double center[3];
    double diagonal;
    void debug();
private:
	double computeDistanceCageMesh(int i);
	double normalizeDistance(double d, double min, double max);
	void setColorError(double d, double *color);
    // #Vx3: Stores the vertex coordinates, one vertex per row
    MatrixXd CageV;
    // #Fx3[4]: in the ith row, stores the indices of the vertices of the ith face
    MatrixXi CageF;
    // #Vx3: Stores the vertex coordinates, one vertex per row
    MatrixXd MeshV; // Vx3
    MatrixXd MeshParV; // Vx2

    // #param.Fx3[4]: in the ith row, stores the indices of the vertices of the ith face
    MatrixXi MeshF; // Fx3
    // in the ith row, stores "quality" param of ith vertex
    VectorXi MeshParF; 

    // #dist.Vx1: in the ith row, stores the distance of the vertices of the ith vertex
    VectorXd distV;

    //debug
public:
    int faceInMinPoint = -1;
    int faceInMaxPoint = -1;
    int onlyFace = -1;
    int cagenumF = 0;
};
#endif