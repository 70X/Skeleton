#include "Process.hh"



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
        MatrixXd V = m.V;
        distancesMeshCage = VectorXd(V.rows());
        /*
        bool normalized = false;
        Vector2d maxPar = c.ParV_2D.colwise().maxCoeff();
        if (maxPar(0) >= 0 && maxPar(1) <= 1)
            normalized = true;
        cout << "normalized: "<< normalized << endl;
        */
        for (unsigned int i = 0; i < V.rows(); i++)
            distancesMeshCage[i] = computeDistance(V.row(i), c.getVMapping(i));
        
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
            fprintf (stdout, "Loaded mesh `%s\'.\n#vertices: %d.\n#faces: %d.\n", str, (int)c.V.rows(), (int)c.Q.rows());
        } else if (!strcmp(p, ".ply") || !strcmp(p, ".PLY"))
        {
            readPLY (str);
            fprintf (stdout, "Loaded mesh `%s\'.\n#vertices: %d.\n#faces: %d.\n", str, (int)m.V.rows(), (int)m.F.rows());
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
    
        c.V = MatrixXd (vnum, 3);
        c.Q = MatrixXi (fnum, 4);

        for (int i = 0; i < c.V.rows(); i++)
            r = fscanf (fp, "%lf %lf %lf\n", &(c.V)(i,0), &(c.V)(i,1), &(c.V)(i,2));    
        for (int i = 0; i < c.Q.rows(); i++)
            r = fscanf (fp, "4 %d %d %d %d\n", &(c.Q)(i,0), &(c.Q)(i,1), &(c.Q)(i,2), &(c.Q)(i,3));
        for (int i = 0; i < c.Q.rows(); i++)
            for (int j = 0; j < 4; j++)
                if (c.Q(i,j) >= c.V.rows())
                    fprintf (stderr, "readOFF(): warning vertex: %d"
                             " in face: %d has value: %d, greater than #v: %d\n",
                             j,i,c.Q(i,j),(int)c.V.rows());
        fclose (fp);
		r=r; // remove warnings
        c.computeQQ();
        
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

        m.V = MatrixXd (vnum, 3);
        m.F = MatrixXi (fnum, 3);
        c.ParV = MatrixXd (vnum, 2);
        c.ParQ = VectorXi (vnum);

        for(int i=0; i<vnum; i++)
        {
            getline(fp, line);
            stringstream linestream(line);
            if(flag)
                linestream >> x >> y >> z >> flags >> quality >>
                                    u >> v;
            else linestream >> x >> y >> z >> quality >> 
                                    u >> v;
            m.V.row(i) = Vector3d(x,y,z);
            c.ParV.row(i) = Vector2d(u,v);
            c.ParQ[i] = quality;

        }
        int tmp, v0,v1,v2; // 3
        for(int i=0; i<fnum; i++)
        {
            getline(fp, line);
            stringstream linestream(line);
            linestream >> tmp >> v0 >> v1 >> v2;
            m.F.row(i) = Vector3i(v0,v1,v2);
        }
        
    }
    