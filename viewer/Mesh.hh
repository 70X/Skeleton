#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>

using namespace Eigen;
using namespace std;


#ifndef _MESH_CLASS
#define _MESH_CLASS


class Mesh
{
public:
	Mesh(){};
	~Mesh(){};
    
    // #Vx3: Stores the vertex coordinates, one vertex per row
    MatrixXd V; // Vx3
    // #param.Fx3[4]: in the ith row, stores the indices of the vertices of the ith face
    MatrixXi F; // Fx3

};
#endif