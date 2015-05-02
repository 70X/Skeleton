#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#define ON 1
#define OFF 0

#define GRID_SAMPLE 1

#include "Utility.hh"
#include "Mesh.hh"
#include "Cage.hh"
#include "CageSubDomain.hh"
#include "Polychords.hh"

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>


#ifndef _PROCESS_CLASS
#define _PROCESS_CLASS

class Process
{
public:

    struct classcomp {
  		bool operator() (const Vector2d& v0, const Vector2d& v1) const
  		{return std::tie(v0(0), v0(1)) < std::tie(v1(0), v1(1));}
	};

	Process(){};
	~Process(){};
	void read(char *str);
	void readOFF (char* meshfile);
	void readPLY (char* meshfile);

	void initAll(char* filename);
	void initErrorsAndRelations();
	
private:
	void distancesBetweenMeshCage();
	

	VectorXd errorPolychords();
	void updateTQ();


    /******************************************************/
    /*            raffinement cage                        */
    /******************************************************/
	double errorsGrid(int q, int r = 5, int c = 5);
	double errorAvarageSamples(Vector2d s, double step_x, double step_y, map<Vector2d, double, classcomp> storeErrorSample);
	double errorSample(int q, Vector2d s);
public:
	void raffinementQuadLayout(int times = 1);
private:
	
    /******************************************************/
    /*            moving cage towards mesh                */
    /******************************************************/
    void initSubDomain(CageSubDomain &sC);
	void movingVertexCageToMesh(vector<int> newVertices);
	vector<int> getBorderTrianglesSubDomainQ(vector<int> subQ);
public:
	Mesh M;
	Cage C;
	Polychords P;
    // #dist.Vx1: in the ith row, stores the distance of the vertices of the ith vertex
    VectorXd distancesMeshCage;
    VectorXd errorQuads;

    vector<vector<int>> TQ;
    // -- ONLY FOR DEBUG
    vector<map<Vector2d, vector<int>, classcomp> > storeSampleTriangles;
    vector<Vector2d> orphanSample;
    MatrixXd mapV;

    map<int, CageSubDomain> storeSubC;
    vector<int> debugPartialTQ;

};

#endif