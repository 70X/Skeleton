#include "CageSubDomain.hh"

void CageSubDomain::initDomain(int Vi)
{
    vector<int> oneRingVi = getVV(Vi);
    sQ = getQV(Vi); 
    sV = expMapping(Vi, oneRingVi);
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

vector< pair<int, Vector2d> > CageSubDomain::expMapping(int Vi, vector<int> oneRingVi)
{
    vector< pair<int, Vector2d> > E;
    vector<double> Tj = getAnglesRoundV(Vi, oneRingVi);
    double A = 2 * M_PI / sumAngles(Tj);

    E.push_back(std::make_pair(Vi, Vector2d(0,0)) );
    for (int i=0; i<oneRingVi.size(); i++)
    {
        int Vj = oneRingVi[i];
        double R = Utility::computeDistance(getV(Vi), getV(Vj));

        E.push_back(std::make_pair(Vj, Vector2d( 	
        										R * ( cos( Tj[i]*A ) ),  
        										R * ( sin( Tj[i]*A ) )
        									)
								)
                   );
        //printV(Vi);
        //printV(Vj);
        // Vi=8 e Vj=10 angle 36.207 in rad 0.632
        //cout <<"R : "<< R << " -> "<<Tj[i] << " * "<<A<<"=" << Tj[i]*A << endl;
        //cout <<"("<<R * ( cos( Tj[i]*A ) )<<","<<R * ( sin( Tj[i]*A ) )<<")"<<endl<<endl;
    }

    return E;
}



Vector2d CageSubDomain::getVMapping(int q, Vector2d p)
{
    // check if exists in domain q
    assert( find(sQ.begin(), sQ.end(), q) != sQ.end()  && "Error: the quad's id doesn't exist in CageSubDomain::Q");
    Vector4i quad = Q.row(q);
    Vector2d    A = sV[ quad[0] ].second,
                B = sV[ quad[1] ].second,
                C = sV[ quad[2] ].second,
                D = sV[ quad[3] ].second;
    double u = p(0);
    double v = p(1);
    Vector2d P = (A*(1-u)+B*u)*(1-v) + (D*(1-u)+C*u)*v;
    
    return P;
}

MatrixXd CageSubDomain::getTMapping(Vector3i ABC)
{
    MatrixXd T = MatrixXd(3, 2); // triangle mapping into new domain
    T << 	getVMapping(QVmesh(ABC(0)), Vmesh.row(ABC(0)) ),
    		getVMapping(QVmesh(ABC(1)), Vmesh.row(ABC(1)) ),
    		getVMapping(QVmesh(ABC(2)), Vmesh.row(ABC(2)) );
    return T;
}