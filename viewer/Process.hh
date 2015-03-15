#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#define ON 1
#define OFF 0

#define PARAMETER_SPACE 1

#include "Mesh.hh"
#include "Cage.hh"
#include "Polychords.hh"

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#ifndef _PROCESS_CLASS
#define _PROCESS_CLASS

class Process
{
public:
	Process(){};
	~Process(){};
	void read(char *str);
	void readOFF (char* meshfile);
	void readPLY (char* meshfile);

	
	double normalizeDistance(double d, double min, double max);
	void distancesBetweenMeshCage();
	double computeDistance(Vector3d v, Vector3d v_map);

	void raffinementQuadLayout();

	VectorXd computeErrorPolychords();
private:
	void updateTQ();
	double computeErrorsGrid(int q, int r = 5, int c = 5);
	double computeErrorSample(int q, Vector2d s);
	Vector3i findTriangle(int q, Vector2d s);
	bool isInside(Vector2d P0, Vector2d P1, Vector2d s);
	double areaTriangle(Vector2d A, Vector2d B, Vector2d C);

public:
	Mesh M;
	Cage C;
	Polychords P;
    // #dist.Vx1: in the ith row, stores the distance of the vertices of the ith vertex
    VectorXd distancesMeshCage;
    vector<vector<int>> TQ;
};

#endif