#include "DrawMesh.hh"

    DrawMesh::DrawMesh(){}
    DrawMesh::~DrawMesh(){}

    double* DrawMesh::getCenter(){
        if (!initObject) 
            throw runtime_error("Error: Call function bb() before!");
        return center;
    }
    double DrawMesh::getDiagonal(){
        if (!initObject) 
            throw runtime_error("Error: Call function bb() before!");
        return diagonal;
    }
    void DrawMesh::bb (MatrixXd MeshV, MatrixXi MeshF)
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
        initObject = true;
       
    }

    void DrawMesh::setColorError(double d, double *color)
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


    void DrawMesh::drawDebug()
    {
        if (p->sC.sV.size() == 0)
            return;
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef (-center[0], -center[1], -center[2]);
        glPointSize(4.0);

        glBegin (GL_LINES);
        glColor3f(0.5,0.5,0.5);
        vector<pair<int,Vector2d>>::iterator Vi = p->sC.sV.begin();
        vector<pair<int,Vector2d>>::iterator prec = p->sC.sV.begin();
        vector<pair<int,Vector2d>>::iterator it = p->sC.sV.begin();
        advance(it, 1);
        vector<pair<int,Vector2d>>::iterator firstRing = it;
        for (; it!=p->sC.sV.end(); ++it, ++prec)
        {
            //p->sC.printV(it->first);
            glColor3f(1,0,0);
            glVertex3f (Vi->second(0), Vi->second(1), Vi->second(2));
            glVertex3f (prec->second(0), prec->second(1), prec->second(2));
            
            glColor3f(0.5,0.5,0.5);
            glVertex3f (prec->second(0), prec->second(1), prec->second(2));
            glVertex3f (it->second(0), it->second(1), it->second(2));
        }
        glColor3f(1,0,0);
        glVertex3f (Vi->second(0), Vi->second(1), Vi->second(2));
        glVertex3f (prec->second(0), prec->second(1), prec->second(2));
        
            glColor3f(0.5,0.5,0.5);
        glVertex3f (prec->second(0), prec->second(1), prec->second(2));
        glVertex3f (firstRing->second(0), firstRing->second(1), firstRing->second(2));
        
        glEnd();


        glBegin (GL_POINTS);
        glColor3f(1, 0,0);

        it = p->sC.sV.begin();

        glVertex3f (p->C.V(it->first, 0), p->C.V(it->first, 1), p->C.V(it->first, 2));
        advance(it, 1);
        glColor3f(0, 1,0);
        for (; it!=p->sC.sV.end(); ++it, ++prec)
        {
            glVertex3f (p->C.V(it->first, 0), p->C.V(it->first, 1), p->C.V(it->first, 2));
        }


        glEnd();

        glPopMatrix();

    }


void DrawMesh::drawLinesVmapping()
{
    if (!showLines) 
        return;
    glBegin (GL_LINES);
    glColor3f(0.5,0.5,0.5);

    for (unsigned int i = 0; i < p->M.V.rows(); i++){
        if(IDQuad != -1 && IDQuad != p->C.QVmesh[i]) 
            continue;
        glVertex3f (p->M.V(i,0), p->M.V(i,1), p->M.V(i,2));
        glVertex3f (p->mapV(i,0), p->mapV(i,1), p->mapV(i,2));
    }
    glEnd();
}


