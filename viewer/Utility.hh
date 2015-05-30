#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

using namespace Eigen;
using namespace std;


#ifndef _UTILITY_H
#define _UTILITY_H

namespace Utility
{
	struct classcomp {
  		bool operator() (const Vector2d& v0, const Vector2d& v1) const
  		{return std::tie(v0(0), v0(1)) < std::tie(v1(0), v1(1));}
	};

	typedef Eigen::MatrixXd Triangle2D;
	typedef Eigen::Matrix3d Triangle3D;

  	double areaTriangle(Vector2d A, Vector2d B, Vector2d C);
	double areaTriangle_old(Vector2d A, Vector2d B, Vector2d C);
	double areaQuad(Vector3d A,Vector3d B,Vector3d C, Vector3d D);
	double getLeft(Vector2d P0, Vector2d P1, Vector2d V);
	bool isLeft(Vector2d P0, Vector2d P1, Vector2d V);
	bool is_inside(Vector2d A, Vector2d B, Vector2d C, Vector2d V);
	Vector3d getCoordBarycentricTriangle(Triangle2D ABCMapping, Triangle3D ABC, Vector2d V);
	double normalizeDistance(double d, double min, double max);
	double computeDistance(Vector3d v, Vector3d v_map);
}
#endif