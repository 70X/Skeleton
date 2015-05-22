#include "Process.hh"
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

    double Process::errorSample(int q, Vector2d s)
    {
        double distance = 0;
        vector<int> triangles = M.findTriangles(TQ[q], s, C);
        storeSampleTriangles[q].insert(std::make_pair(s, triangles) );
        if (triangles.size() == 0)
        {
            int Vi = C.getAreaQuad(q, s);
            CageSubDomain sC;
            getTrianglesInExpMapping(Vi, sC);
            if (sC.triangles.size() == 0)
            {
                //cout << " Sample: non trovo niente" <<endl;
                orphanSample.push_back(s);
                return 0;
            }
            
            orphanSample.push_back(s);
            distance = computeErrorFromListTriangle(sC.triangles, sC, sC.examVertex,  C.getVMapping(q, s));
        }
        else
            distance = computeErrorFromListTriangle(triangles, C, s,  C.getVMapping(q, s));
      
        return distance;
    }


    double Process::errorAvarageSamples( Vector2d s, double step_x, double step_y, map<Vector2d, double, classcomp> storeErrorSample)
    {
        vector<double> eRound;
        double Err = 0;

        Vector2d W  = Vector2d( s(0) - step_x, s(1)          );
        Vector2d SW = Vector2d( s(0) - step_x, s(1) - step_y );
        Vector2d S  = Vector2d( s(0)         , s(1) - step_y );
        Vector2d SE = Vector2d( s(0) + step_x, s(1) - step_y );
        Vector2d E  = Vector2d( s(0) + step_x, s(1)          );
        Vector2d NE = Vector2d( s(0) + step_x, s(1) + step_y );
        Vector2d N  = Vector2d( s(0)         , s(1) + step_y );
        Vector2d NW = Vector2d( s(0) - step_x, s(1) + step_y );

        if ( storeErrorSample.find(W) != storeErrorSample.end() && storeErrorSample.at(W) > 0)
            eRound.push_back(storeErrorSample.at(W));

        if ( storeErrorSample.find(SW) != storeErrorSample.end() && storeErrorSample.at(SW) > 0)
            eRound.push_back(storeErrorSample.at(SW));

        if ( storeErrorSample.find(S) != storeErrorSample.end() && storeErrorSample.at(S) > 0)
            eRound.push_back(storeErrorSample.at(S));

        if ( storeErrorSample.find(SE) != storeErrorSample.end() && storeErrorSample.at(SE) > 0)
            eRound.push_back(storeErrorSample.at(SE));

        if ( storeErrorSample.find(E) != storeErrorSample.end() && storeErrorSample.at(E) > 0)
            eRound.push_back(storeErrorSample.at(E));

        if ( storeErrorSample.find(NE) != storeErrorSample.end() && storeErrorSample.at(NE) > 0)
            eRound.push_back(storeErrorSample.at(NE));

        if ( storeErrorSample.find(N) != storeErrorSample.end() && storeErrorSample.at(N) > 0)
            eRound.push_back(storeErrorSample.at(N));

        if ( storeErrorSample.find(NW) != storeErrorSample.end() && storeErrorSample.at(NW) > 0)
            eRound.push_back(storeErrorSample.at(NW));

        if (eRound.size() == 0) return 0; // not Exists neighborhood of s
        for(vector<double>::const_iterator Ei = eRound.begin(); Ei != eRound.end(); ++Ei)
            Err+= *Ei;
        return (Err/(double) eRound.size());
    }
    double Process::errorsGrid(int q)
    {
        double tmpE, E = 0;
        double domain = GRID_SAMPLE;
        double diagonal = C.bb();
        double spacing = diagonal * (1.0/20.0);

        Vector3d _A = C.V.row(C.Q(q,0));
        Vector3d _B = C.V.row(C.Q(q,1));
        Vector3d _C = C.V.row(C.Q(q,2));
        Vector3d _D = C.V.row(C.Q(q,3));
        double m = ceil( (Utility::computeDistance((_B-_A), (_D-_C))/2.0) / spacing)+1;
        double n = ceil( (Utility::computeDistance((_A-_D), (_C-_B))/2.0) / spacing)+1;
        m = n = 5;
        double step_x = domain/m;
        double step_y = domain/n;
        
        map<Vector2d, double, classcomp> storeErrorSample;

        orphanSample.clear();
        storeSampleTriangles.push_back(map<Vector2d, vector<int>, classcomp>());
        for (int i=1; i<m; i++)
            for (int j=1; j<n; j++)
            {
                Vector2d s = Vector2d(step_x*i, step_y*j);
                tmpE = errorSample(q, s);
                storeErrorSample.insert(make_pair(s, tmpE) );
                E += tmpE;
                // when return 0 is handled ahead
            }
            
        // management orphan sample
        if (orphanSample.size() > 0)
        {
            for(vector<Vector2d>::const_iterator s = orphanSample.begin(); s != orphanSample.end(); ++s)
            {
                tmpE = errorAvarageSamples((*s),step_x, step_y, storeErrorSample);
                //cout << q << " s: "<< (*s)(0)<<","<<(*s)(1)<<" = "<< tmpE <<endl;
                E += tmpE;
            }
        }
        
       
        return E * Utility::areaQuad(_A, _B, _C, _D ) / (m*n);
    }
    void Process::initErrorsAndRelations()
    {
        C.computeQQ();
        P.computePolychords(); 
        updateTQ();
        distancesBetweenMeshCage();
        storeSampleTriangles.clear();
        storeSubC.clear();
        errorQuads = VectorXd(C.Q.rows());
        for (int i=0; i<C.Q.rows(); i++)
        {
            errorQuads(i) = errorsGrid(i);
            //cout << i <<" Err: "<< errorQuads(i) << " with: "<<orphanSample.size()<<endl;
        }
    }
    VectorXd Process::errorPolychords()
    {
        double E;
        VectorXd polychordsError(P.getSize());

        for (int i=0; i<P.getSize(); i++)
        {
            E = 0;
            for(vector<int>::const_iterator q = P.P[i].begin(); q != P.P[i].end(); ++q)
            {
                E += errorQuads(*q);
            }
            polychordsError[i] = E*((double) P.P[i].size() / (double) C.Q.rows() );
        }
        return polychordsError;
    }
    void Process::raffinementQuadLayout(int times)
    {
        int i = 0;
        double max = errorQuads.maxCoeff();
        while(i < times )
        {
            cout << " -----------iteration Raffinement ---------------"<<endl;
            vector<int> newVertices;

            VectorXd polychordsError = errorPolychords();
            double maxError = 0;
            int worstPolychord = -1;
            // Find polychord with the greatest error
            for (unsigned int id=0; id<polychordsError.rows(); id++)
            {
                double currentError = polychordsError[id];
                cout << "Error polychord["<<id<<"] :"<< currentError << endl;
                 if (currentError > maxError)
                {
                    maxError = currentError;
                    worstPolychord = id;
                }
            }
            cout << "The worst Polychord error: "<< worstPolychord << endl;
            //worstPolychord = 2;
            // Split found polychord and cage 
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
            // 8 => 7, 10 => 8, 9 => 11, 9 => 9
            // moveCage towards mesh triangle
            movingVertexCageToMesh(newVertices);
            max = errorQuads.maxCoeff();

            cout << " ----------- End  ---------------"<<endl;
            
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
        C.computeQQ();
        
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
