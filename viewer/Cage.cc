#include "Cage.hh"


Vector3d Cage::getVMapping(int i)
{
		int quality = ParQ(i);
        Vector2d p = ParV.row(i);
        Vector4i quad = Q.row(quality);
        Vector3d 	A(V.row(quad[0])),
                	B(V.row(quad[1])),
                	C(V.row(quad[2])),
                	D(V.row(quad[3]));

        Vector3d U = (B-A);
        Vector3d V = (D-A);
        return A + U*p(0) + V*p(1);
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
        cout << "isManifold(): "<<isManifold() << endl;
        vector<vector<int> > QQT;
        for (int q=0; q<Q.rows(); q++)
            for (int i=0; i<4; i++)
            {
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