#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>

class Process;
#define DOMAIN_PARAMETER_SPACE 1.0

#ifndef _IERROR_CLASS
#define _IERROR_CLASS

class IError
{
public:
	virtual ~IError() {};
	virtual int getPolychordWithMaxError() = 0;
	virtual double getErrorpolychordByID(int idP) = 0;
	virtual void computeErrorsGrid() = 0;
	virtual void debug(Process *f) = 0;
};

#endif