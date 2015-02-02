#include "patch.hh"
#include <iostream>


Patch::Patch(MatrixXd & Vpatch, MatrixXi &Fpatch, MatrixXd &Vfixed, int nFixed, int nFree)
{
    V = Vpatch;
    F = Fpatch;
    Vcoord = Vfixed;
    handleV = nFixed;
    freeV = nFree;
}

MatrixXd Patch::computeHarmonicMap()
{
    cout << "Compute Harmonic Map " << endl;

    int n = freeV + handleV;          // total vertices

    Lrr = SparseMatrixXd(freeV, freeV);
    Lrk = SparseMatrixXd(freeV, handleV);

    SparseMatrixXd Mcopy;
    SparseMatrixXd Mrr = SparseMatrixXd(freeV, freeV);

    VectorXd bu(freeV), bv(freeV);       // known term of system
    VectorXd vku(handleV), vkv(handleV);    // coords of fixed vertices
    VectorXd vru(freeV), vrv(freeV);    // coords of unknown vertices

    igl::cotmatrix(V,F,L);
    igl::massmatrix(V,F,igl::MASSMATRIX_VORONOI,M);

    // salvo M
    Mcopy = M;

    // extrac il blocco rr da M (alto a sin)
    extractBlock(Mcopy,0,0,freeV,freeV,Mrr);

    invertDiag(M);

    L = M*L;

    extractBlock(L,0,0,freeV,freeV,Lrr);
    extractBlock(L,0,freeV,freeV,handleV,Lrk);

    // fill in coords of fixed vertices
    for (unsigned i=freeV; i<n; i++)
    {
        vku(i-freeV) = Vcoord(i,0);
        vkv(i-freeV) = Vcoord(i,1);
    }

    Lrr = Mrr*Lrr;
    Lrk = Mrr*Lrk;

    // build known term
    bu = - Lrk * vku;
    bv = - Lrk * vkv;

    // solve system
    solver.compute(Lrr);
    if (solver.info()!=Eigen::Success) {cout << "Decomposition failed!" << endl; return Vcoord;}

    vru = solver.solve(bu);
    if (solver.info()!=Eigen::Success) {cout << "Solving u failed!" << endl;  return Vcoord ;}

    vrv = solver.solve(bv);
    if (solver.info()!=Eigen::Success) {cout << "Solving v failed!" << endl; return Vcoord;}

    // build vector of internal vertices
    for (int i=0; i<freeV; i++)
    {
        Vcoord(i,0) = vru(i);
        Vcoord(i,1) = vrv(i);
    }
//    cout << "Risultato calcolato" << endl;
    return Vcoord;
}

void Patch::extractBlock(SparseMatrixXd & mat, int r0, int c0, int r, int c, SparseMatrixXd & m1)
{
    std::vector<Eigen::Triplet<double> > tripletList;
    for (int k=c0; k<c0+c; ++k)
        for (SparseMatrixXd::InnerIterator it(mat,k); it; ++it)
        {
            if (it.row()>=r0 && it.row()<r0+r)
                tripletList.push_back(Eigen::Triplet<double>(it.row()-r0,it.col()-c0,it.value()));
        }
    m1.setFromTriplets(tripletList.begin(), tripletList.end());
}

void Patch::invertDiag(SparseMatrixXd & mat)
{
    SparseMatrixXd m1(mat.rows(),mat.cols());
    mat.reserve(VectorXi::Constant(mat.cols(),1));
    for (int k=0; k<mat.rows(); ++k)
        for (SparseMatrixXd::InnerIterator it(mat,k); it; ++it)
        {
                m1.insert(it.row(), it.col()) = 1.0/it.value();
        }
    m1.makeCompressed();
    mat = m1;
}





