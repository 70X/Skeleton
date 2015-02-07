#include "Mesh.hh"

    Mesh::Mesh(){}
    Mesh::~Mesh(){}
////////////////////// I/O ///////////////////////////////////////
    void Mesh::read(char *str)
    {
        cout << str <<endl;
        char* p;
        for (p = str; *p != '\0'; p++) ;
        while (*p != '.') p--;
        if (!strcmp(p, ".off") || !strcmp(p, ".OFF"))
        {
            readOFF (str);
            fprintf (stdout, "Loaded mesh `%s\'.\n#vertices: %d.\n#faces: %d.\n", str, (int)CageV.rows(), (int)CageF.rows());
        } else if (!strcmp(p, ".ply") || !strcmp(p, ".PLY"))
        {
            readPLY (str);
            fprintf (stdout, "Loaded mesh `%s\'.\n#vertices: %d.\n#faces: %d.\n", str, (int)MeshV.rows(), (int)MeshF.rows());
        }else {
            fprintf (stdout, "File format not supported!\n");
            return;
        }
        
        
    }

    void Mesh::readOFF (char* meshfile)
    {
        int vnum, fnum;
        FILE *fp = fopen (meshfile, "r");

        if (!fp) fprintf (stderr, "readOFF(): could not open file %s", meshfile);

        int r = 0;
        r = fscanf (fp, "OFF\n%d %d 0\n",  &vnum, &fnum);
    
        CageV = MatrixXd (vnum, 3);
        CageF = MatrixXi (fnum, 4);

        for (int i = 0; i < CageV.rows(); i++)
            r = fscanf (fp, "%lf %lf %lf\n", &CageV(i,0), &CageV(i,1), &CageV(i,2));    
        for (int i = 0; i < CageF.rows(); i++)
            r = fscanf (fp, "4 %d %d %d %d\n", &CageF(i,0), &CageF(i,1), &CageF(i,2), &CageF(i,3));
        for (int i = 0; i < CageF.rows(); i++)
            for (int j = 0; j < 4; j++)
                if (CageF(i,j) >= CageV.rows())
                    fprintf (stderr, "readOFF(): warning vertex: %d"
                             " in face: %d has value: %d, greater than #v: %d\n",
                             j,i,CageF(i,j),(int)CageV.rows());
        fclose (fp);
				r=r; // remove warnings
    }

    void Mesh::readPLY (char* meshfile)
    {
        int vnum, fnum;
        //FILE *fp = fopen (meshfile, "r");
        ifstream fp(meshfile);
        if (!fp.is_open()) fprintf (stderr, "readPLY(): could not open file %s", meshfile);

        string line;
        unsigned found;
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
            if (line == "end_header")
                break;
        }
        
        //std::cout << vnum << " " << fnum << endl;

        float x,y,z,u,v,quality;

        MeshV = MatrixXd (vnum, 3);
        mapV = MatrixXd (vnum, 3);
        MeshParV = MatrixXd (vnum, 2);
        MeshF = MatrixXi (fnum, 3);
        MeshParF = VectorXi (vnum);

        for(int i=0; i<vnum; i++)
        {
            getline(fp, line);
            stringstream linestream(line);
            linestream >> x >> y >> z >> quality >>
                u >> v;
            MeshV.row(i) = Vector3d(x,y,z);
            MeshParV.row(i) = Vector2d(u,v);
            MeshParF[i] = quality;

        }
        int tmp, v0,v1,v2; // 3
        for(int i=0; i<fnum; i++)
        {
            getline(fp, line);
            stringstream linestream(line);
            linestream >> tmp >> v0 >> v1 >> v2;
            MeshF.row(i) = Vector3i(v0,v1,v2);
        }
        
    }
    






    double Mesh::computeDistanceCageMesh(int i)
    {
        int quality = MeshParF(i);
        Vector2d p = MeshParV.row(i);
        Vector4i quad = CageF.row(quality);
        Vector3d A(CageV.row(quad[0])),
                B(CageV.row(quad[1])),
                C(CageV.row(quad[2])),
                D(CageV.row(quad[3]));
        Vector3d U = (B-A)/(B-A).norm();
        Vector3d V = (D-A)/(D-A).norm();

        Vector3d v_map = U*p(0) + V*p(1);
        Vector3d v = MeshV.row(i);
        double d = (v-v_map).norm();
        
        mapV.row(i) = v_map;

        //if (p(0) < 0 || p(0) > 1 || p(1) < 0 || p(1) > 1)
        //if (quality == 0 && i == 695)
        {
            cout << "------------------------"<<endl;
            cout << "ith (" << i << ")" << endl;
            cout << "ith face: " << quad(0) << "," <<quad(1) << ","<<quad(2)<<","<<quad(3)<<endl;
            
            cout << "A ("<<A(0) << "," <<A(1) << "," <<A(2) <<")"<< endl;
            cout << "B ("<<B(0) << "," <<B(1) << "," <<B(2) <<")"<< endl;
            cout << "C ("<<C(0) << "," <<C(1) << "," <<C(2) <<")"<<endl;
            cout << "D ("<<D(0) << "," <<D(1) << "," <<D(2) <<")"<< endl;

            cout << "| v(0): "<<v(0) << " - v_map(0): "<<v_map(0) <<"|"<<endl;
            cout << "| v(1): "<<v(1) << " - v_map(1): "<<v_map(1) <<"\t|\t.norm() = "<<d << endl;
            cout << "| v(2): "<<v(2) << " - v_map(2): "<<v_map(2) <<"|"<<endl<<endl;
            cout << "U: ("<<U(0) << "," <<U(1) << "," <<U(2) <<")"<<endl;
            cout << "V: ("<<V(0) << "," <<V(1) << "," <<V(2) <<")"<< endl;
            cout << "uv: "<<p(0)<<"," <<p(1) << endl;
            cout << "------------------------"<<endl;

        }
        /*if (d<10)
        {
            std::cout << "(" <<i<<")" << std::endl;
            std::cout << v << std::endl;
            std::cout << v_map << std::endl;
            std::cout << d << std::endl;
        }*/
        return d;
    }
    void Mesh::distanceBetweenMeshCage()
    {
        double min, max;
        MatrixXd V = MeshV;
        distV = VectorXd(V.rows());
        for (unsigned int i = 0; i < V.rows(); i++)
            distV[i] = computeDistanceCageMesh(i);
        min = distV.minCoeff();
        max = distV.maxCoeff();

        for (unsigned int i = 0; i < V.rows(); i++)
            distV[i] = normalizeDistance(distV[i], min, max);
    }

    double Mesh::normalizeDistance(double d, double min, double max)
    {
        return (d-min)/(max-min);
    }

    void Mesh::debug()
    {
        double min, max;
        min = distV.minCoeff();
        max = distV.maxCoeff();
        cout << "Distance: *Min="<< min << " | *Max=" << max << " " << endl;
        for (int i=0; i<distV.rows(); i++)
        {
            if (min == distV[i])
            {
                cout << "MIN" << endl;
                cout << "Position ("<<i<<") Vertex = ("<<(MeshV.row(i))[0]<<","<<
                (MeshV.row(i))[1]<<","<<(MeshV.row(i))[2]<<")" << 
                " VertexPar = ("<<
                    (MeshParV.row(i))[0]<<","<<(MeshParV.row(i))[1]<<")"<<endl;
                cout << " quality = " << MeshParF[i] << endl<<endl;
            }
            if (max == distV[i])
            {
                cout << "MAX" << endl;
                cout << "Position ("<<i<<") Vertex = ("<<(MeshV.row(i))[0]<<","<<
                (MeshV.row(i))[1]<<","<<(MeshV.row(i))[2]<<")" << 
                " VertexPar = ("<<
                    (MeshParV.row(i))[0]<<","<<(MeshParV.row(i))[1]<<")"<<endl;
                cout << " quality = " << MeshParF[i] << endl<<endl;
            }
        }
        //cout << normalizeDistance(distV[0], min, max) << endl;
    }