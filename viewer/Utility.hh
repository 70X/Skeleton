#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

using namespace Eigen;
using namespace std;

namespace Utility
{
	typedef Eigen::MatrixXd Triangle2D;
	typedef Eigen::Matrix3d Triangle3D;

  	double areaTriangle(Vector2d A, Vector2d B, Vector2d C);
	double areaTriangle_old(Vector2d A, Vector2d B, Vector2d C);
	double areaQuad(Vector3d A,Vector3d B,Vector3d C, Vector3d D);
	double debug(Vector2d P0, Vector2d P1, Vector2d V);
	bool isLeft(Vector2d P0, Vector2d P1, Vector2d V);
	bool is_inside(Vector2d A, Vector2d B, Vector2d C, Vector2d V);
	Vector3d getCoordBarycentricTriangle(Triangle2D ABCMapping, Triangle3D ABC, Vector2d V);
	double normalizeDistance(double d, double min, double max);
	double computeDistance(Vector3d v, Vector3d v_map);
}