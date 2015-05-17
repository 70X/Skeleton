#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>
#include <bitset>

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
	  

      vector<int> findTrianglesDebug(vector<int> Ts, Vector2d V, Cage &C)
    {
        vector<int> listTriangleIDs;

        for(vector<int>::const_iterator idT = Ts.begin(); idT != Ts.end(); ++idT)
        {
            MatrixXd ABC = C.getTMapping(F.row(*idT)); //ritorna il triangolo mappato in C o in SubDomainC
            
            Vector2d _A = ABC.row(0);
            Vector2d _B = ABC.row(1);
            Vector2d _C = ABC.row(2);

            double T[3];
            T[0] = Utility::getLeft(_A, _B, V);
            T[1] = Utility::getLeft(_B, _C, V);
            T[2] = Utility::getLeft(_C, _A, V);

            bitset<3> confront;
            confront.set(0, Utility::isLeft(_A, _B, V));
            confront.set(1, Utility::isLeft(_B, _C, V));
            confront.set(2, Utility::isLeft(_C, _A, V));
            if (confront.count() >= 2)
            {
             for (std::size_t i=0; i<confront.size(); ++i)
                if (!confront[i] && T[i] > -0.0001)
                {
                    listTriangleIDs.push_back(*idT);
                    cout << T[i] << " -> "<<*idT << endl;
                }
            }

            /*
            {
                cout << Utility::getLeft(_A, _B, V) << endl;
                cout << Utility::getLeft(_B, _C, V) << endl;
                cout << Utility::getLeft(_C, _A, V) << endl<<endl;
            }
            if (Utility::is_inside(_A,_B,_C, V) )
            {
                listTriangleIDs.push_back(*idT);
            }*/
        }

        
        return listTriangleIDs;
    }
private:
    
    bool TTVTflag;  // true iff TV* and TT are up-to-date

};
#endif