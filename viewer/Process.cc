#include "Process.hh"

    VectorXd Process::computeQuadsError()
    {
        VectorXd QuadsError(C.Q.rows());
        for (unsigned int i=0; i<C.Vpar.rows(); i++)
            QuadsError[C.QVpar[i]] += distancesMeshCage[i];

        // print
        /*for (unsigned int i=0; i<QuadsError.rows(); i++)
            cout << "q = " <<i<<" = "<<QuadsError[i] << endl; 
        */
        return QuadsError;
    }
    void Process::raffinementQuadLayout()
    {
        while(1)
        {
            VectorXd QuadsError = computeQuadsError();
            double maxError = 0;
            int worstPolychord = -1;
            // Find polychord with the greatest error
            for (unsigned int id=0; id<P.getSize(); id++)
            {
                double currentError = 0;
                for(vector<int>::const_iterator q = P.P[id].begin(); q != P.P[id].end(); ++q)
                    currentError += QuadsError[*q];
                
                //cout << "id: "<<id<<" error: "<<currentError<<endl;
                if (currentError > maxError)
                {
                    maxError = currentError;
                    worstPolychord = id;
                }
            }
            cout << worstPolychord << endl;
            break;
            // Split polychord found and cage 
            // Reinitialize relation adj.
            // moveCage towards mesh triangle
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
            distancesMeshCage[i] = computeDistance(V.row(i), C.getVMapping(i));
        
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
    