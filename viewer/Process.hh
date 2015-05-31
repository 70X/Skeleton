#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#define ON 1
#define OFF 0

#include "Utility.hh"
#include "Mesh.hh"
#include "Cage.hh"
#include "CageSubDomain.hh"
#include "Polychords.hh"
#include "IError.hh"

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
	Process(){};
	~Process(){};
	void read(char *str);
	void readOFF (char* meshfile);
	void readPLY (char* meshfile);

	void initAll(char* filename);
	void initErrorsAndRelations();
	
private:
	void distancesBetweenMeshCage();
	
	void updateTQ();
public:
	void getTrianglesInExpMapping(int Vi, CageSubDomain &sC);
	void raffinementQuadLayout(int times = 1);
	double computeErrorFromListTriangle(vector<int> triangles, Cage &domain, Vector2d examVertex, Vector3d smap);
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
	IError *E;
    // #dist.Vx1: in the ith row, stores the distance of the vertices of the ith vertex
    VectorXd distancesMeshCage;

    vector<int> debugTsQ;
    vector<vector<int>> TQ;
    // -- ONLY FOR DEBUG
    vector<map<Vector2d, vector<int>,  Utility::classcomp> > storeSampleTriangles;
    vector<Vector2d> orphanSample;
    
    int IDPolychord = -1;
    map<int, CageSubDomain> storeSubC;
    map<int, vector<int>> debugPartialTQ;

};

#endif