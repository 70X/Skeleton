#include "CageSubDomain.hh"

void CageSubDomain::initDomain(int Vi)
{
    vector<int> oneRingVi = getVV(Vi);
    sQ = getQV(Vi); 
    expMapping(Vi, oneRingVi);
};

double CageSubDomain::angleBetweenTwoV(Vector3d Vj0, Vector3d Vj1, Vector3d Vi)
{
    Vector3d v = (Vj0 - Vi) / (Vj0 - Vi).norm();
    Vector3d w = (Vj1 - Vi) / (Vj1 - Vi).norm();
    double nV = v.norm();
    double nW = w.norm();

    double angle =  acos(
                             v.dot(w) /
                            (nV * nW)
                        );
    return angle;
}

vector<double> CageSubDomain::getAnglesRoundV(int Vi, vector<int> oneRingVi)
{
    vector<double> Tj;
	vector<int>::const_iterator Vj;
    double angle = 0;
    for(Vj = oneRingVi.begin(); (Vj+1) != oneRingVi.end(); ++Vj)
    {
        angle += angleBetweenTwoV( getV(*Vj), getV(*(Vj+1)), getV(Vi) );
        Tj.push_back(angle);
    }
    angle += angleBetweenTwoV( getV(*Vj), getV( oneRingVi[0] ), getV(Vi) );
    Tj.push_back( angle );
    return Tj;
}

double CageSubDomain::sumAngles(vector<double> Tj)
{
    return Tj[Tj.size()-1];
}

void CageSubDomain::expMapping(int Vi, vector<int> oneRingVi)
{
    vector<Vector2d> E;
    vector<double> Tj = getAnglesRoundV(Vi, oneRingVi);
    double A = 2 * M_PI / sumAngles(Tj);

    iV[Vi] = 0;
    E.push_back(Vector2d(0,0));
    for (int i=0; i<oneRingVi.size(); i++)
    {
        int Vj = oneRingVi[i];
        double R = Utility::computeDistance(getV(Vi), getV(Vj));

        //iV.insert(pair(Vj, i+1));
        iV[Vj] = i+1;
        E.push_back(Vector2d( 	
    							R * ( cos( Tj[i]*A ) ),  
    							R * ( sin( Tj[i]*A ) )
    						)
                   );
        //printV(Vi);
        //printV(Vj);
        // Vi=8 e Vj=10 angle 36.207 in rad 0.632
        //cout <<"R : "<< R << " -> "<<Tj[i] << " * "<<A<<"=" << Tj[i]*A << endl;
        //cout <<"("<<R * ( cos( Tj[i]*A ) )<<","<<R * ( sin( Tj[i]*A ) )<<")"<<endl<<endl;
    }
    sV = E;
}



Vector2d CageSubDomain::getVMapping(int q, Vector2d p)
{
    Vector4i quad = Q.row(q);
    Vector2d    A = sV[ iV.find(quad[0])->second ],
                B = sV[ iV.find(quad[1])->second ],
                C = sV[ iV.find(quad[2])->second ],
                D = sV[ iV.find(quad[3])->second ];
    double u = p(0);
    double v = p(1);
    Vector2d P = (A*(1-u)+B*u)*(1-v) + (D*(1-u)+C*u)*v;
    return P;
}

MatrixXd CageSubDomain::getTMapping(Vector3i ABC)
{
    MatrixXd T = MatrixXd(3, 2); // triangle mapping into new domain
    Vector2d A = getVMapping(QVmesh(ABC(0)), Vmesh.row(ABC(0)) );
    Vector2d B = getVMapping(QVmesh(ABC(1)), Vmesh.row(ABC(1)) );
    Vector2d C = getVMapping(QVmesh(ABC(2)), Vmesh.row(ABC(2)) );
    T.row(0) = A;
    T.row(1) = B;
    T.row(2) = C;
    return T;
}