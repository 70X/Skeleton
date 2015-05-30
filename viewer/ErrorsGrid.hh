#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>
#include <map>
#include <math.h>

#include "Utility.hh"
#include "Cage.hh"
#include "Mesh.hh"
#include "CageSubDomain.hh"


	//VectorXd errorPolychords();
	vector<vector<double>> errorPolychords();
#define GRID_SAMPLE 1

using namespace Eigen;
using namespace std;

class Process;

#ifndef _ERRORSGRID_CLASS
#define _ERRORSGRID_CLASS

class ErrorsGrid
{
private:
	Process *Env;
	Mesh *M;
	Cage *C;
	vector<Vector2d> orphanSample;
public:
	ErrorsGrid(Process &process)
	{
		Env = &process;
		M = &(Env->M);
		C = &(Env->C);
		Env->errorQuads = computeErrorsGrid();
	};

	VectorXd computeErrorsGrid();
	int getPolychordWithMaxError();
private:
	double errorsGridByQuadID(int q);
	double errorAvarageSamples( Vector2d s, double step_x, double step_y, map<Vector2d, double, Utility::classcomp> storeErrorSample);
	double errorSample(int q, Vector2d s);
	double computeErrorFromListTriangle(vector<int> triangles, Cage &domain, Vector2d examVertex, Vector3d smap);

	vector<vector<double>> errorPolychords();
};
#endif