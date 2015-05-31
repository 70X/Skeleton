#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>

#define DOMAIN_PARAMETER_SPACE 1.0

using namespace Eigen;
using namespace std;

#ifndef _CAGE_CLASS
#define _CAGE_CLASS
class Cage
{
public:
	Cage(){};
	~Cage(){};
	// #Vx3: Stores the vertex coordinates, one vertex per row
    MatrixXd V;
    // #Fx3[4]: in the ith row, stores the indices of the vertices of the ith face
    MatrixXi Q;
    MatrixXd Vmesh; // Vx2
    VectorXi QVmesh;
    // relation adjacents of Cage
    MatrixXi QQ;
    VectorXi _QV;
    
    void setPartialQV(VectorXi _QV) { this->_QV = _QV;      };
    void setQ(MatrixXi Q)           { this->Q = Q;          };
    void setV(MatrixXd V)           { this->V = V;          };
    void setQQ(MatrixXi QQ)         { this->QQ = QQ;        };
    void setVmesh(MatrixXd Vmesh)   { this->Vmesh = Vmesh;  };
    void setQVmesh(MatrixXi QVmesh)  { this->QVmesh = QVmesh;};


    Vector3d getV(int Vi);
    Vector3d getVMapping(int q, Vector2d p);
    virtual MatrixXd getTMapping(Vector3i ABC);

    void computeQQ();
    bool isManifold();

    int findV(Vector3d v);
    int appendV(Vector3d v, int q);
    int appendQ(Vector4i q);

    vector<int> getVmeshQ(int q);
    vector<int> getVV(int v);
    vector<int> getQV(int v);
    
    // return edge of the quad d from the quad s
    int getEdgeQuadAdjacent(int s, int d);

    // Split q along half two edge
    //
    // q = id quad
    // e0, e1 = [0,1,2,3] edge positions in Q
    // collectV = collection of new Vertices
    //
    // return update of collectV
    void split(int q, int e0, int e1, vector<int> &collectV);

    int getAreaQuad(int q, Vector2d s);
    double bb();
private:
	void updateQVmesh(int q, int q_new, int to_axis);
public:
    void printV(int Vi)
    {
        cout << "V["<<Vi<<"]"<< V(Vi,0) <<","<< V(Vi, 1) <<","<< V(Vi, 2) <<endl;
    }
    bool QQ_QV_flag = false;
};
#endif