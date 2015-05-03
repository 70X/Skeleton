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
	   
    bool isLeft(Vector2d P0, Vector2d P1, Vector2d V)
  {
      Matrix2d m;
      m << P1(0)-P0(0), V(0)-P0(0),
           P1(1)-P0(1), V(1)-P0(1);
      //cout << m.determinant() << endl;
      return m.determinant() > 0;
  };
    vector<int> findTrianglesDebug(vector<int> Ts, Vector2d V, Cage &C)
    {
        vector<int> listTriangleIDs;

        for(vector<int>::const_iterator idT = Ts.begin(); idT != Ts.end(); ++idT)
        {
            MatrixXd ABC = C.getTMapping(F.row(*idT)); //ritorna il triangolo mappato in C o in SubDomainC
            
            Vector2d _A = ABC.row(0);
            Vector2d _B = ABC.row(1);
            Vector2d _C = ABC.row(2);
            if (isLeft(_A, _B, V) &&
                isLeft(_B, _C, V) &&
                isLeft(_C, _A, V))
            {
                listTriangleIDs.push_back(*idT);
            }
        }

        
        return listTriangleIDs;
    };
private:
    
    bool TTVTflag;  // true iff TV* and TT are up-to-date

};
#endif