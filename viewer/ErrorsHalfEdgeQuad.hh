#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>
#include <map>
#include <math.h>

#include "ErrorsGrid.hh"
#include "Utility.hh"
#include "Cage.hh"
#include "Mesh.hh"
#include "CageSubDomain.hh"
#include "Process.hh"

#define DOMAIN_PARAMETER_SPACE 1.0

using namespace Eigen;
using namespace std;

#ifndef _ERRORSHEQ_CLASS
#define _ERRORSHEQ_CLASS

class ErrorsHalfEdgeQuad : public ErrorsGrid
{
	map<pair<int, int>, double> errorQuadsByDirection; // key: <idQ, edge> value: error
public:
	ErrorsHalfEdgeQuad(Process &process)
	{
		Env = &process;
		M = &(Env->M);
		C = &(Env->C);
		computeErrorsGrid();
	};

	virtual int getPolychordWithMaxError();
private:
	virtual void computeErrorsGrid();
	void errorsGridByQuadID(int q);
	double errorsQuadAlongDirection(int q, double step_x, double step_y, int m, int n);
};

#endif