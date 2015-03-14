#include "Mesh.hh"
	
	bool Mesh::isManifold()
    {
        vector<vector<int> > TTT;
        for(int f=0;f<F.rows();++f)
            for (int i=0;i<3;++i)
            {
                // v1 v2 f ei 
                int v1 = F(f,i);
                int v2 = F(f,(i+1)%3);
                if (v1 > v2) std::swap(v1,v2);
                vector<int> r(4);
                r[0] = v1; r[1] = v2;
                r[2] = f;  r[3] = i;
                TTT.push_back(r);
            }
        std::sort(TTT.begin(),TTT.end());
        TT = MatrixXi::Constant((int)(F.rows()),3,-1);
        
        for(unsigned int i=2;i<TTT.size();++i)
        {
            vector<int>& r1 = TTT[i-2];
            vector<int>& r2 = TTT[i-1];
            vector<int>& r3 = TTT[i];
            if ( (r1[0] == r2[0] && r2[0] == r3[0]) 
                 && 
                 (r1[1] == r2[1] && r2[1] == r3[1]) )
            {
                return false;
            }
        }
        return true;
    }
// Initialize TT-VT*
    void Mesh::initTT()
    {
        assert(isManifold());
        VT.resize(V.rows());
        vector<vector<int> > TTT;
        for(int f=0;f<F.rows();++f)
            for (int i=0;i<3;++i)
            {
                // VT*
                VT(F(f,i))=f;
                // v1 v2 f ei 
                int v1 = F(f,i);
                int v2 = F(f,(i+1)%3);
                if (v1 > v2) std::swap(v1,v2);
                vector<int> r(4);
                r[0] = v1; r[1] = v2;
                r[2] = f;  r[3] = i;
                TTT.push_back(r);
            }
        std::sort(TTT.begin(),TTT.end());
        TT = MatrixXi::Constant((int)(F.rows()),3,-1);
        
        for(unsigned int i=1;i<TTT.size();++i)
        {
            vector<int>& r1 = TTT[i-1];
            vector<int>& r2 = TTT[i];
            if ((r1[0] == r2[0]) && (r1[1] == r2[1]))
            {
                TT(r1[2],r1[3]) = r2[2];
                TT(r2[2],r2[3]) = r1[2];
            }
        }
        TTVTflag = true;
    }

VectorXi Mesh::getVT(int i) // returns VT relation of the i-th vertex
    {
        VectorXi buf(BUFFER_VT_AND_VV), buf_inv(BUFFER_VT_AND_VV);
        int curt, firstt, k=0, l=0;
        
        if (!TTVTflag) initTT(); 
        
        firstt = curt = VT(i);
        do {
            buf(k++) = curt;
            assert(k<BUFFER_VT_AND_VV);
            for (int j=0;j<3;++j)
                if (F(curt,j)==i)
                {
                    curt = TT(curt,(j+2)%3);
                    break;
                }
        } while (curt!=firstt && curt != -1);

        if(curt!=firstt){ //allora e' necessario fare il giro inverso
            curt = firstt; //questo e' il primo in entrambi i versi
            do {
                buf_inv(l++) = curt;
                assert(l<BUFFER_VT_AND_VV);
                for (int j=0;j<3;++j)
                    if (F(curt,j)==i)
                    {
                        curt = TT(curt,j);
                        break;
                    }
            } while (curt!=firstt && curt != -1);


            for(l--;l>0;l--){
                buf(k++) = buf_inv(l);
            }

        }

        buf.conservativeResize(k);

        return buf;
    }