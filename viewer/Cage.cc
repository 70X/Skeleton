#include "Cage.hh"

double Cage::bb ()
{

    MatrixXd V = this->V;
    VectorXd min, max;

    min = VectorXd::Constant(V.cols(),0.0);
    max = VectorXd::Constant(V.cols(),0.0);

    min = V.colwise().minCoeff();
    max = V.colwise().maxCoeff();
    
    return (max - min).norm();
}

int Cage::findV(Vector3d v)
{
    for (unsigned int j=0; j<V.rows(); j++)
    {
        if (v(0) == V(j,0) && v(1) == V(j,1)
            && v(2) == V(j,2))
        {
            return j;
        }
    }
    return -1;
}

int Cage::appendV(Vector3d v, int q)
{
    int j;
    if ( (j = findV(v) ) != -1)
    {
        //cout << "v already exists: "<<j<<endl;
        return j;
    }
    int i = V.rows();
    V.conservativeResize(i+1, 3);
    V.row(i) = v;

    _QV.conservativeResize(_QV.rows() + 1);
    _QV(i) = q;

    return i;
}
int Cage::appendQ(Vector4i q)
{
    int i  = Q.rows();
    Q.conservativeResize(i+1, 4);
    Q.row(i) = q;
    return i;
}

void Cage::split(int q, int e0, int e1, vector<int> &collectV)
{
    e0 = (e0 == 0 || e0 == 2) ? 0 : 1;
    e1 = (e0 == 0 || e0 == 2) ? 2 : 3;
    Vector4i relQ = Q.row(q);
    MatrixXd relV(4,3);
    relV.row(0) << V.row(relQ(0));
    relV.row(1) << V.row(relQ(1));
    relV.row(2) << V.row(relQ(2));
    relV.row(3) << V.row(relQ(3));

    Vector3d P0 = (relV.row(e0) + relV.row((e0+1)%4))/2;
    Vector3d P1 = (relV.row(e1) + relV.row((e1+1)%4))/2;

    int iP0 = appendV(P0, q);
    int iP1 = appendV(P1, q);

    if (find(collectV.begin(), collectV.end(), iP0) == collectV.end())
        collectV.push_back(iP0);

    if (find(collectV.begin(), collectV.end(), iP1) == collectV.end())
        collectV.push_back(iP1);

    //cout << " QUI " << _QV(iP0) << " /// " << _QV.size() << endl;
    int q_new;
    if (e0 == 0 || e0 == 2)
    {
        Q.row(q) =      Vector4i(relQ(0), iP0, iP1, relQ(3));
        q_new = appendQ(Vector4i(iP0, relQ(1), relQ(2), iP1));
    }
    else
    {
        Q.row(q) =      Vector4i(relQ(0), relQ(1), iP0, iP1)  ;
        q_new = appendQ(Vector4i(iP1, iP0, relQ(2), relQ(3)) );
    }
    //cout << q << " - " << q_new << " to " << e0 << endl;
    updateQVmesh(q, q_new, e0);

}


    int Cage::getAreaQuad(int q, Vector2d s)
    {
        Vector4i ABCD = Q.row(q);
        VectorXd min = VectorXd::Constant(Vmesh.cols(),0.0);
        VectorXd max = VectorXd::Constant(Vmesh.cols(),0.0);
        
        //min = Vmesh.colwise().minCoeff(); //(0, 0)
        max = Vmesh.colwise().maxCoeff(); //(1, 1)

        double x = s(0);
        double y = s(1);
        double Ox = min(0);     // x origin
        double Oy = min(1);     // y origin
        double Hx = max(0)/2.0; // x half space
        double Hy = max(1)/2.0; // y half space
        double Mx = max(0);     // x max space (point C of ABCD)
        double My = max(1);     // y max space (point C of ABCD)
 
        assert(x>=Ox && x<=Mx && y>=Oy && y<=My);
        
        if(x >= Ox && x <= Hx  && y >= Oy && y <= Hy) // area SW
            return ABCD[0];
        if(x > Hx && x <= Mx   && y >= Oy && y <= Hy) // area SE
            return ABCD[1];
        if(x > Hx && x <= Mx   && y > Hy  && y <= My) // area NE
            return ABCD[2];
        if(x >= Ox && x <= Hx  && y > Hy  && y <= My) // area NW
            return ABCD[3];

 
    }

vector<int> Cage::getVmeshQ(int q)
{
    vector<int> vertices;
    for(int i=0; i<Vmesh.rows(); i++)
    {
        if (QVmesh(i) == q)
            vertices.push_back(i);
    }
    return vertices;
}


void Cage::updateQVmesh(int q, int q_new, int to_axis)
{
    vector<int> VQ = getVmeshQ(q);
    int XY = (to_axis == 0 || to_axis == 2) ? 0 : 1;
     for(vector<int>::const_iterator i = VQ.begin(); i != VQ.end(); ++i)
    {
        if (Vmesh(*i, XY) > 0.5)
        {
            Vmesh(*i, XY) -= 0.5;
            QVmesh(*i) = q_new;
        }
        // update vertex into new smaller quad
        Vmesh(*i, XY) /= 0.5;
    }
}


