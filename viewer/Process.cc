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
        sC.computeDomain(Vi); 
        vector<int> TsQ = getBorderTrianglesSubDomainQ(sC.sQ);
        
        #ifdef __MODE_DEBUG
        debugPartialTQ[Vi] = TsQ;
        #endif
            
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
    void Process::movingVertexCageToMesh(vector<int> newVertices, Cage &C)
    {
        Vector3d Vs;
        //cout << "How many vertices? "<< newVertices.size() << endl;
        for(vector<int>::const_iterator Vi = newVertices.begin(); Vi != newVertices.end(); ++Vi )
        {  
            CageSubDomain sC;
            sC.initAll(C);
            getTrianglesInExpMapping(*Vi, sC);
            if (sC.triangles.size() == 0)
            {
                //cout << "ALERT "<< *Vi << " niente"<<endl;
            }

            for(vector<int>::const_iterator idT = sC.triangles.begin(); idT != sC.triangles.end(); ++idT)
            {
                Vs = Utility::getCoordBarycentricTriangle(sC.getTMapping(M.F.row(*idT)), M.getT(*idT), sC.sV[ sC.iV[*Vi] ]);
                C.V.row(*Vi) = Vs; // new Mapping :)
                break;
            }
            
            #ifdef __MODE_DEBUG
            storeSubC[*Vi] = sC;
            #endif
        }
    }

    void Process::initErrorsAndRelations(Cage &C, Polychords &P)
    {
        C.computeQQ();
        P = Polychords(&(C));
        P.computePolychords(); 
        distancesBetweenMeshCage();
        
        #ifdef __MODE_DEBUG
        storeSampleTriangles.clear();
        storeSubC.clear();
        #endif
    }

    double Process::distancesBetweenMeshCage(Cage C)
    {
        double E = 0;
        for (unsigned int i = 0; i < M.V.rows(); i++)
        {
            int q = C.QVmesh(i);
            Vector2d p = C.Vmesh.row(i);
            E += Utility::computeDistance(M.V.row(i), C.getVMapping(q, p));
        }
        return E;
    }

    void Process::processToSplit(Cage &C, vector<int> listQ, vector<int> &newVertices, vector<int> &newQuads)
    {
        int q_next, q_start = listQ[0];
        newQuads.clear();
        newVertices.clear();
        for(vector<int>::const_iterator q = listQ.begin(); q != listQ.end(); ++q)
        {
            q_next = *(q+1);
            if (q+1 == listQ.end())
                q_next = q_start;
            int e = C.getEdgeQuadAdjacent(*q, q_next);
            newQuads.push_back(C.split(*q, e, (e+2)%4, newVertices));
        }
    }

    int Process::queueRaffinementQuadLayout(Cage &C0, Polychords &P0, int tryTimes)
    {
        map<double, int>  queueE;
        vector<int> newVertices, newQuads;

        Process process;
        process.C = Cage(C0.V, C0.Q, C0.Vmesh, C0.QVmesh, C0.QQ, C0._QV);
        process.P = Polychords(&(process.C));
        process.P.computePolychords();
        process.M = M;
        
        for(int idP = 0; idP<P0.getSize(); idP++)
        {
            Process fakeEnvironment = process;
            map<double, int>  Etmp;
            IError *Err = new ErrorsHalfEdgeQuad(&fakeEnvironment, E->getErrorPolychords(), E->getErrorsQuad());
            double errBefore = Err->getErrorpolychordByID(idP);

            for(int i=0; i<tryTimes; i++)
            {
                processToSplit(fakeEnvironment.C, fakeEnvironment.P.P[idP], newVertices, newQuads);
                newQuads.insert( newQuads.end(), fakeEnvironment.P.P[idP].begin(), fakeEnvironment.P.P[idP].end());
                // Reinitialize relation adj.
                initErrorsAndRelations(fakeEnvironment.C, fakeEnvironment.P);
        
                // moveCage towards mesh triangle
                movingVertexCageToMesh(newVertices, fakeEnvironment.C); 

                Err->computeErrorsGrid(newQuads);
                double errAfter = Err->getErrorpolychordByID(idP);
                Etmp.insert(make_pair(errAfter - errBefore, i));
                errBefore = errAfter;
            }
            map<double, int>::const_iterator worstSplit = Etmp.begin();
            queueE.insert(make_pair(worstSplit->first, idP));
        }
        map<double, int>::const_iterator worstPolychord = queueE.begin();
        
        //for (map<double, int>::const_iterator it = queueE.begin(); it != queueE.end(); ++it)
        //    cout << "["<<it->second<<"]"<<" "<<it->first <<endl;
        
        return worstPolychord->second;
    }

    void Process::raffinementQuadLayout(int times)
    {
        ofstream seqPolychord, timePolychord;
        configurationFileOutput(seqPolychord, timePolychord);

        bool condition = false;
        if (QuadMax != -1 || ErrMax != 0)
            condition = true;

        int i = 0;
        while( condition || i < times )
        {
            if (QuadMax != -1 && C.Q.rows() > QuadMax)
                break;
            // Reinitialize relation adj.
            initErrorsAndRelations(C, P);
            raffinementTimes++;
            cout << " -----------iteration Raffinement:"<<raffinementTimes<<" ---------------"<<endl<<endl;
            chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
            switch(error_type_choice)
            {
                case WITH_QUEUE:
                        cout <<"\t raffinament with queue "<<endl;
                        if (E == NULL || info.error_mode != LastIteration::WITH_QUEUE)
                        {
                            E = new ErrorsHalfEdgeQuad(this);
                        }
                        else
                            E->computeErrorsGrid(info.newQuads);
                        
                        info.worstPolychord = queueRaffinementQuadLayout(C, P);
                        info.error_mode = LastIteration::WITH_QUEUE;
                        break;  
                case GRID_SIMPLE:
                        cout <<"\t raffinament with ErrorsGrid"<<endl;
                        E = new ErrorsGrid(this);
                        info.worstPolychord = E->getPolychordWithMaxError();
                        info.error_mode = LastIteration::GRID_SIMPLE;
                        break;
                case GRID_HALFEDGE:
                        cout <<"\t raffinament with ErrorsHalfEdgeQuad "<<endl;

                        if (E == NULL || info.error_mode != LastIteration::GRID_HALFEDGE)
                            E = new ErrorsHalfEdgeQuad(this);
                        else
                            E->computeErrorsGrid(info.newQuads);
                        
                        info.worstPolychord = E->getPolychordWithMaxError();
                        info.error_mode = LastIteration::GRID_HALFEDGE;
                        break;
                default: 
                        break;
            }
            seqPolychord <<raffinementTimes<<" "<< info.worstPolychord <<endl;
            cout << "The worst Polychord ID: "<< info.worstPolychord << " error: "<< info.LastError <<" error"<< endl<<endl;
            if (info.LastError < ErrMax)
                break;

            processToSplit(C, P.P[info.worstPolychord], info.newVertices, info.newQuads);
            info.newQuads.insert( info.newQuads.end(), P.P[info.worstPolychord].begin(), P.P[info.worstPolychord].end());

            // moveCage towards mesh triangle
            movingVertexCageToMesh(info.newVertices, C); 
            
            
            chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>( t2 - t1 ).count();
            cout << duration <<" microsec." <<endl<<endl;
            timePolychord<<raffinementTimes<<" "<<duration<<endl;
            i++;
        }
        seqPolychord.close();
        timePolychord.close();
    }

    void Process::distancesBetweenMeshCage()
    {
        double min, max;
        MatrixXd V = M.V;
        distancesMeshCage = VectorXd(V.rows());

        for (unsigned int i = 0; i < V.rows(); i++)
        {
            int q = C.QVmesh(i);
            Vector2d v = C.Vmesh.row(i);
            distancesMeshCage[i] = Utility::computeDistance(V.row(i), C.getVMapping(q, v));
            
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

        raffinementTimes = 0;
        reopenFile = true;
        info.clean();

        E = NULL;

        P = Polychords(&(C));
        initErrorsAndRelations(C, P);

        vector<int> indexVerticesCage(C.V.rows());
        std::iota(indexVerticesCage.begin(), indexVerticesCage.end(), 0);
        movingVertexCageToMesh(indexVerticesCage, C);
    }


    void Process::configurationFileOutput(ofstream &seqPolychord, ofstream &timePolychord)
    {
        string fileNameTime, fileNameSequence, introTime, introSeq;
        //char *newF = strchr(filename, '.');
        string newF(filename);
        std::size_t found = newF.find_last_of("/\\");
        newF = newF.substr(found+1);
        newF = newF.substr(0, newF.find_last_of("."));

        mkdir("analysisOutput", S_IRWXU);
        mkdir( ("analysisOutput/"+newF).c_str(), S_IRWXU);

        switch(error_type_choice)
        {
            case WITH_QUEUE:
                            fileNameSequence = "analysisOutput/"+newF+"/WQ_seqPolychord_"+ newF +".txt";
                            fileNameTime = "analysisOutput/"+newF+"/WQ_timePolychord_"+ newF +".txt";
                            introTime = introSeq = "#Raff. with queue ";
                            break;  
            case GRID_SIMPLE:
                            fileNameSequence = "analysisOutput/"+newF+"/EG_seqPolychord_"+ newF +".txt";
                            fileNameTime = "analysisOutput/"+newF+"/EG_timePolychord_"+ newF +".txt";
                            introTime = introSeq = "#Raff. with ErrorsGrid ";
                            break;
            case GRID_HALFEDGE: 
                            fileNameSequence = "analysisOutput/"+newF+"/HE_seqPolychord_"+ newF +".txt";
                            fileNameTime = "analysisOutput/"+newF+"/HE_timePolychord_"+ newF +".txt";
                            introTime = introSeq = "#Raff. with ErrorsHalfEdgeQuad ";
                            break;
        }
        std::ios_base::openmode mode = std::ios_base::app;
        if (reopenFile)
        {
            mode = std::ofstream::out | std::ofstream::trunc;
            reopenFile = false;
        }   

        timePolychord.open(fileNameTime, mode);
        seqPolychord.open(fileNameSequence, mode);
        //timePolychord << introTime << endl;
        //seqPolychord << introSeq <<endl;
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