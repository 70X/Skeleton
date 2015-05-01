#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>
#include <map>
#include <math.h>
#include "Utility.hh"
#include "Cage.hh"

using namespace Eigen;
using namespace std;

#ifndef _SUBCAGE_CLASS
#define _SUBCAGE_CLASS

class CageSubDomain : public Cage
{
	public:
    map<int, Vector2d> sV;       // key: id vector concern Cage.V    | value: new mapping vertex
    vector<int> sQ;              // id quad into new domain
    CageSubDomain(){};
    ~CageSubDomain(){};

    void initDomain(int Vi);

    Vector2d getVMapping(int q, Vector2d p);
    MatrixXd getTMapping(Vector3i ABC);

private:
    map<int, Vector2d> expMapping(int Vi, vector<int> oneRingVi);

    vector<double> getAnglesRoundV(int Vi, vector<int> oneRingV);
    double   angleBetweenTwoV(Vector3d Vj0, Vector3d Vj1, Vector3d Vi);
    double   sumAngles(vector<double> Tj);

    void print_sV(int Vi, vector<int> oneRingVi)
    {
        cout << " DEBUG: "<<endl;
        cout << " sV["<<Vi<<"]\t ---> ["<<sV[Vi](0)<<","<<sV[Vi](1)<<"]"<<endl;
        for(vector<int>::const_iterator vj = oneRingVi.begin(); vj != oneRingVi.end(); ++vj)
        {
            cout << " sV["<<*vj<<"]\t---> ["<<sV[*vj](0)<<","<<sV[*vj](1)<<"]"<<endl;
        }
    };


};
#endif