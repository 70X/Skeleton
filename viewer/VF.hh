#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#define ON 1
#define OFF 0

#include <Eigen/Core>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFFER_VT_AND_VV 45 //! cylinder mesh has vertices shared by 40 faces

using namespace Eigen;
using namespace std;

#ifndef _VF_CLASS
#define _VF_CLASS

#ifndef MIN
	#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
	#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

class VF
{
public:
    
    VF(){TTVTflag=false;};
    VF(const VF& other){
        TTVTflag=false;
        V=other.V;
        F=other.F;
        bb();
    };
    ~VF(){};


////////////////////// I/O ///////////////////////////////////////

    // read mesh from an ascii file with automatic detection of file
    // format. supported: obj, off)
    void read(char *str)
    {
		char* p;
		for (p = str; *p != '\0'; p++) ;
		while (*p != '.') p--;

		//printf ("%s\n", p);
		if (!strcmp(p, ".obj") || !strcmp(p, ".OBJ"))
		{
			//printf ("readOBJ\n");
			readOBJ (str);
		} else if (!strcmp(p, ".off") || !strcmp(p, ".OFF"))
		{
			//printf ("readOFF\n");
			readOFF (str);
		} else {
            fprintf (stdout, "File format not supported!\n");
            return;
        }

        // topology is NOT initialized
        // initTT();
        TTVTflag = false;
        
		fprintf (stdout, "Loaded mesh `%s\'.\n#vertices: %d.\n#faces: %d.\n", str, (int)V.rows(), (int)F.rows());
        if (!isManifold()) fprintf (stdout, "Mesh is not edge manifold! Cannot use topology.\n"); 
    }


    // write mesh to an ascii file with automatic detection of file
    // format. supported: obj, off)
    void write(char *str)
    {
	char* p;
        for (p = str; *p != '\0'; p++) ;
        while (*p != '.') p--;

        //printf ("%s\n", p);
        if (!strcmp(p, ".obj") || !strcmp(p, ".OBJ"))
        {
            //printf ("writeOBJ\n");
            return writeOBJ (str);
        } else if (!strcmp(p, ".off") || !strcmp(p, ".OFF"))
        {
            //printf ("writeOFF\n");
            return writeOFF (str);
        } else {
            fprintf (stdout, "File format not supported!\n");
            return;
        }
    }