Vector3d Cage::getVMapping(int q, Vector2d p)
{
    Vector4i quad = Q.row(q);
    Vector3d    A(V.row(quad[0])),
                B(V.row(quad[1])),
                C(V.row(quad[2])),
                D(V.row(quad[3]));

    double u = p(0);
    double v = p(1);
    Vector3d P = (A*(1-u)+B*u)*(1-v) + (D*(1-u)+C*u)*v;
    return P;
}

// check if the cage is edge-manifold
    bool Cage::isManifold()
    {
        vector<vector<int> > QQT;
        for(int q=0;q<Q.rows();++q)
            for (int i=0;i<4;++i)
            {
                // v1 v2 q ei 
                int v1 = Q(q,i);
                int v2 = Q(q,(i+1)%4);
                if (v1 > v2) std::swap(v1,v2);
                vector<int> r(4);
                r[0] = v1; r[1] = v2;
                r[2] = q;  r[3] = i;
                QQT.push_back(r);
            }
        std::sort(QQT.begin(),QQT.end());
        QQ = MatrixXi::Constant((int)(Q.rows()),4,-1);


        for(unsigned int i=2;i<QQT.size();++i)
        {
            vector<int>& r1 = QQT[i-2];
            vector<int>& r2 = QQT[i-1];
            vector<int>& r3 = QQT[i];
            if ( (r1[0] == r2[0] && r2[0] == r3[0]) 
                 && 
                 (r1[1] == r2[1] && r2[1] == r3[1]) )
            {
                return false;
            }
        }
        return true;
    }



    void Cage::computeQQ()
    {
        assert(isManifold());
        QQ_QV_flag = true;
        _QV = VectorXi(V.rows());
        //cout << "isManifold(): "<<isManifold() << endl;
        vector<vector<int> > QQT;
        for (int q=0; q<Q.rows(); q++)
            for (int i=0; i<4; i++)
            {
                // QV*
                _QV( Q(q,i) ) = q;

                int v0 = Q(q,i);
                int v1 = Q(q,(i+1)%4);
                if (v0 > v1) swap(v0,v1);
                vector<int> r(4);
                r[0] = v0; r[1] = v1;
                r[2] = q;  r[3] = i;
                QQT.push_back(r);
            }
        sort(QQT.begin(), QQT.end());

        QQ = MatrixXi::Constant(Q.rows(), 4, -1);

        /*for (int i=0; i<QQT.size(); i++)
        {
            cout << "["<<i<<"] (" << QQT[i][0] <<
            ", "<<QQT[i][1]<<", "<<QQT[i][2]<<","<<QQT[i][3]<<")"<<endl;
        }*/

        for (unsigned int i=1; i<QQT.size(); i++)
        {
            vector<int>& r0 = QQT[i-1];
            vector<int>& r1 = QQT[i];
            if ((r0[0] == r1[0]) && (r0[1] == r1[1]))
            {
                QQ(r0[2], r0[3]) = r1[2];
                QQ(r1[2], r1[3]) = r0[2];
            }

        }
    }

    int Cage::getEdgeQuadAdjacent(int s, int d)
    {
        for (unsigned int i=0; i<4; i++)
        {
            if (QQ(s,i) == d) return i;
        }
    }

    MatrixXd Cage::getTMapping(Vector3i ABC)
{
    MatrixXd T = MatrixXd(3, 2);
    T <<    Vmesh.row(ABC(0)),
            Vmesh.row(ABC(1)),
            Vmesh.row(ABC(2));
    
    return T;
}

vector<int> Cage::getQV(int Vi)
{
    vector<int> QV, QVccw;
    int curt, firstt, k=0, l=0;
    int BUFFER_V = V.rows();

    if (!QQ_QV_flag) computeQQ(); 
        
    firstt = curt = _QV(Vi);
    do {
        QV.push_back(curt);
        assert(k<BUFFER_V); k++;
        for (int j=0;j<4;++j)
            if (Q(curt,j)==Vi)
            {
                curt = QQ(curt,(j+3)%4);
                break;
            }
    } while (curt!=firstt && curt != -1);

    if(curt!=firstt)
    {                   //allora e' necessario fare il giro inverso
        curt = firstt; //questo e' il primo in entrambi i versi
        do {
            QVccw.push_back(curt);
            assert(l<BUFFER_V); l++;
            for (int j=0;j<4;++j)
                if (Q(curt,j)==Vi)
                {
                    curt = QQ(curt,j);
                    break;
                }
        } while (curt!=firstt && curt != -1);


        for(l--;l>0;l--){
            QV.push_back(QVccw[l]);
        }

    }
    return QV;
}

vector<int> Cage::getVV(int Vi)
{
    vector<int> VV;
    vector<int> QV = getQV(Vi);
    //cout << "QV : "<< QV.size() << endl;
    for(vector<int>::const_iterator q = QV.begin(); q != QV.end(); ++q)
    {
        for (int i=0; i<4; i++ )
        {
            if ( Q(*q, i) == Vi)
            {
                // 3 cycle: quad round to Vi
                int j=(i+1)%4;
                while (j != i)
                {
                    if (find(VV.begin(), VV.end(), Q(*q, j)) == VV.end() )
                        VV.push_back(Q(*q, j));
                    j = (j+1)%4;
                }
            }
        }
    }
    return VV;
}

Vector3d Cage::getV(int Vi)
{
    return V.row(Vi);
}