#include "Process.hh"
    vector<int> Process::getBorderTrianglesSubDomainQ(vector<int> subQ)
    {
        vector<int> TsQ;
        for(int idT=0; idT<M.F.rows(); idT++)
        {
            bool flag = true;
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
    void Process::initSubDomain(CageSubDomain &sC)
    {
        sC.setV(C.V);
        sC.setQ(C.Q);
        sC.setVmesh(C.Vmesh);
        sC.setQVmesh(C.QVmesh);
        sC.setQQ(C.QQ);
        sC.setPartialQV(C._QV);
        sC.sV.clear();
        sC.iV.clear();
        sC.sQ.clear();
        sC.triangles.clear();
    }

    void Process::movingVertexCageToMesh(vector<int> newVertices)
    {
        Vector3d Vs;
        //CageSubDomain sC;
        cout << "How many vertices? "<< newVertices.size() << endl;
        for(vector<int>::const_iterator Vi = newVertices.begin(); Vi != newVertices.end(); ++Vi )
        {  
            CageSubDomain sC;
            initSubDomain(sC);
            sC.initDomain(*Vi); 
            
            vector<int> TsQ = getBorderTrianglesSubDomainQ(sC.sQ);
            debugPartialTQ = TsQ;
            //error: ‘Cage’ is an inaccessible base of ‘CageSubDomain’ TO DO
            //cout << "Vector: "<< sC.iV.find(*Vi)->first<<" - "<<sC.iV.find(*Vi)->second<<endl;
            
            sC.triangles = M.findTriangles(TsQ, sC.sV[ sC.iV[*Vi] ], sC);
            cout <<*Vi <<". ALERT "<< sC.triangles.size()<<endl;
            for(vector<int>::const_iterator idT = sC.triangles.begin(); idT != sC.triangles.end(); ++idT )
            {
                cout << "Triangle: "<<*idT << endl;
            }
            /*if (*Vi == 8)
            {
                int idTdebug = 8053;
                cout << C.QVmesh(M.F(idTdebug, 0)) << ","<< C.QVmesh(M.F(idTdebug, 1)) <<","<< C.QVmesh(M.F(idTdebug, 2)) <<endl;
        
                for(vector<int>::const_iterator q = sC.sQ.begin(); q != sC.sQ.end(); ++q )
                {
                    cout << *q<<" , ";
                }
                cout <<endl;
                break;
            }
            break;   
            */

            /*for(vector<int>::const_iterator idT = triangles.begin(); idT != triangles.end(); ++idT)
            {
                Vs = Utility::getCoordBarycentricTriangle(sC.getTMapping(M.F.row(*idT)), M.getT(*idT), sC.sV[ sC.iV[*Vi] ]);
                C.V.row(*Vi) = Vs; // new Mapping :)
            }*/
            storeSubC[*Vi] = sC;
        }
    }


    double Process::errorSample(int q, Vector2d s)
    {
        vector<int> triangles = M.findTriangles(TQ[q], s, C);
        storeSampleTriangles[q].insert(std::make_pair(s, triangles) );
        if (triangles.size() == 0)
        {
            orphanSample.push_back(s);
            //if ( (s(0) != 0.8 && s(0) != 0.2) && (s(1) != 0.8 && s(1) != 0.2) )
            //cout << "Triangle not found: quad["<<q<<"]" << "---" << s(0)<<"-"<<s(1)<<endl;
            return 0;
        }
        double distance = 0;
        for(vector<int>::const_iterator idT = triangles.begin(); idT != triangles.end(); ++idT)
        {
            Vector3d Vs = Utility::getCoordBarycentricTriangle(C.getTMapping(M.F.row(*idT)), M.getT(*idT), s);
            distance += Utility::computeDistance(Vs, C.getVMapping(q, s));
        }
       //cout << distance <<"  =   "<< distance/triangles.size() << " -> "<<triangles.size()<<endl;        
        return distance/triangles.size();
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
    double Process::errorsGrid(int q, int r, int c)
    {
        double tmpE, E = 0;
        double domain = GRID_SAMPLE;
        double step_x = domain/r;
        double step_y = domain/c;

        map<Vector2d, double, classcomp> storeErrorSample;

        orphanSample.clear();
        storeSampleTriangles.push_back(map<Vector2d, vector<int>, classcomp>());
        for (int i=1; i<r; i++)
            for (int j=1; j<c; j++)
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
        
       
        return E * Utility::areaQuad(
                            Vector3d(C.V.row(C.Q(q, 0))),
                            Vector3d(C.V.row(C.Q(q, 1))),
                            Vector3d(C.V.row(C.Q(q, 2))),
                            Vector3d(C.V.row(C.Q(q, 3)))
                            ) / (r*c);
    }
    void Process::initErrorsAndRelations()
    {
        C.computeQQ();
        P.computePolychords(); 
        updateTQ();
        storeSampleTriangles.clear();
        errorQuads = VectorXd(C.Q.rows());
        for (int i=0; i<C.Q.rows(); i++)
            errorQuads(i) = errorsGrid(i);
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
        while(i < times)
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
        /*
        bool normalized = false;
        Vector2d maxPar = C.ParV_2D.colwise().maxCoeff();
        if (maxPar(0) >= 0 && maxPar(1) <= 1)
            normalized = true;
        cout << "normalized: "<< normalized << endl;
        */
        mapV = MatrixXd(V.rows(), 3);
        for (unsigned int i = 0; i < V.rows(); i++)
        {
            int quality = C.QVmesh(i);
            Vector2d p = C.Vmesh.row(i);
            mapV.row(i) = C.getVMapping(quality, p);
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
