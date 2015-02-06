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
        cagenumF = fnum;
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
    void Mesh::setColorError(double d, double *color)
    {
        if (d <= 0.5)
        {
            // y = 2x
            color[2] = 1; 
            color[0] = color[1] = d*2; 
        }
        else if (d > 0.5)
        {
            // y = -2x + 2
            color[0] = 1; 
            color[1] = color[2] = (-2*d)+2;
        }
    }
void Mesh::drawMesh (draw_mode_t mode, double thresholdMin, 
    double thresholdMax)
    {
        MatrixXd V = MeshV;
        MatrixXi F = MeshF;  
        
        double min, max;
        min = distV.minCoeff();
        max = distV.maxCoeff();
        double* colorError[3];
        colorError[0] = new double[3];
        colorError[1] = new double[3];
        colorError[2] = new double[3];

        glColor3f (0.0,0.0,0.0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef (-center[0], -center[1], -center[2]);
        glPointSize(4.0);
        Vector3f color (0,1,0);       // generic mesh color

        VectorXi mpf = MeshParF; 
    if (mode == SMOOTH)
    {
        glEnable (GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glShadeModel(GL_SMOOTH);
        glBegin (GL_TRIANGLES);
            for (int i = 0; i < F.rows(); i++)
            {
                if (F.cols() != 3 || V.cols() != 3)
                    fprintf (stderr, "VF:draw(): only triangles in 3D are supported\n");

                int i0,i1,i2;
                i0 = i1 = i2 = 0;
                if (i0 < 0 || i0 >= V.rows())
                    fprintf (stderr, "VF::draw(): out of boundary F(%d,%d) = %d\n",
                         i,0,F(i,0));
                if (i1 < 0 || i1 >= V.rows())
                    fprintf (stderr, "VF::draw(): out of boundary F(%d,%d) = %d\n",
                         i,1,F(i,1));
                if (i2 < 0 || i2 >= V.rows())
                    fprintf (stderr, "VF::draw(): out of boundary F(%d,%d) = %d\n",
                         i,2,F(i,2));
                // indexes of vertices of face i
                i0 = F(i,0);
                i1 = F(i,1);
                i2 = F(i,2);
                Vector3d v0 (V(i0,0), V(i0,1), V(i0,2));
                Vector3d v1 (V(i1,0), V(i1,1), V(i1,2));
                Vector3d v2 (V(i2,0), V(i2,1), V(i2,2));
                double d0, d1, d2;

                d0 = normalizeDistance(distV(i0), min, max);
                d1 = normalizeDistance(distV(i1), min, max);
                d2 = normalizeDistance(distV(i2), min, max);
                //if (d0>thresholdMin && d0 < thresholdMax)
                //{
                //cout << d0 << " " << d1 << " " << d2 << endl;
                setColorError(d0, colorError[0]);
                setColorError(d1, colorError[1]);
                setColorError(d2, colorError[2]);
                glColor3f(colorError[0][0],colorError[0][1],colorError[0][2]);
                glVertex3f (v0(0), v0(1), v0(2));
                glColor3f(colorError[1][0],colorError[1][1],colorError[1][2]);
                glVertex3f (v1(0), v1(1), v1(2));
                glColor3f(colorError[2][0],colorError[2][1],colorError[2][2]);
                glVertex3f (v2(0), v2(1), v2(2));
                //}

            }
        glEnd(); 
    }
    if (mode == FLAT )
    {

        glEnable (GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glShadeModel(GL_FLAT); 
         
        glBegin (GL_TRIANGLES);
            glColor3f(color(0),color(1),color(2));  // same color for all triangles
            for (int i = 0; i < F.rows(); i++)
            {
                if (F.cols() != 3 || V.cols() != 3)
                    fprintf (stderr, "VF:draw(): only triangles in 3D are supported\n");

                int i0,i1,i2;
                i0 = i1 = i2 = 0;
                if (i0 < 0 || i0 >= V.rows())
                    fprintf (stderr, "VF::draw(): out of boundary F(%d,%d) = %d\n",
                         i,0,F(i,0));
                if (i1 < 0 || i1 >= V.rows())
                    fprintf (stderr, "VF::draw(): out of boundary F(%d,%d) = %d\n",
                         i,1,F(i,1));
                if (i2 < 0 || i2 >= V.rows())
                    fprintf (stderr, "VF::draw(): out of boundary F(%d,%d) = %d\n",
                         i,2,F(i,2));
                // indexes of vertices of face i
                i0 = F(i,0);
                i1 = F(i,1);
                i2 = F(i,2);
                
                // coords of vertices of face i
                Vector3d v0 (V(i0,0), V(i0,1), V(i0,2));
                Vector3d v1 (V(i1,0), V(i1,1), V(i1,2));
                Vector3d v2 (V(i2,0), V(i2,1), V(i2,2));
                // normal per face (flat shading)
                Vector3d u,v,n;
                u = v1 - v0;
                v = v2 - v0;
                n = (u.cross(v));

                glNormal3f (n(0), n(1), n(2));
                //cout << "Vertices: "<<v0<<","<<v1<<","<<v2<<endl;
                // triangle
                
                glVertex3f (v0(0), v0(1), v0(2));
                glVertex3f (v1(0), v1(1), v1(2));
                glVertex3f (v2(0), v2(1), v2(2));

            }
        glEnd();            
    }

    if (mode == POINTS)
    {
        glDisable (GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glBegin (GL_POINTS);

        double d;
        for (unsigned int i = 0; i < V.rows(); i++){
            
            if(onlyFace != -1 && onlyFace != mpf[i]) continue;
            d = normalizeDistance(distV[i], min, max);
            if (d>thresholdMin && d < thresholdMax)
            {
                glColor3f(1,0,0);
                glVertex3f (V(i,0), V(i,1), V(i,2));
            }
            /*else if (mode==POINTS)
            {
                glColor3f(color(0),color(1),color(2));
                glVertex3f (V(i,0), V(i,1), V(i,2));
            }*/

        }
        glEnd();                    
    }
     glBegin (GL_LINES);

            double d0;
        for (unsigned int i = 0; i < V.rows(); i++){
            if(onlyFace != -1 && onlyFace != mpf[i]) continue;
            d0 = normalizeDistance(distV[i], min, max);
            if (d0>thresholdMin && d0 < thresholdMax)
            {
                glColor3f(0,1,0);
                glVertex3f (V(i,0), V(i,1), V(i,2));
                glVertex3f (mapV(i,0), mapV(i,1), mapV(i,2));
            }
        }
    glEnd();

    if (mode == WIRE)
    {
        glDisable (GL_DEPTH_TEST);
        glDisable (GL_LIGHTING);
        glColor3f(color(0),color(1),color(2)); // same color for all edges
        
        glBegin (GL_LINES);
        for (int i = 0; i < F.rows(); i++)
        {
        int i0,i1,i2;
        i0 = i1 = i2 = 0;

        i0 = F(i,0);
        i1 = F(i,1);
        i2 = F(i,2);
        
        Vector3d v0 (V(i0,0), V(i0,1), V(i0,2));
        Vector3d v1 (V(i1,0), V(i1,1), V(i1,2));
        Vector3d v2 (V(i2,0), V(i2,1), V(i2,2));
        
        // triangle
        glVertex3f (v0(0), v0(1), v0(2));
        glVertex3f (v1(0), v1(1), v1(2));
        glVertex3f (v0(0), v0(1), v0(2));
        glVertex3f (v2(0), v2(1), v2(2));
        glVertex3f (v1(0), v1(1), v1(2));
        glVertex3f (v2(0), v2(1), v2(2));
        }
        glEnd();            
    }

    glPopMatrix();
    return; 
    }






    void Mesh::drawCage(draw_mode_t mode)
    {
        MatrixXd V = CageV;
        MatrixXi F = CageF;
        glColor3f (0.0,0.0,0.0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef (-center[0], -center[1], -center[2]);
        glPointSize(4.0);
        Vector3f color (0.5,0.5,1.0);       // generic mesh color
        
    if ( mode == FLAT || mode == SMOOTH)
    {
        glEnable (GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        // if (mode == SMOOTH)
        //  glShadeModel(GL_SMOOTH);
        if (mode == FLAT)
        glShadeModel(GL_FLAT);
         
        glBegin (GL_QUADS);
            glColor3f(color(0),color(1),color(2));  // same color for all triangles
            
            for (int i = 0; i < F.rows(); i++)
            {
                if(onlyFace != -1 && onlyFace != i) continue;
                if (F.cols() != 4 || V.cols() != 3)
                    fprintf (stderr, "VF:draw(): only cube in 3D are supported\n");

                int i0,i1,i2,i3;
                i0 = i1 = i2 = i3 = 0;
                if (i0 < 0 || i0 >= V.rows())
                    fprintf (stderr, "VF::draw(): out of boundary F(%d,%d) = %d\n",
                         i,0,F(i,0));
                if (i1 < 0 || i1 >= V.rows())
                    fprintf (stderr, "VF::draw(): out of boundary F(%d,%d) = %d\n",
                         i,1,F(i,1));
                if (i2 < 0 || i2 >= V.rows())
                    fprintf (stderr, "VF::draw(): out of boundary F(%d,%d) = %d\n",
                         i,2,F(i,2));
                if (i3 < 0 || i3 >= V.rows())
                    fprintf (stderr, "VF::draw(): out of boundary F(%d,%d) = %d\n",
                         i,3,F(i,3));
                // indexes of vertices of face i
                i0 = F(i,0);
                i1 = F(i,1);
                i2 = F(i,2);
                i3 = F(i,3);
                
                // coords of vertices of face i
                Vector3d v0 (V(i0,0), V(i0,1), V(i0,2));
                Vector3d v1 (V(i1,0), V(i1,1), V(i1,2));
                Vector3d v2 (V(i2,0), V(i2,1), V(i2,2));
                Vector3d v3 (V(i3,0), V(i3,1), V(i3,2));
                //std::cout << v0 << "," << v1<<","<<v2<<","<<v3<<std::endl;
                // normal per face (flat shading)
                
                Vector3d u,v,n;
                u = v1 - v0;
                v = v3 - v1;
                n = (u.cross(v));

                glNormal3f (n(0), n(1), n(2));
                
                // triangle
                glVertex3f (v0(0), v0(1), v0(2));
                glVertex3f (v1(0), v1(1), v1(2));
                glVertex3f (v2(0), v2(1), v2(2));
                glVertex3f (v3(0), v3(1), v3(2));
                
                //break;
            }
            glEnd();            
        }

        if (mode == POINTS)
        {
            glDisable (GL_DEPTH_TEST);
            glDisable(GL_LIGHTING);
            glBegin (GL_POINTS);

            glColor3f(color(0),color(1),color(2)); // same color for all points
            for (unsigned int i = 0; i < V.rows(); i++){
                glVertex3f (V(i,0), V(i,1), V(i,2));
            }
            glEnd();                    
        }

        if (mode == WIRE)
        {
            glDisable (GL_DEPTH_TEST);
            glDisable (GL_LIGHTING);
            glColor3f(color(0),color(1),color(2)); // same color for all edges
            
            glBegin (GL_LINES);
            for (int i = 0; i < F.rows(); i++)
            {
                
            int i0,i1,i2,i3;
            i0 = i1 = i2 = i3 = 0;

            i0 = F(i,0);
            i1 = F(i,1);
            i2 = F(i,2);
            i3 = F(i,3);
            
            Vector3d v0 (V(i0,0), V(i0,1), V(i0,2));
            Vector3d v1 (V(i1,0), V(i1,1), V(i1,2));
            Vector3d v2 (V(i2,0), V(i2,1), V(i2,2));
            Vector3d v3 (V(i3,0), V(i3,1), V(i3,2));
            
            // triangle

            glVertex3f (v0(0), v0(1), v0(2));
            glVertex3f (v1(0), v1(1), v1(2));
            glVertex3f (v1(0), v1(1), v1(2));
            glVertex3f (v2(0), v2(1), v2(2));
            glVertex3f (v2(0), v2(1), v2(2));
            glVertex3f (v3(0), v3(1), v3(2));
            glVertex3f (v3(0), v3(1), v3(2));
            glVertex3f (v0(0), v0(1), v0(2));
            //if (i==10) break;
            /*
            glVertex3f (v0(0), v0(1), v0(2));
            glVertex3f (v1(0), v1(1), v1(2));
            glVertex3f (v0(0), v0(1), v0(2));
            glVertex3f (v2(0), v2(1), v2(2));
            glVertex3f (v1(0), v1(1), v1(2));
            glVertex3f (v2(0), v2(1), v2(2));
            */
            
            }
            glEnd();            
        }

        glPopMatrix();
        return; 
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
        /*if (quality == 0 && i == 695)
        {
            cout << "------------------------"<<endl;
            cout << "(" << i << ")" << endl;
            cout << quad(0) << "," <<quad(1) << ","<<quad(2)<<","<<quad(3)<<endl;
            
            cout << A(0) << "," <<A(1) << "," <<A(2) << endl;
            cout << B(0) << "," <<B(1) << "," <<B(2) << endl;
            cout << C(0) << "," <<C(1) << "," <<C(2) << endl;
            cout << D(0) << "," <<D(1) << "," <<D(2) << endl;

            cout << v(0) << " - "<<v_map(0) <<endl;
            cout << v(1) << " - "<<v_map(1) <<".norm() = "<<d << endl;
            cout << v(2) << " - "<<v_map(2) <<endl;
            cout << U(0) << "," <<U(1) << "," <<U(2) << endl;
            cout << V(0) << "," <<V(1) << "," <<V(2) << endl;
            cout << "------------------------"<<endl;

        }*/
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
        MatrixXd V = MeshV;
        distV = VectorXd(V.rows());
        for (unsigned int i = 0; i < V.rows(); i++)
            distV[i] = computeDistanceCageMesh(i);

    }
    void Mesh::bb ()
    {

        MatrixXd V = MeshV;
        MatrixXi F = MeshF;
        VectorXd min, max;
        VectorXd vec;

        min = VectorXd::Constant(V.cols(),0.0);
        max = VectorXd::Constant(V.cols(),0.0);

        min = V.colwise().minCoeff();
        max = V.colwise().maxCoeff();

        vec = ((max - min) / 2.0);
        vec = (V.colwise().sum()) / (float)(V.rows());

        center[0] = vec(0);
        center[1] = vec(1);
        center[2] = vec(2);
        
        diagonal = (max - min).norm();
       
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
                faceInMinPoint = MeshParF[i];
                cout << "MIN" << endl;
                cout << "Position ("<<i<<") Vertex = ("<<(MeshV.row(i))[0]<<","<<
                (MeshV.row(i))[1]<<","<<(MeshV.row(i))[2]<<")" << 
                " VertexPar = ("<<
                    (MeshParV.row(i))[0]<<","<<(MeshParV.row(i))[1]<<")"<<endl;
                cout << " quality = " << MeshParF[i] << endl<<endl;
            }
            if (max == distV[i])
            {
                faceInMaxPoint = MeshParF[i];
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