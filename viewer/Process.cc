#include "Process.hh"
#include "ErrorsGrid.hh"
#include "ErrorsHalfEdgeQuad.hh"

    vector<int> Process::getBorderTrianglesSubDomainQ(vector<int> subQ)
    {
        vector<int> TsQ;
        for(int idT=0; idT<M.F.rows(); idT++)
        {
            int q0 = C.QVmesh(M.F(idT,0));
            int q1 = C.QVmesh(M.F(idT,1));
            int q2 = C.QVmesh(M.F(idT,2));
            if (q0==q1 && q1==q2)
                continue;
            else if (   find(subQ.begin(), subQ.end(), q0) != subQ.end()
                &&      find(subQ.begin(), subQ.end(), q1) != subQ.end()
                &&      find(subQ.begin(), subQ.end(), q2) != subQ.end()
                &&      find(TsQ.begin(), TsQ.end(), idT)  == TsQ.end())
            {
                TsQ.push_back(idT);
            }
        }
        return TsQ;
    }

     void Process::updateTQ()
    {
        cout << "Q size: "<<C.Q.rows() << endl;
        vector<vector<int>> listTQ(C.Q.rows());
         for(int i=0; i<M.F.rows(); i++)
        {
            int q0 = C.QVmesh(M.F(i,0));
            int q1 = C.QVmesh(M.F(i,1));
            int q2 = C.QVmesh(M.F(i,2));
            if (q0==q1 && q1==q2)
                listTQ[q0].push_back(i);
        }
        TQ = listTQ;
    }
    
    double Process::computeErrorFromListTriangle(vector<int> triangles, Cage &domain, Vector2d examVertex, Vector3d smap)
    {
        double distance = 0;
        for(vector<int>::const_iterator idT = triangles.begin(); idT != triangles.end(); ++idT)
        {
            Vector3d Vs = Utility::getCoordBarycentricTriangle(domain.getTMapping(M.F.row(*idT)), M.getT(*idT), examVertex);
            distance += Utility::computeDistance(Vs, smap);

        }
         return distance/triangles.size();
    }

    void Process::getTrianglesInExpMapping(int Vi, CageSubDomain &sC)
    {
        sC.initAll(C);
        sC.computeDomain(Vi); 
        vector<int> TsQ = getBorderTrianglesSubDomainQ(sC.sQ);
        debugPartialTQ[Vi] = TsQ;
            
        sC.triangles = M.findTriangles(TsQ, sC.sV[ sC.iV[Vi] ], sC);
        sC.examVertex =  sC.sV[ sC.iV[Vi] ];

        if (sC.triangles.size() == 0)
        {
            //sC.computeDomainPartial(Vi);
            //sC.triangles = M.findTriangles(TsQ, sC.sV[ sC.iV[Vi] ], sC);
         
            //vector<int> triangles = M.findTrianglesDebug(TsQ, sC.sV[ sC.iV[*Vi] ], sC);
            //sC.triangles = triangles;
            //cout <<Vi <<". ALERT "<< sC.triangles.size()<<endl;
        }
    }
    void Process::movingVertexCageToMesh(vector<int> newVertices)
    {
        Vector3d Vs;
        cout << "How many vertices? "<< newVertices.size() << endl;
        for(vector<int>::const_iterator Vi = newVertices.begin(); Vi != newVertices.end(); ++Vi )
        {  
            CageSubDomain sC;
            getTrianglesInExpMapping(*Vi, sC);
            if (sC.triangles.size() == 0)
            {
                cout << "ALERT "<< *Vi << " niente"<<endl;
            }
            for(vector<int>::const_iterator idT = sC.triangles.begin(); idT != sC.triangles.end(); ++idT)
            {
                Vs = Utility::getCoordBarycentricTriangle(sC.getTMapping(M.F.row(*idT)), M.getT(*idT), sC.sV[ sC.iV[*Vi] ]);
                C.V.row(*Vi) = Vs; // new Mapping :)
                break;
            }
            //if (storeSubC.size() == 0)
            storeSubC[*Vi] = sC;
        }
    }

    void Process::initErrorsAndRelations()
    {
        C.computeQQ();
        P.computePolychords(); 
        updateTQ();
        distancesBetweenMeshCage();
        storeSampleTriangles.clear();
        storeSubC.clear();

        //E = new ErrorsGrid(*this);
        E = new ErrorsHalfEdgeQuad(*this);
    }

    void Process::raffinementQuadLayout(int times)
    {
        int i = 0;
        while(i < times )
        {
            cout << " -----------iteration Raffinement ---------------"<<endl;
            vector<int> newVertices;


            // Find polychord with the greatest error
            int worstPolychord = E->getPolychordWithMaxError();
            cout << "The worst Polychord error: "<< worstPolychord << endl;
            //worstPolychord = 2;
            // Split found polychord and cage 
            if (worstPolychord == -1)
                return;
            int q_next, q_start = P.P[worstPolychord][0];
            for(vector<int>::const_iterator q = P.P[worstPolychord].begin(); q != P.P[worstPolychord].end(); ++q)
            {
                q_next = *(q+1);
                if (q+1 == P.P[worstPolychord].end())
                    q_next = q_start;
                int e = C.getEdgeQuadAdjacent(*q, q_next);
                C.split(*q, e, (e+2)%4, newVertices);
            }
            
            // Reinitialize relation adj.
            initErrorsAndRelations();
            // moveCage towards mesh triangle
            movingVertexCageToMesh(newVertices); 

            cout << " ----------- Cycle End ---------------"<<endl;
            
            /*
            for (int i=0; i<C.Q.rows(); i++)
                cout << "["<<i<<"]"<< C.Q.row(i)<<endl;
            cout << "QQ" <<endl;
            for(int i=0; i<C.QQ.rows(); i++)
                cout << "["<<i<<"]"<< C.QQ.row(i) << endl;
            cout << endl;*/
            i++;
        }
    }


    void Process::distancesBetweenMeshCage()
    {
        double min, max;
        MatrixXd V = M.V;
        distancesMeshCage = VectorXd(V.rows());

        for (unsigned int i = 0; i < V.rows(); i++)
        {
            int quality = C.QVmesh(i);
            Vector2d p = C.Vmesh.row(i);
            distancesMeshCage[i] = Utility::computeDistance(V.row(i), C.getVMapping(quality, p));
        
        }
        
        min = distancesMeshCage.minCoeff();
        max = distancesMeshCage.maxCoeff();
        for (unsigned int i = 0; i < V.rows(); i++)
            distancesMeshCage[i] = Utility::normalizeDistance(distancesMeshCage[i], min, max);
    }


    void Process::initAll(char *filename)
    {
        char buf[200];
        strcpy(buf, filename);
        read(buf);
        read(strcat(buf, ".domain.off"));
        distancesBetweenMeshCage();

        P = Polychords(&(C));
        initErrorsAndRelations();

        vector<int> indexVerticesCage(C.V.rows());
        std::iota(indexVerticesCage.begin(), indexVerticesCage.end(), 0);
        movingVertexCageToMesh(indexVerticesCage);
    }
