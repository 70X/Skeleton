#include "Process.hh"
    
    void Process::updateTQ()
    {
        vector<vector<int>> listTQ(C.Q.rows());
        int count_v_orphan = 0;
        for(int i=0; i<M.F.rows(); i++)
        {
            int i0 = M.F(i,0);
            int i1 = M.F(i,1);
            int i2 = M.F(i,2);
            if (C.VparQ(i0) == C.VparQ(i1)
                && C.VparQ(i1) == C.VparQ(i2))
            {
                listTQ[C.VparQ(i0)].push_back(i);
            }
            else 
            {
                count_v_orphan++;
            }
        }
        cout << "orphan vertices : "<<count_v_orphan <<endl;
        TQ = listTQ;
    }


    Vector3i Process::findTriangle(int q, Vector2d s)
    {
        vector<int> countTimes;
        Vector3i ABC;
        
        for(vector<int>::const_iterator idF = TQ[q].begin(); idF != TQ[q].end(); ++idF)
        {
            Vector2d _A = C.Vpar.row(M.F(*idF,0));
            Vector2d _B = C.Vpar.row(M.F(*idF,1));
            Vector2d _C = C.Vpar.row(M.F(*idF,2));
            if (isInside(_A, _B, s) &&
                isInside(_B, _C, s) &&
                isInside(_C, _A, s) )
            {
                countTimes.push_back(*idF);
                ABC = Vector3i(M.F(*idF,0), M.F(*idF,1), M.F(*idF,2) );
                //return Vector3i(F(idF,0), F(idF,1), F(idF,2) );
            }
        }
        if (countTimes.size() > 1)
        {
            cout <<"["<<q<<"] triangles found "<< countTimes.size() << " ( ";
             for(vector<int>::const_iterator f = countTimes.begin(); f != countTimes.end(); ++f)
                cout << *f << "; ";
            cout << ") for s " << s(0) << " " << s(1) << endl;
        }
        return ABC;
    }


    bool Process::isInside(Vector2d P0, Vector2d P1, Vector2d s)
    {
        Matrix2d m;
        m << P1(0)-P0(0), s(0)-P0(0),
             P1(1)-P0(1), s(1)-P0(1);

        return m.determinant() > 0;
    }

    double Process::areaTriangle(Vector2d A, Vector2d B, Vector2d C)
    {
        double a = ( 
                (B(0) - A(0) )*(C(1) - A(1) ) -
                (C(0) - A(0) )*(B(1) - A(1) )
            ) / 2;
    return (a > 0.0) ? a : -a;
    }

    double Process::computeErrorSample(int q, Vector2d s)
    {
        Vector3i ABC = findTriangle(q, s);
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
        return computeDistance(Vs, C.getVMapping(q, s));
    }

    double Process::computeErrorsGrid(int q, int r, int c)
    {
        double E = 0;
        double domain = PARAMETER_SPACE;
        double step_x = domain/r;
        double step_y = domain/c;
        for (int i=1; i<r; i++)
        {
            for (int j=1; j<c; j++)
                E += computeErrorSample(q, Vector2d(step_x*i, step_y*j));
        }
        return E;
    }
/*
    VectorXd Process::computeQuadsError()
    {
        VectorXd QuadsError(C.Q.rows());
        for (unsigned int i=0; i<C.Vpar.rows(); i++)
            QuadsError[C.VparQ[i]] += distancesMeshCage[i];
        return QuadsError;
    }*/
    VectorXd Process::computeErrorPolychords()
    {
        updateTQ();
        double E;
        VectorXd polychordsError(P.getSize());
        for (int i=0; i<P.getSize(); i++)
        {
            E = 0;
            for(vector<int>::const_iterator q = P.P[i].begin(); q != P.P[i].end(); ++q)
            {
                E += computeErrorsGrid(*q);
            }
            polychordsError[i] = E*((double) P.P[i].size() / (double) C.Q.rows() );
        }
        return polychordsError;
    }
    void Process::raffinementQuadLayout()
    {
        int times = 0;
        while(times < 3)
        {
            cout << " -----------iteration: "<< times << " ---------------"<<endl;
            
            VectorXd polychordsError = computeErrorPolychords();
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
            //worstPolychord = 1;
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
            C.computeQQ();
            P.computePolychords(); 
            cout << endl;
            // Reinitialize relation adj.
            // moveCage towards mesh triangle
            
            /*
            for (int i=0; i<C.Q.rows(); i++)
                cout << "["<<i<<"]"<< C.Q.row(i)<<endl;
            cout << "QQ" <<endl;
            for(int i=0; i<C.QQ.rows(); i++)
                cout << "["<<i<<"]"<< C.QQ.row(i) << endl;
            cout << endl;*/
            times++;
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
        for (unsigned int i = 0; i < V.rows(); i++)
        {
            int quality = C.VparQ(i);
            Vector2d p = C.Vpar.row(i);
            distancesMeshCage[i] = computeDistance(V.row(i), C.getVMapping(quality, p));
        }
        
        min = distancesMeshCage.minCoeff();
        max = distancesMeshCage.maxCoeff();
        for (unsigned int i = 0; i < V.rows(); i++)
            distancesMeshCage[i] = normalizeDistance(distancesMeshCage[i], min, max);
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
        C.VparQ = VectorXi (vnum);

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
            C.VparQ[i] = quality;

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
    