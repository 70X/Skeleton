#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>
#include <map>
#include <math.h>

#include "IError.hh"
#include "Utility.hh"
#include "Cage.hh"
#include "Mesh.hh"
#include "CageSubDomain.hh"
#include "Process.hh"

#define DOMAIN_PARAMETER_SPACE 1.0

using namespace Eigen;
using namespace std;

#ifndef _ERRORSGRID_CLASS
#define _ERRORSGRID_CLASS

class ErrorsGrid : public IError
{
protected:
	Process *Env;
	vector<Vector2d> orphanSample;
    VectorXd errorQuads;
    vector<vector<int>> TQ;
public:
	ErrorsGrid(){};
	ErrorsGrid(Process *process)
	{
		Env = process;
		computeErrorsGrid();
	};

	virtual int getPolychordWithMaxError();
	virtual double getErrorpolychordByID(int idP);
	virtual void computeErrorsGrid();
	virtual void debug(Process *f) { Env = f;cout << f->P.getSize() <<" = "<<Env->P.getSize()<<endl; };
protected:
	void updateTQ();
	
	double errorsGridByQuadID(int q);
	double errorAvarageSamples( Vector2d s, double step_x, double step_y, map<Vector2d, double, Utility::classcomp> storeErrorSample);
	double errorSample(int q, Vector2d s);

	vector<pair<int, pair<int, int> > > errorPolychords();
};
#endif