/////////////////// I/O ///////////////////////////////////////
    void Process::read(char *str)
    {
        cout << str <<endl;
        char* p;
        for (p = str; *p != '\0'; p++) ;
        while (*p != '.') p--;
        if (!strcmp(p, ".off") || !strcmp(p, ".OFF"))
        {
            readOFF (str);
            fprintf (stdout, "Loaded mesh `%s\'.\n#vertices: %d.\n#faces: %d.\n", str, (int)C.V.rows(), (int)C.Q.rows());
        } else if (!strcmp(p, ".ply") || !strcmp(p, ".PLY"))
        {
            readPLY (str);
            fprintf (stdout, "Loaded mesh `%s\'.\n#vertices: %d.\n#faces: %d.\n", str, (int)M.V.rows(), (int)M.F.rows());
        }else {
            fprintf (stdout, "File format not supported!\n");
            return;
        }
        
        
    }

    void Process::readOFF (char* meshfile)
    {
        int vnum, fnum;
        FILE *fp = fopen (meshfile, "r");

        if (!fp) fprintf (stderr, "readOFF(): could not open file %s", meshfile);

        int r = 0;
        r = fscanf (fp, "OFF\n%d %d 0\n",  &vnum, &fnum);
    
        C.V = MatrixXd (vnum, 3);
        C.Q = MatrixXi (fnum, 4);

        for (int i = 0; i < C.V.rows(); i++)
            r = fscanf (fp, "%lf %lf %lf\n", &(C.V)(i,0), &(C.V)(i,1), &(C.V)(i,2));    
        for (int i = 0; i < C.Q.rows(); i++)
            r = fscanf (fp, "4 %d %d %d %d\n", &(C.Q)(i,0), &(C.Q)(i,1), &(C.Q)(i,2), &(C.Q)(i,3));
        for (int i = 0; i < C.Q.rows(); i++)
            for (int j = 0; j < 4; j++)
                if (C.Q(i,j) >= C.V.rows())
                    fprintf (stderr, "readOFF(): warning vertex: %d"
                             " in face: %d has value: %d, greater than #v: %d\n",
                             j,i,C.Q(i,j),(int)C.V.rows());
        fclose (fp);
        r=r; // remove warnings
    }

    void Process::readPLY (char* meshfile)
    {
        int vnum, fnum;
        //FILE *fp = fopen (meshfile, "r");
        ifstream fp(meshfile);
        if (!fp.is_open()) fprintf (stderr, "readPLY(): could not open file %s", meshfile);

        string line;
        unsigned found;
        bool flag = false;
        while(getline(fp, line))
        {
            if (line.find("element vertex") != std::string::npos)
            {
                found = line.find_last_of(" ");
                vnum = atoi(line.substr(found+1).c_str());
            }
            if (line.find("element face") != std::string::npos)
            {
                found = line.find_last_of(" ");
                fnum = atoi(line.substr(found+1).c_str());
            }
            if (line.find("int flags") != std::string::npos)
            {
                flag = true;
            }
            if (line == "end_header")
                break;
        }
        
        //std::cout << vnum << " " << fnum << endl;

        float x,y,z,u,v,quality;
        int flags;

        M.V = MatrixXd (vnum, 3);
        M.F = MatrixXi (fnum, 3);
        C.Vmesh = MatrixXd (vnum, 2);
        C.QVmesh = VectorXi (vnum);

        for(int i=0; i<vnum; i++)
        {
            getline(fp, line);
            stringstream linestream(line);
            if(flag)
                linestream >> x >> y >> z >> flags >> quality >>
                                    u >> v;
            else linestream >> x >> y >> z >> quality >> 
                                    u >> v;
            M.V.row(i) = Vector3d(x,y,z);
            C.Vmesh.row(i) = Vector2d(u,v);
            C.QVmesh[i] = quality;

        }
        int tmp, v0,v1,v2; // 3
        for(int i=0; i<fnum; i++)
        {
            getline(fp, line);
            stringstream linestream(line);
            linestream >> tmp >> v0 >> v1 >> v2;
            M.F.row(i) = Vector3i(v0,v1,v2);
        }
        
    } 