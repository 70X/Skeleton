#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#define ON 1
#define OFF 0
#define ERROR_TYPE_NUM 3 
//#define __MODE_DEBUG

#include "Utility.hh"
#include "Mesh.hh"
#include "Cage.hh"
#include "CageSubDomain.hh"
#include "Polychords.hh"
#include "IError.hh"

#include <sys/stat.h>
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

class LastIteration
{
public:
    LastIteration(){};
    ~LastIteration(){};
    typedef enum {WITH_QUEUE, GRID_SIMPLE, GRID_HALFEDGE} error_t;

	double LastError = 0;
    vector<int> newQuads;
    vector<int> newVertices;
	int worstPolychord = -1;
	error_t error_mode;
	void clean()
	{
        LastError = 0;
        worstPolychord = -1;
        newQuads.clear();
        newVertices.clear();
	}
};

class Process
{
public:
	Process(){ };
	~Process(){ };
	typedef enum {WITH_QUEUE, GRID_SIMPLE, GRID_HALFEDGE} error_t;

	void read(char *str);
	void readOFF (char* meshfile);
	void readPLY (char* meshfile);

	void initAll(char* filename);
	void initErrorsAndRelations(Cage &C, Polychords &P);
	
	//TODO: When a triangle is not found for specific point map
    //      return with no action. It's temporarily wrong.
	void getTrianglesInExpMapping(int Vi, CageSubDomain &sC);
	void raffinementQuadLayout(int times = 1);
	int queueRaffinementQuadLayout(Cage &C, Polychords &P, int tryTimes = 2);
	double computeErrorFromListTriangle(vector<int> triangles, Cage &domain, Vector2d examVertex, Vector3d smap);
private:
	void distancesBetweenMeshCage();
	double distancesBetweenMeshCage(Cage C);
	void processToSplit(Cage &C, vector<int> listQ, vector<int> &newVertices, vector<int> &newQuads);
	
    /******************************************************/
    /*            moving cage towards mesh                */
    /******************************************************/
    void initSubDomain(CageSubDomain &sC);
	void movingVertexCageToMesh(vector<int> newVertices, Cage &C);
	vector<int> getBorderTrianglesSubDomainQ(vector<int> subQ);
	//------------------- OUTPUT FILE -----------------------/
	void configurationFileOutput(ofstream &seqPolychord, ofstream &timePolychord);
public:
	Mesh M;
	Cage C;
	Polychords P;
	IError *E;

	bool reopenFile = true;
    // #dist.Vx1: in the ith row, stores the distance of the vertices of the ith vertex
    VectorXd distancesMeshCage;
    vector<Vector2d> orphanSample;
	

	LastIteration info;
	// --- RAFFINEMENT GUI
	int QuadMax = -1;
    double ErrMax = 0;
	int numberOfRaff = -1;


	error_t error_type_choice = GRID_HALFEDGE;
	int raffinementTimes = 0;
	char filename[200];

    // -- ONLY FOR DEBUG - When is defined MODE_DEBUG
    vector<map<Vector2d, vector<int>,  Utility::classcomp> > storeSampleTriangles;
    map<int, CageSubDomain> storeSubC;
    map<int, vector<int>> debugPartialTQ;
};

#endif