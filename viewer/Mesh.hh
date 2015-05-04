#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>

#include "Utility.hh"
#include "Cage.hh"
#define BUFFER_VT_AND_VV 45

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
    MatrixXi TT;
    
    // #V: Stores partial Vertex-Triangle relation
    VectorXi VT;

    bool isManifold();
    void initTT();
    VectorXi getVT(int i);
    Matrix3d getT(int i);
    vector<int> findTriangles(vector<int> Ts, Vector2d V, Cage &C);
	  
private:
    
    bool TTVTflag;  // true iff TV* and TT are up-to-date

};
#endif