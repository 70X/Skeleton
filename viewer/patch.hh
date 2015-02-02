#ifndef PATCH_H
#define PATCH_H

#include "igl/cotmatrix.h"
#include "igl/massmatrix.h"
//#include "../cotmatrix_addins/min_quad_with_fixed.h"
#include <Eigen/SparseCholesky>
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <Eigen/Core>
#include "Eigentypes.h"

typedef Eigen::SparseMatrix<double> SparseMatrixXd;

using namespace Eigen;
using namespace std;

class Patch
{
public:

    // number of free vertices
    int freeV;
    // number of handle vertices
    int handleV;

    // #Vx3: Stores the vertex coordinates, one vertex per row
    MatrixXd V, Vcoord;

    // #Fx3[4]: in the ith row, stores the indices of the vertices of the ith face
    MatrixXi F;

    // #V: index attribute for vertices
    VectorXi IV;

    inline Patch(){}
    Patch(MatrixXd & Vpatch, MatrixXi & Fpatch, MatrixXd & Vfixed, int nFixed, int nFree);

    MatrixXd computeHarmonicMap();


private:

    // Numerics:

    SparseMatrixXd L;   // Laplacian matrix
    SparseMatrixXd M;   // Mass matrix
    SparseMatrixXd Lrr; // Upper left block of L
    SparseMatrixXd Lrk; // Upper right block of L
    Eigen::SimplicialCholesky<SparseMatrixXd> solver;

    void extractBlock(SparseMatrixXd & mat, int r0, int c0, int r, int c, SparseMatrixXd & m1);
    void invertDiag(SparseMatrixXd & mat);


};

#endif // PATCH_H
