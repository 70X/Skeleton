#include "Process.hh"
        
    void Process::updateTQ()
    {
        cout << "Q size: "<<C.Q.rows() << endl;
        VectorXi countQuadVertices = VectorXi::Constant(M.V.rows(), -1);

        vector<vector<int>> listTQ(C.Q.rows());
        int count_v_orphan = 0;
        for(int i=0; i<M.F.rows(); i++)
        {
            int i0 = M.F(i,0);
            int i1 = M.F(i,1);
            int i2 = M.F(i,2);
            int q0 = C.QVpar(i0);
            int q1 = C.QVpar(i1);
            int q2 = C.QVpar(i2);

            // tutti i vertici appartengono allo stesso quad
            if (q0==q1 && q1==q2)
            {
                countQuadVertices(i0) = 1;
                countQuadVertices(i1) = 1;
                countQuadVertices(i2) = 1;
                listTQ[q0].push_back(i);
            }
            // solo due vertici appartengono allo stesso quad
            else if (q0==q1 || q0==q2 || q1==q2)
            {
                countQuadVertices(i0) = 2;
                countQuadVertices(i1) = 2;
                countQuadVertices(i2) = 2;
                /*listTQ[q0].push_back(i);
                listTQ[q2].push_back(i);*/
            }
            // tutti e tre i vertici appartengono a quad differenti
            else if (q0!=q1 && q1!=q2)
            {
                countQuadVertices(i0) = 3;
                countQuadVertices(i1) = 3;
                countQuadVertices(i2) = 3;
                /*listTQ[q0].push_back(i);
                listTQ[q1].push_back(i);
                listTQ[q2].push_back(i);*/
            }
            // verifica che non ci siano altri casi non gestiti
            else 
            {
                cout << i << " ???? "<<q0<<" "<<q1<<" "<<q2 << endl;
            }
            
        }
        TQ = listTQ;

        //DEBUG -------------------------------------------
        double oneV = 0, twoV = 0, allV = 0;
        for(int i=0; i<countQuadVertices.rows(); i++)
        {
            if (countQuadVertices(i) == 3) oneV++;
            if (countQuadVertices(i) == 2) twoV++;
            if (countQuadVertices(i) == 1) allV++;
        }
        cout << "Each Vertex with quad different: "<< oneV <<endl;
        cout << "Two Verteces with the same quad: "<< twoV <<endl;
        cout << "All Verteces with the same quad: "<< allV <<endl;
        cout << "Tot:                             "<< oneV+twoV+allV <<endl;
    }


    vector<Vector3i> Process::findTriangles(int q, Vector2d s)
    {
        vector<int> listTriangleIDs;
        vector<Vector3i> ABC;
        
        for(vector<int>::const_iterator idF = TQ[q].begin(); idF != TQ[q].end(); ++idF)
        {
            Vector2d _A = C.Vpar.row(M.F(*idF,0));
            Vector2d _B = C.Vpar.row(M.F(*idF,1));
            Vector2d _C = C.Vpar.row(M.F(*idF,2));
            if (isLeft(_A, _B, s) &&
                isLeft(_B, _C, s) &&
                isLeft(_C, _A, s) )
            {
                listTriangleIDs.push_back(*idF);
                ABC.push_back(Vector3i(M.F(*idF,0), M.F(*idF,1), M.F(*idF,2) ));
                //return Vector3i(F(idF,0), F(idF,1), F(idF,2) );
            }
        }
        
        /*if (listTriangleIDs.size() > 1)
        {
            cout <<"idQuad["<<q<<"] triangles found "<< listTriangleIDs.size() << " ( ";
             for(vector<int>::const_iterator f = listTriangleIDs.begin(); f != listTriangleIDs.end(); ++f)
                cout << *f << "; ";
            cout << ") for s " << s(0) << " " << s(1) << endl; 
        }
        else if (listTriangleIDs.size() == 0)
            cout <<"["<<q<<"] ( NON TROVATO NIENTE "
                  << ") for s " << s(0) << " " << s(1) << endl;
        else
        {
            //cout << _A(0)<<" "<<_A(1)<<"---"<<_B(0)<<" "<<_B(1)<<"---"<<_C(0)<<" "<<_C(1)<<endl;  
        }*/

        storeSampleTriangles[q].insert(std::make_pair(s, listTriangleIDs) );
        
        return ABC;
    }


    bool Process::isLeft(Vector2d P0, Vector2d P1, Vector2d s)
    {
        Matrix2d m;
        m << P1(0)-P0(0), s(0)-P0(0),
             P1(1)-P0(1), s(1)-P0(1);
        return m.determinant() > 0;
        //return (  (P1(0) - P0(0)) *  (s(1)  - P0(1))
        //        - (s(0)  - P0(0)) *  (P1(1) - P0(1)) ) >= 0;
    }

    double Process::areaTriangle(Vector2d A, Vector2d B, Vector2d C)
    {
        double a = ( 
                (B(0) - A(0) )*(C(1) - A(1) ) -
                (C(0) - A(0) )*(B(1) - A(1) )
            ) / 2;
    return (a > 0.0) ? a : -a;
    }

    double Process::areaQuad(int q)
    {
        Vector3d _A = C.V.row(C.Q(q, 0));
        Vector3d _B = C.V.row(C.Q(q, 1));
        Vector3d _C = C.V.row(C.Q(q, 2));
        Vector3d _D = C.V.row(C.Q(q, 3));
       
        double ABC = ( (((_C-_A).cross(_C-_B))).norm() )/2.0;
        double ACD = ( (((_C-_A).cross(_C-_D))).norm() )/2.0;
        //cout <<q<<". area = "<< ABC+ACD <<endl;
        return ABC+ACD;
       

    }

    double Process::errorSample(int q, Vector2d s)
    {
        vector<Vector3i> triangles = findTriangles(q, s);
        if (triangles.size() == 0)
        {
            orphanSample.push_back(s);
            //if ( (s(0) != 0.8 && s(0) != 0.2) && (s(1) != 0.8 && s(1) != 0.2) )
            //cout << "Triangle not found: quad["<<q<<"]" << "---" << s(0)<<"-"<<s(1)<<endl;
            return 0;
        }
        double distance = 0;
        for (int i = 0; i < triangles.size(); i++)
        {
            Vector3i ABC = triangles[i];
            Vector2d _A = C.Vpar.row(ABC(0));
            Vector2d _B = C.Vpar.row(ABC(1));
            Vector2d _C = C.Vpar.row(ABC(2));
            
            double alpha = areaTriangle(s,_B,_C)/areaTriangle(_A,_B,_C);
            double beta  = areaTriangle(_A,s,_C)/areaTriangle(_A,_B,_C);
            double gamma = areaTriangle(_A,_B,s)/areaTriangle(_A,_B,_C);

            Vector3d VA = M.V.row(ABC(0));
            Vector3d VB = M.V.row(ABC(1));
            Vector3d VC = M.V.row(ABC(2));
            Vector3d Vs = alpha * VA + beta * VB + gamma * VC;
            distance += computeDistance(Vs, C.getVMapping(q, s));
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
        
       
        return E * areaQuad(q) / (r*c);
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
            //initErrorsAndRelations();
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
                C.split(*q, e, (e+2)%4);
            }
            
            initErrorsAndRelations();
            cout << " ----------- End  ---------------"<<endl;
            // Reinitialize relation adj.
            // moveCage towards mesh triangle
            
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

    double Process::normalizeDistance(double d, double min, double max)
    {
        return (d-min)/(max-min);
    }

    double Process::computeDistance(Vector3d v, Vector3d v_map)
    {
        return (v-v_map).norm();
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
            int quality = C.QVpar(i);
            Vector2d p = C.Vpar.row(i);
            mapV.row(i) = C.getVMapping(quality, p);
            distancesMeshCage[i] = computeDistance(V.row(i), C.getVMapping(quality, p));
        
        }
        
        min = distancesMeshCage.minCoeff();
        max = distancesMeshCage.maxCoeff();
        for (unsigned int i = 0; i < V.rows(); i++)
            distancesMeshCage[i] = normalizeDistance(distancesMeshCage[i], min, max);
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
        C.Vpar = MatrixXd (vnum, 2);
        C.QVpar = VectorXi (vnum);

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
            C.Vpar.row(i) = Vector2d(u,v);
            C.QVpar[i] = quality;

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
