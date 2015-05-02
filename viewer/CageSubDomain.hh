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
    // key: id vector concern Cage.V    | value: new mapping vertex
    vector< pair<int, Vector2d> > sV;
    // id quad into new domain
    vector<int> sQ;              
    CageSubDomain(){};
    ~CageSubDomain(){};

    void initDomain(int Vi);

    Vector2d getVMapping(int q, Vector2d p);
    MatrixXd getTMapping(Vector3i ABC);

private:
    vector< pair<int, Vector2d> > expMapping(int Vi, vector<int> oneRingVi);

    vector<double> getAnglesRoundV(int Vi, vector<int> oneRingV);
    double   angleBetweenTwoV(Vector3d Vj0, Vector3d Vj1, Vector3d Vi);
    double   sumAngles(vector<double> Tj);
public:
    void print_sV()
    {
        cout << " DEBUG: "<<endl;
        printV(sV.begin()->first);
        for(vector< pair<int, Vector2d> >::const_iterator vj = sV.begin(); vj != sV.end(); ++vj)
        {
            //cout << " sV["<<*vj<<"]\t---> ["<<sV[*vj](0)<<","<<sV[*vj](1)<<"]"<<endl;
            printV(vj->first);
        }
    };

    void printV(int V)
    {
        cout << " sV["<<V<<"]\t ---> ["<<sV[V].second(0)<<","<<sV[V].second(1)<<"]"<<endl;
    };

};
#endif