#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>
#include <map>
#include <math.h>
#include <vector>

class Process;
#define DOMAIN_PARAMETER_SPACE 1.0

#ifndef _IERROR_CLASS
#define _IERROR_CLASS

class IError
{
public:
	virtual ~IError() {};

	virtual std::map<std::pair<int, int>, double> getErrorsQuad() = 0;
	virtual std::map<int, double> getErrorPolychords() = 0;
	virtual int getPolychordWithMaxError() = 0;
	virtual double getErrorpolychordByID(int idP) = 0;
	virtual void computeErrorsGrid(std::vector<int> listQuad) = 0;
	virtual void debug(Process *f) = 0;
};

#endif