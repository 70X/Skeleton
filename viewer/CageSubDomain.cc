#include "CageSubDomain.hh"

void CageSubDomain::initDomain(int Vi)
{
    vector<int> oneRingVi = getVV(Vi);
    sQ = getQV(Vi); 
    cout << sQ.size() << endl;
    sV = expMapping(Vi, oneRingVi);

    print_sV(Vi, oneRingVi);
};

double CageSubDomain::angleBetweenTwoV(Vector3d Vj0, Vector3d Vj1, Vector3d Vi)
{
    Vector3d v = (Vj0 - Vi) / (Vj0 - Vi).norm();
    Vector3d w = (Vj1 - Vi) / (Vj1 - Vi).norm();
    cout << "Versor: "<<v(0)<<","<<v(1)<<","<<v(2)<<endl;
    cout << "Versor: "<<w(0)<<","<<w(1)<<","<<w(2)<<endl;
    double nV = v.norm();
    double nW = w.norm();

    double angle =  acos(
                             v.dot(w) /
                            (nV * nW)
                        );

    Vector3d signV = v.cross(w);
    return (signV(2) < 0) ? -angle : angle;
}

vector<double> CageSubDomain::getAnglesRoundV(int Vi, vector<int> oneRingVi)
{
    vector<double> Tj;
	vector<int>::const_iterator Vj;
    for(Vj = oneRingVi.begin(); (Vj+1) != oneRingVi.end(); ++Vj)
    {
        cout<<"Vj :"<<*Vj<<" Vj+1 :"<<*(Vj+1)<<" Vi :"<< Vi<<endl;
        Tj.push_back( angleBetweenTwoV( getV(*Vj), getV(*(Vj+1)), getV(Vi) ) );
    }
    Tj.push_back( angleBetweenTwoV( getV(*Vj), getV( oneRingVi[0] ), getV(Vi) ) );
    return Tj;
}

double CageSubDomain::sumAngles(vector<double> Tj)
{
    double S = 0;
    for(vector<double>::const_iterator a = Tj.begin(); a != Tj.end(); ++a)
        S += abs(*a);

    return S;
}

map<int, Vector2d> CageSubDomain::expMapping(int Vi, vector<int> oneRingVi)
{
    map<int, Vector2d> E;
    vector<double> Tj = getAnglesRoundV(Vi, oneRingVi);
    double A = 2 * M_PI / sumAngles(Tj);

    E.insert(std::make_pair(Vi, Vector2d(0,0)) );
    for (int i=0; i<oneRingVi.size(); i++)
    {
        int Vj = oneRingVi[i];
        double R = Utility::computeDistance(getV(Vi), getV(Vj));

        E.insert(std::make_pair(Vj, Vector2d( 	
        										R * ( cos( Tj[i]*A ) ),  
        										R * ( sin( Tj[i]*A ) )
        									)
								)
        		);
        printV(Vi);
        printV(Vj);
        // Vi=8 e Vj=10 angle 36.207 in rad 0.632
        cout <<"R : "<< R << " -> "<<Tj[i] << " * "<<A<<"=" << Tj[i]*A << endl;
        cout <<"("<<R * ( cos( Tj[i]*A ) )<<","<<R * ( sin( Tj[i]*A ) )<<")"<<endl<<endl;
    }

    /*
    E(Vi) = 0;
    E(Vj) = R * exp(i * Tj * A)

    con 

    R = distanza tra Vi e Vj
    Tj l’angolo tra i versori (Vj - Vi)/||Vj - Vi|| e (Vj+1 - Vi)/||Vj+1 - Vi||
    A = 2 * PI / sommatoria Tj, j=0..n
    */
    return E;
}



Vector2d CageSubDomain::getVMapping(int q, Vector2d p)
{
    // check if exists in domain q
    assert( find(sQ.begin(), sQ.end(), q) != sQ.end()  && "Error: the quad's id doesn't exist in CageSubDomain::Q");
    Vector4i quad = Q.row(q);
    Vector2d    A = sV[ quad[0] ],
                B = sV[ quad[1] ],
                C = sV[ quad[2] ],
                D = sV[ quad[3] ];
    double u = p(0);
    double v = p(1);
    Vector2d P = (A*(1-u)+B*u)*(1-v) + (D*(1-u)+C*u)*v;
    if (false && quad[0] == 8 && quad[1] == 10 && quad[2] == 5 && quad[3] == 1)
    {
        cout << "------------ QUAD getVMapping -------------- " <<endl;
        cout << quad << endl;
        cout << "--------------- P Vector2d ------------------- " << endl;
        cout << P << endl;
        cout << "--------------- END ------------------------- " << endl;
    }
    return P;
}

MatrixXd CageSubDomain::getTMapping(Vector3i ABC)
{
    MatrixXd T = MatrixXd(3, 2); // triangle mapping into new domain
    T << 	getVMapping(QVmesh(ABC(0)), Vmesh.row(ABC(0)) ),
    		getVMapping(QVmesh(ABC(1)), Vmesh.row(ABC(1)) ),
    		getVMapping(QVmesh(ABC(2)), Vmesh.row(ABC(2)) );
    //cout << T << endl;
    return T;
}