void DrawMesh::drawMesh (draw_mode_t mode)
    { 
        drawDebug();
        return;
        MatrixXd V = p->M.V;
        MatrixXi F = p->M.F;  
        VectorXd distV = p->distancesMeshCage;
        VectorXi QVmesh = p->C.QVmesh;

        double* colorError[3];
        colorError[0] = new double[3];
        colorError[1] = new double[3];
        colorError[2] = new double[3];

        int i0,i1,i2;

        glColor3f (0.0,0.0,0.0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef (-center[0], -center[1], -center[2]);
        glPointSize(4.0);
        Vector3f color (0,1,0);       // generic mesh color

    if (mode == SMOOTH || mode == FLAT)
    {
        glEnable (GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        if (mode == SMOOTH) glShadeModel(GL_SMOOTH);
        else if (mode == FLAT) glShadeModel(GL_FLAT); 
        glBegin (GL_TRIANGLES); 
        glColor3f(0,1,0);
        
                
        if(IDQuad != -1 && IDQuad < p->TQ.size() )
        {
            int q = IDQuad;
            for(vector<int>::const_iterator idF = p->TQ[q].begin(); idF != p->TQ[q].end(); ++idF)
            {
                int i = *idF;

                i0 = F(i,0);
                i1 = F(i,1);
                i2 = F(i,2);
                 Vector3d v0 (V(i0,0), V(i0,1), V(i0,2));
                Vector3d v1 (V(i1,0), V(i1,1), V(i1,2));
                Vector3d v2 (V(i2,0), V(i2,1), V(i2,2));
                Vector3d u,v,n;
                u = v1 - v0;
                v = v2 - v0;
                n = (u.cross(v));
                glNormal3f (n(0), n(1), n(2));

                double d0, d1, d2;

                d0 = distV(i0);
                d1 = distV(i1);
                d2 = distV(i2);

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
                
            }
        }
        else
                for (int i = 0; i < F.rows(); i++)
            {

                        
                if(IDTriangle != -1 && IDTriangle < p->M.F.rows() && IDTriangle != i)
                {
                    continue;
                }

                if (F.cols() != 3 || V.cols() != 3)
                    fprintf (stderr, "VF:draw(): only triangles in 3D are supported\n");

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
                //1684
                //1701 = nV = 11

                Vector3d u,v,n;
                u = v1 - v0;
                v = v2 - v0;
                n = (u.cross(v));
                glNormal3f (n(0), n(1), n(2));

                double d0, d1, d2;

                d0 = distV(i0);
                d1 = distV(i1);
                d2 = distV(i2);
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
            }
        glEnd(); 
    }
    /*glEnable (GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_FLAT); 
    glBegin (GL_TRIANGLES); 
    glColor3f(0,1,0);
    for(vector<int>::const_iterator idT = p->debugPartialTQ.begin(); idT != p->debugPartialTQ.end(); ++idT )
    {
        i0 = F(*idT,0);
        i1 = F(*idT,1);
        i2 = F(*idT,2);

        Vector3d v0 (V(i0,0), V(i0,1), V(i0,2));
        Vector3d v1 (V(i1,0), V(i1,1), V(i1,2));
        Vector3d v2 (V(i2,0), V(i2,1), V(i2,2));

        glColor3f(colorError[0][0],colorError[0][1],colorError[0][2]);
        glVertex3f (v0(0), v0(1), v0(2));
        glColor3f(colorError[1][0],colorError[1][1],colorError[1][2]);
        glVertex3f (v1(0), v1(1), v1(2));
        glColor3f(colorError[2][0],colorError[2][1],colorError[2][2]);
        glVertex3f (v2(0), v2(1), v2(2));
    } 
    glEnd(); */

    drawLinesVmapping();
    if (mode == POINTS)
    {
        glDisable (GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glBegin (GL_POINTS);

        for (unsigned int i = 0; i < V.rows(); i++){
            
            if(IDQuad != -1 && IDQuad != QVmesh[i]) continue;
            glColor3f(1,0,0);
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


        if(IDQuad != -1 && IDQuad < p->TQ.size() )
        {
            int q = IDQuad;
            for(vector<int>::const_iterator idF = p->TQ[q].begin(); idF != p->TQ[q].end(); ++idF)
            {
                int i = *idF;


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
        }
        else
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

    void DrawMesh::drawSamplePointWithHisTriangle(int q, Vector2d s, int t)
    {
        
            int i0 = p->M.F(t,0);
            int i1 = p->M.F(t,1);
            int i2 = p->M.F(t,2);
            Vector3d v0 (p->M.V(i0,0), p->M.V(i0,1), p->M.V(i0,2));
            Vector3d v1 (p->M.V(i1,0), p->M.V(i1,1), p->M.V(i1,2));
            Vector3d v2 (p->M.V(i2,0), p->M.V(i2,1), p->M.V(i2,2));
            

            glDisable (GL_DEPTH_TEST);
                glDisable(GL_LIGHTING);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glShadeModel(GL_FLAT); 
                glBegin (GL_TRIANGLES); 

                glColor4f(0.5,1,0.5, 0.4);
                glVertex3f (p->mapV(i0,0), p->mapV(i0,1), p->mapV(i0,2));
                glVertex3f (p->mapV(i1,0), p->mapV(i1,1), p->mapV(i1,2));
                glVertex3f (p->mapV(i2,0), p->mapV(i2,1), p->mapV(i2,2));
        
            glEnd();

            glDisable (GL_DEPTH_TEST);
            glDisable(GL_LIGHTING);
            glBegin (GL_LINES);
            glColor3f(1, 0.4, 0);

                    // triangle
            glVertex3f (v0(0), v0(1), v0(2));
            glVertex3f (v1(0), v1(1), v1(2));
            glVertex3f (v0(0), v0(1), v0(2));
            glVertex3f (v2(0), v2(1), v2(2));
            glVertex3f (v1(0), v1(1), v1(2));
            glVertex3f (v2(0), v2(1), v2(2));

            glColor3f(0.5,0.5,0.5);
            
            glVertex3f (v0(0), v0(1), v0(2));
            glVertex3f (p->mapV(i0,0), p->mapV(i0,1), p->mapV(i0,2));
            glVertex3f (v1(0), v1(1), v1(2));
            glVertex3f (p->mapV(i1,0), p->mapV(i1,1), p->mapV(i1,2));
            glVertex3f (v2(0), v2(1), v2(2));
            glVertex3f (p->mapV(i2,0), p->mapV(i2,1), p->mapV(i2,2));

        glEnd();

        glDisable (GL_DEPTH_TEST);
            glDisable(GL_LIGHTING);
            glBegin(GL_POINTS);

            glColor3f(0.8, 0.4, 0);
            Vector3d pt = p->C.getVMapping(q, s);
            glColor3f(1,0,0);
            glVertex3f (pt(0), pt(1), pt(2));
        glEnd();
    }
    void DrawMesh::drawGrid()
    {
        if (IDQuad == -1 || 
            p->storeSampleTriangles.size() == 0) return;

        Vector3d s;
        MatrixXi F = p->C.Q;
 // same color for all points
            
         for (int q = 0; q < F.rows(); q++)
        {
            if(IDQuad != q) continue;
            
            map<Vector2d, vector<int>, Process::classcomp>::iterator it = p->storeSampleTriangles[q].begin();
            for (it=p->storeSampleTriangles[q].begin(); it!=p->storeSampleTriangles[q].end(); ++it)
                for(vector<int>::const_iterator t= (it->second).begin(); t != (it->second).end(); ++t)
                        drawSamplePointWithHisTriangle(q, (it->first), *t);
        }
    }

    void DrawMesh::drawCage (draw_mode_t mode)
    {
        
        MatrixXd V = p->C.V;
        MatrixXi F = p->C.Q;
        Polychords pc = p->P;
        glColor3f (0.0,0.0,0.0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef (-center[0], -center[1], -center[2]);
        glPointSize(4.0);
        Vector3f color (0.5,0.5,1.0);       // generic mesh color
      if (showGrid)
            drawGrid();  
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
                if(IDQuad != -1 && IDQuad != i) 
                    continue;
                int i0,i1,i2,i3;
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
            
            
            
            if (IDPolychord != -1 && IDPolychord < pc.getSize())
                {
                    glColor3f(1,0,0);
            
                glShadeModel(GL_FLAT);
         
                glBegin (GL_QUADS);
                    for(vector<int>::const_iterator it = pc.P[IDPolychord].begin(); it != pc.P[IDPolychord].end(); ++it)
                    {
                        int i0,i1,i2,i3;
                        i0 = i1 = i2 = i3 = 0;
                        int i = *it;
                        i0 = F(i,0);
                        i1 = F(i,1);
                        i2 = F(i,2);
                        i3 = F(i,3);
                        
                        Vector3d v0 (V(i0,0), V(i0,1), V(i0,2));
                        Vector3d v1 (V(i1,0), V(i1,1), V(i1,2));
                        Vector3d v2 (V(i2,0), V(i2,1), V(i2,2));
                        Vector3d v3 (V(i3,0), V(i3,1), V(i3,2));
                        

                        glVertex3f (v0(0), v0(1), v0(2));
                        glVertex3f (v1(0), v1(1), v1(2));
                        glVertex3f (v1(0), v1(1), v1(2));
                        glVertex3f (v2(0), v2(1), v2(2));
                        glVertex3f (v2(0), v2(1), v2(2));
                        glVertex3f (v3(0), v3(1), v3(2));
                        glVertex3f (v3(0), v3(1), v3(2));
                        glVertex3f (v0(0), v0(1), v0(2));

                
                    }
                }
            else
            {
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
                

                glVertex3f (v0(0), v0(1), v0(2));
                glVertex3f (v1(0), v1(1), v1(2));
                glVertex3f (v1(0), v1(1), v1(2));
                glVertex3f (v2(0), v2(1), v2(2));
                glVertex3f (v2(0), v2(1), v2(2));
                glVertex3f (v3(0), v3(1), v3(2));
                glVertex3f (v3(0), v3(1), v3(2));
                glVertex3f (v0(0), v0(1), v0(2));

                
                }
            }
            glEnd();            
        }

        glPopMatrix();
        
        return; 
    }