    // Read a mesh from an ascii obj file
    void readOBJ(const char *str)
    {
        ifstream s(str);
        vector<Vector3d> Vtemp;
        vector<Vector3i> Ftemp;
        char buf[1000];
        while(!s.eof())
        {
            s.getline(buf, 1000);
            if (buf[0] == 'v') // vertex coordinates found
            {
                char v;
                double v1,v2,v3;
                sscanf(buf, "%c %lf %lf %lf",&v,&v1,&v2,&v3);
                Vtemp.push_back(Vector3d(v1,v2,v3));
            }
            else if (buf[0] == 'f') // face description found
            {
                char v;
                int v1,v2,v3;
                sscanf(buf, "%c %d %d %d",&v,&v1,&v2,&v3);
                Ftemp.push_back(Vector3i(v1-1,v2-1,v3-1));
            }
        }
        s.close();
        V = MatrixXd(Vtemp.size(),3);
        for(int i=0;i<V.rows();++i) V.row(i) = Vtemp[i];
        F = MatrixXi(Ftemp.size(),3);
        for(int i=0;i<F.rows();++i) F.row(i) = Ftemp[i];
    }
    
	
    // Write a mesh in an ascii obj file
    void writeOBJ(const char *str)
    {
        ofstream s(str);
        for(int i=0;i<V.rows();++i)
            s << "v " << V(i,0) << " " << V(i,1) << " " << V(i,2) << endl;
        for(int i=0;i<F.rows();++i)
            s << "f " << F(i,0)+1 << " " << F(i,1)+1 << " " << F(i,2)+1 << endl;
        s.close();
    }

	
    // read mesh from a ascii off file
    void readOFF (char* meshfile)
    {
        int vnum, fnum;
        FILE *fp = fopen (meshfile, "r");

        if (!fp) fprintf (stderr, "readOFF(): could not open file %s", meshfile);

        int r = 0;
        r = fscanf (fp, "OFF\n%d %d 0\n",  &vnum, &fnum);
    
        V = MatrixXd (vnum, 3);
        F = MatrixXi (fnum, 3);

        for (int i = 0; i < V.rows(); i++)
            r = fscanf (fp, "%lf %lf %lf\n", &V(i,0), &V(i,1), &V(i,2));    
        for (int i = 0; i < F.rows(); i++)
            r = fscanf (fp, "3 %d %d %d\n", &F(i,0), &F(i,1), &F(i,2));
        for (int i = 0; i < F.rows(); i++)
            for (int j = 0; j < 3; j++)
                if (F(i,j) >= V.rows())
                    fprintf (stderr, "readOFF(): warning vertex: %d"
                             " in face: %d has value: %d, greater than #v: %d\n",
                             j,i,F(i,j),(int)V.rows());
        fclose (fp);
				r=r; // remove warnings
    }

	
    // write mesh to an ascii off file
    void writeOFF (char *fname)
    {
        FILE *fp = fopen (fname, "w");

        if (!fp) fprintf (stderr, "writeOFF(): could not open file %s", fname);
        fprintf (fp, "OFF\n%d %d 0\n",  (int) V.rows(), (int) F.rows());
        for (int i = 0; i < V.rows(); i++)
            fprintf (fp, "%f %f %f\n", V(i,0), V(i,1), V(i,2));
        for (int i = 0; i < F.rows(); i++)
            fprintf (fp, "3 %d %d %d\n", F(i,0), F(i,1), F(i,2));
        fclose (fp);
    }
    
	
//////////////// Topology ////////////////////////////	
    
	
    // check if the mesh is edge-manifold
    bool isManifold()
    {
        vector<vector<int> > TTT;
        for(int f=0;f<F.rows();++f)
            for (int i=0;i<3;++i)
            {
                // v1 v2 f ei 
                int v1 = F(f,i);
                int v2 = F(f,(i+1)%3);
                if (v1 > v2) std::swap(v1,v2);
                vector<int> r(4);
                r[0] = v1; r[1] = v2;
                r[2] = f;  r[3] = i;
                TTT.push_back(r);
            }
        std::sort(TTT.begin(),TTT.end());
        TT = MatrixXi::Constant((int)(F.rows()),3,-1);
        
        for(unsigned int i=2;i<TTT.size();++i)
        {
            vector<int>& r1 = TTT[i-2];
            vector<int>& r2 = TTT[i-1];
            vector<int>& r3 = TTT[i];
            if ( (r1[0] == r2[0] && r2[0] == r3[0]) 
                 && 
                 (r1[1] == r2[1] && r2[1] == r3[1]) )
            {
                return false;
            }
        }
        return true;
    }
    
	
    // Initialize TT-VT*
    void initTT()
    {
        assert(isManifold());
        VT.resize(V.rows());
        vector<vector<int> > TTT;
        for(int f=0;f<F.rows();++f)
            for (int i=0;i<3;++i)
            {
                // VT*
                VT(F(f,i))=f;
                // v1 v2 f ei 
                int v1 = F(f,i);
                int v2 = F(f,(i+1)%3);
                if (v1 > v2) std::swap(v1,v2);
                vector<int> r(4);
                r[0] = v1; r[1] = v2;
                r[2] = f;  r[3] = i;
                TTT.push_back(r);
            }
        std::sort(TTT.begin(),TTT.end());
        TT = MatrixXi::Constant((int)(F.rows()),3,-1);
        
        for(unsigned int i=1;i<TTT.size();++i)
        {
            vector<int>& r1 = TTT[i-1];
            vector<int>& r2 = TTT[i];
            if ((r1[0] == r2[0]) && (r1[1] == r2[1]))
            {
                TT(r1[2],r1[3]) = r2[2];
                TT(r2[2],r2[3]) = r1[2];
            }
        }
        TTVTflag = true;
    }
    
        
    inline Vector3i getTT(int i) // returns TT relation of the i-th face
    { 
        if (!TTVTflag) initTT();
        return TT.row(i);
    }
    
