#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>

#define DOMAIN_PARAMETER_SPACE 1.0

#ifndef _IERROR_CLASS
#define _IERROR_CLASS

class IError
{
public:
	virtual ~IError() {};
	virtual int getPolychordWithMaxError() = 0;
};

#endif