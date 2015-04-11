#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>

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
    MatrixXd Vpar; // Vx2
    VectorXi QVpar;
    // relation adjacents of Cage
    MatrixXi QQ;

    Vector3d getVMapping(int q, Vector2d p);
    void computeQQ();
    bool isManifold();

    int appendV(Vector3d v);
    int appendQ(Vector4i q);

    vector<int> getVparQ(int q);
    /*
     * return edge of the quad d from the quad s
     */
    int getEdgeQuadAdjacent(int s, int d);

    void split(int q, int e0, int e1);
private:
	void updateQV(int q, int q_new, int to_axis);
};
#endif