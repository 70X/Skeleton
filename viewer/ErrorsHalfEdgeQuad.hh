#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>
#include <map>
#include <math.h>
#include <chrono>

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
	// key: <idQ, edge> value: error
	map<pair<int, int>, double> errorQuadsByDirection; 
	map<int, double> storeErrorPolychords;
public:
	ErrorsHalfEdgeQuad(Process *process, map<int, double> sEP, map<pair<int, int>, double>  eQBD)
	{
		Env = process;
		storeErrorPolychords = sEP;
		errorQuadsByDirection = eQBD;
	}

	ErrorsHalfEdgeQuad(Process *process)
	{
		Env = process;

		vector<int> listQuad(Env->C.Q.rows());
        std::iota(listQuad.begin(), listQuad.end(), 0);
		computeErrorsGrid(listQuad);  
		/*
		vector<int> listPolychord(Env->P.getSize());
        std::iota(listPolychord.begin(), listPolychord.end(), 0);
		errorPolychords(listPolychord);
		*/
	};
	virtual map<pair<int, int>, double> getErrorsQuad() { return errorQuadsByDirection; }
	virtual map<int, double> getErrorPolychords(){ return storeErrorPolychords; }
	
	virtual int getPolychordWithMaxError();
	virtual double getErrorpolychordByID(int idP);
	virtual void computeErrorsGrid(vector<int> listQuad);
	virtual void debug(Process *f) { Env = f; }
private:
	void errorsGridByQuadID(int q);
	double errorsQuadAlongDirection(int q, double step_x, double step_y, int m, int n);
	void errorPolychords(vector<int> listPolychord);
};

#endif