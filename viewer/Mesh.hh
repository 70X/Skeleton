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


class Mesh
{
public:
	Mesh();
	~Mesh();
	

	void read(char *str);
	void readOFF (char* meshfile);
	void readPLY (char* meshfile);

	MatrixXd getMeshV(){ return MeshV; };
	MatrixXd getCageV(){ return CageV; };

	MatrixXi getMeshF(){ return MeshF; };
	MatrixXi getCageF(){ return CageF; };

	VectorXi getMeshParToFaceCage(){ return MeshParF; };
	VectorXd getDistBetweenVVmap(){ return distV; };
	// Variable 

	void distanceBetweenMeshCage();
    void debug();
private:
	double computeDistanceCageMesh(int i);
	double normalizeDistance(double d, double min, double max);
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
    MatrixXd mapV;
};
#endif