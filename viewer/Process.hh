#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#define ON 1
#define OFF 0


#include "Mesh.hh"
#include "Cage.hh"

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


	Mesh m;
	Cage c;
    // #dist.Vx1: in the ith row, stores the distance of the vertices of the ith vertex
    VectorXd distancesMeshCage;
};

#endif