    VectorXi getVT(int i) // returns VT relation of the i-th vertex
    {
        VectorXi buf(BUFFER_VT_AND_VV), buf_inv(BUFFER_VT_AND_VV);
        int curt, firstt, k=0, l=0;
        
        if (!TTVTflag) initTT(); 
        
        firstt = curt = VT(i);
        do {
            buf(k++) = curt;
            assert(k<BUFFER_VT_AND_VV);
            for (int j=0;j<3;++j)
                if (F(curt,j)==i)
                {
                    curt = TT(curt,(j+2)%3);
                    break;
                }
        } while (curt!=firstt && curt != -1);

        if(curt!=firstt){ //allora e' necessario fare il giro inverso
            curt = firstt; //questo e' il primo in entrambi i versi
            do {
                buf_inv(l++) = curt;
                assert(l<BUFFER_VT_AND_VV);
                for (int j=0;j<3;++j)
                    if (F(curt,j)==i)
                    {
                        curt = TT(curt,j);
                        break;
                    }
            } while (curt!=firstt && curt != -1);


            for(l--;l>0;l--){
                buf(k++) = buf_inv(l);
            }

        }

        buf.conservativeResize(k);

        return buf;
    }

    VectorXi getVV(int i) // returns VT relation of the i-th vertex
    {
        VectorXi buf(BUFFER_VT_AND_VV), buf_inv(BUFFER_VT_AND_VV);
        int curt, firstt, k=0, l=0;

        if (!TTVTflag) initTT(); 
        
        firstt = curt = VT(i);
        do {
            assert(k<BUFFER_VT_AND_VV);
            for (int j=0;j<3;++j)
                if (F(curt,j)==i)
                {
                    buf(k++)= F(curt,(j+2)%3);
                    curt = TT(curt,(j+2)%3);
                    break;
                }
        } while (curt!=firstt && curt != -1);

        if(curt!=firstt){ //allora e' necessario fare il giro inverso
            curt = firstt; //questo e' il primo in entrambi i versi
            do {
                assert(l<BUFFER_VT_AND_VV);
                for (int j=0;j<3;++j)
                    if (F(curt,j)==i)
                    {
                        buf_inv(l++) = F(curt,(j+1)%3);
                        curt = TT(curt,j);
                        break;
                    }
            } while (curt!=firstt && curt != -1);


            for(l--;l>=0;l--){
                buf(k++) = buf_inv(l);
            }

        }
        
        buf.conservativeResize(k);
        return buf;
    }
    
    
    
    
//////////////////// OpenGL /////////////////////////////////////
	
// #define VF_DRAW_MODE_NUM 5
//     typedef enum {SMOOTH, POINTS, WIRE, HIDDEN, FLAT} draw_mode_t;
#define VF_DRAW_MODE_NUM 4
    typedef enum {SMOOTH, FLAT, POINTS, WIRE} draw_mode_t;

	
    void draw (draw_mode_t mode)
    {
		       
		glColor3f (0.0,0.0,0.0);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef (-center[0], -center[1], -center[2]);
		glPointSize(4.0);
		Vector3f color (0.5,0.5,1.0);		// generic mesh color
        
	if (mode == SMOOTH || mode == FLAT)
	{
	    glEnable (GL_DEPTH_TEST);
	    glEnable(GL_LIGHTING);
	    // if (mode == SMOOTH)
	    // 	glShadeModel(GL_SMOOTH);
	    if (mode == FLAT)
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

    void bb ()
    {
        // FE = MatrixXi::Constant((int)(F.rows()),3,-1);
	VectorXd min, max;
	VectorXd vec;

	min = VectorXd::Constant(V.cols(),0.0);
	max = VectorXd::Constant(V.cols(),0.0);
	//center = VectorXd::Constant(V.cols(),0.0);
	center[0] = center[1] = center[2] = 0.0;

	min = V.colwise().minCoeff();
	max = V.colwise().maxCoeff();

	vec = ((max - min) / 2.0);
	vec = (V.colwise().sum()) / (float)(V.rows());
	// for (unsigned i = 0; i < 3; i++)
	//     center(i) = vec(i);
	center[0] = vec(0);
	center[1] = vec(1);
	center[2] = vec(2);
	// center[0] = center[1] = center[2] = 0.0;
	
	diagonal = (max - min).norm();
    }


/////////////////////////////////////// CLASS FIELDS ///////////////////////
	
    double center[3];
    double diagonal;
    
		
    // #Vx3: Stores the vertex coordinates, one vertex per row
    MatrixXd V;
    
    // #Fx3[4]: in the ith row, stores the indices of the vertices of the ith face
    MatrixXi F;
    
    // #Fx3: Stores the Triangle-Triangle relation
    MatrixXi TT;
    
    // #V: Stores partial Vertex-Triangle relation
    VectorXi VT;

	// #V: index attribute for vertices
	VectorXi IV;
	
private:
    
    bool TTVTflag;  // true iff TV* and TT are up-to-date
    
};

#endif
