// opengl
// #include <GL/glew.h>
#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif
#define __ESC_GLUT 27

#include <AntTweakBar.h>

#include "Process.hh"
#include "DrawMesh.hh"
#include "camera.hh"                // viewing controls

/////////////
// GLOBALS //
/////////////

#define W 800
#define H 600

// OpenGl related vars
Camera camera;
int width = W;
int height = H;
int times = 0;
int choiceIDSubCage = -1;

DrawMesh::draw_mode_t mesh_draw_mode = DrawMesh::FLAT;
DrawMesh::draw_mode_t cage_draw_mode = DrawMesh::WIRE;


// Mesh:
Process p;
DrawMesh drawing;
//////////
// QUIT //
//////////

void quit()
{
    // insert here eventual clean up code
    exit(0);
}


///////////////////
// GLUT CALLBACK //
///////////////////
void display()
{
    glClearColor(1.0f, 1.0f, 1.0f, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw boundary
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    // draw GUI
    camera.display_begin();
    if (drawing.showMesh)
        drawing.drawMesh(mesh_draw_mode);
    if (drawing.showCage)
        drawing.drawCage(cage_draw_mode);

    camera.display_end();
    // draw GUI
    TwDraw();

    glFlush();
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    camera.reshape(w,h);
    width = w;
    height = h;
//    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    TwWindowSize(w, h);
    glutPostRedisplay();
}


void mouse_click(int button, int state, int x, int y)
{
    if (TwEventMouseButtonGLUT(button, state, x, y))
    {
        glutPostRedisplay();
        return;
    }

	// insert here management of selection and editing
	
    //cout << x <<","<<y<<endl;
	// camera motion with left button
	camera.mouse(button, state, x, y);
    glutPostRedisplay();
}


void mouse_move(int x, int y)
{
    if (TwEventMouseMotionGLUT(x, y))
    {
        glutPostRedisplay();
        return;
    }
	
	// insert here management of selection and editing

    else // view motion
        camera.mouse_move(x,y);
    glutPostRedisplay();
}


void keyboard(unsigned char k, int x, int y)
{
    if (k == __ESC_GLUT) quit();

    TwEventKeyboardGLUT(k, x, y);
	
    // insert here management of other keybord keys 

    //camera.key (k, x, y);
    glutPostRedisplay();
}

void special(int k, int x, int y)
{
    TwEventSpecialGLUT(k, x, y);

    //camera.special (k, x, y);
    glutPostRedisplay();
}


///////////////////////////
// ANTTWEAKBAR CALLBACKS //
///////////////////////////

void TW_CALL setAperture (const void *value, void *)
{
    camera.gCamera.aperture = *(const double *) value;
    glutPostRedisplay();
}

void TW_CALL getAperture (void *value, void *)
{
    *(double *) value = camera.gCamera.aperture;
}

void TW_CALL setFocalLength (const void *value, void *)
{
    camera.gCamera.focalLength = *(const double *) value;
    glutPostRedisplay();
}

void TW_CALL getFocalLength (void *value, void *)
{
    *(double *) value = camera.gCamera.focalLength;
}

void TW_CALL setIDPolychord (const void *value, void *)
{
    drawing.IDPolychord = *(const double *) value;
    glutPostRedisplay();
}

void TW_CALL getIDPolychord (void *value, void *)
{
    *(double *) value = drawing.IDPolychord;
}


void TW_CALL setRaffinementTimes (void *value)
{
    times++;
    p.raffinementQuadLayout(1);
    char str[100];
    sprintf(str, "Camera_Rendering/raffinement label='step %d raffinement'",  (int) times);
    TwDefine(str);

    sprintf(str, "Camera_Rendering/IDQuad max=%d ",  (int)(p.C.Q.rows() - 1));
    TwDefine(str);
    
    sprintf(str, "Camera_Rendering/IDPolychord max=%d ",  (int)(p.P.getSize() - 1));
    TwDefine(str);
    glutPostRedisplay();
}



void TW_CALL setEnableCageFace (const void *value, void *)
{
    drawing.IDQuad = *(const double *) value;
    glutPostRedisplay();
}

void TW_CALL getEnableCageFace (void *value, void *)
{
    *(double *) value = drawing.IDQuad;
}

void TW_CALL setTriangleView (const void *value, void *)
{
    drawing.IDTriangle = *(const double *) value;
    //glutPostRedisplay();
}

void TW_CALL getTriangleView (void *value, void *)
{
    *(double *) value = drawing.IDTriangle;
}


void TW_CALL setCageSubDomain (const void *value, void *)
{
    choiceIDSubCage = *(const double *) value;
    drawing.IDCageSubDomain = -1;
    int i = choiceIDSubCage;
    if (p.storeSubC.find(choiceIDSubCage) != p.storeSubC.end())
        drawing.IDCageSubDomain = choiceIDSubCage;
    else
        for(map<int, CageSubDomain>::const_iterator it = p.storeSubC.begin(); it != p.storeSubC.end(); ++it, --i)
        {

            if(i==0)
                 drawing.IDCageSubDomain = it->first;
        }
}

/*
void TW_CALL setCageSubDomain (const void *value, void *)
{
    int choiceIDSubCage = *(const double *) value;
    /*int i = choiceIDSubCage;
    for(map<int, CageSubDomain>::const_iterator it = p.storeSubC.begin(); it != p.storeSubC.end(); ++it, --i)
    {
        if(i==0)
             drawing.IDCageSubDomain = it->first;
    }
    char str[100];
    sprintf(str, "Camera_Rendering/LabelIDCageSubDomain value=%d ",  (int)(drawing.IDCageSubDomain));
    TwDefine(str);*/
//}

void TW_CALL getCageSubDomain (void *value, void *)
{
    *(double *) value = drawing.IDCageSubDomain;
}



void TW_CALL call_quit(void *clientData)
{ 
    quit();
}

char filename[200];

void TW_CALL resetRaffinament (void *value)
{
    drawing.IDPolychord = -1;
    drawing.IDQuad = -1;
    p.initAll(filename);
    times = 0;
    
    char str[50];
    TwDefine("Camera_Rendering/raffinement label='step 0 raffinement'");

    sprintf(str, "Camera_Rendering/IDQuad max=%d ",  (int)(p.C.Q.rows() - 1));
    TwDefine(str);
    
    sprintf(str, "Camera_Rendering/IDPolychord max=%d ",  (int)(p.P.getSize() - 1));
    TwDefine(str);
    glutPostRedisplay();
}

void debug()
{
    Vector3d A(2,3,1);
    Vector3d B(0,-1,4);
    Vector3d C = (B-A);
    cout << (B-A)/(B-A).norm() << endl<<endl;
    cout << C/(sqrt(
        pow(C(0), 2)+pow(C(1),2) + pow(C(2),2) 
        )) << endl;
}

//////////
// MAIN //
//////////

int main (int argc, char *argv[])
{
    if (argc != 2)
    {
	fprintf (stderr, "Usage: ./viewer mesh_file\n");
	exit (-1);
    }

    strcpy(filename, argv[1]);
    p.initAll(filename);
    drawing.setProcess(p);
    drawing.bb(p.M.V, p.M.F);
    p.raffinementQuadLayout(1);
    #define __VIEWER__DEBUG
    #ifdef __VIEWER__DEBUG

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(W, H);
    glutCreateWindow("viewer");

    TwInit(TW_OPENGL, NULL);
    TwWindowSize(W, H);

    glClearColor(0, 0, 0, 0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    //glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    camera.SetLighting(4);

    camera.gCameraReset(drawing.getDiagonal(), drawing.getCenter());

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse_click);
    glutMotionFunc(mouse_move);
    glutPassiveMotionFunc(mouse_move);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    TwGLUTModifiersFunc(glutGetModifiers);

    TwBar *cBar;
    cBar = TwNewBar("Camera_Rendering");
    TwDefine("Camera_Rendering size='250 420'");
    TwDefine("Camera_Rendering valueswidth=80");
    TwDefine("Camera_Rendering color='192 255 192' text=dark ");

    TwAddButton(cBar, "Quit", call_quit, NULL, "");

    TwEnumVal draw_modeEV[MESH_DRAW_MODE_NUM] = {
        { DrawMesh::SMOOTH, "Smooth" }, { DrawMesh::FLAT, "Flat" }, { DrawMesh::POINTS, "Points" }, { DrawMesh::WIRE, "Wire" } }; 

    TwType draw_modeT = TwDefineEnum("drawModeType", draw_modeEV, MESH_DRAW_MODE_NUM);
    TwType draw_modeC = TwDefineEnum("drawCageModeType", draw_modeEV, MESH_DRAW_MODE_NUM);
    
    // info
    TwAddVarRW(cBar, "show camera help", TW_TYPE_BOOLCPP, &camera.gShowHelp, "group = 'Scene'");
    
    // help
    TwAddVarRW(cBar, "show camera info", TW_TYPE_BOOLCPP, &camera.gShowInfo, "group = 'Scene'");

    // aperture
    TwAddVarCB(cBar, "camera aperture", TW_TYPE_DOUBLE, setAperture, getAperture,
	       NULL, "group = 'Scene' min=0.00 max=100.00 step=0.1");

    // focus distance
    TwAddVarCB(cBar, "camera focus", TW_TYPE_DOUBLE, setFocalLength, getFocalLength,
	       NULL, "group = 'Scene' min=0.00 max=100.00 step=0.1");
    
    TwAddVarRW(cBar, "show mesh", TW_TYPE_BOOLCPP, &drawing.showMesh, 
        "group = 'Mesh'");
    TwAddVarRW(cBar, "show lines mapping cage", TW_TYPE_BOOLCPP, &drawing.showLines, 
        "group = 'Mesh'");
    
    TwAddVarRW(cBar, "DrawMeshMode", draw_modeT, &mesh_draw_mode,
           "group = 'Mesh'" " keyIncr='<' keyDecr='>'"); 
    TwAddVarRW(cBar, "show cage", TW_TYPE_BOOLCPP, &drawing.showCage, 
        "group = 'Cage'");
    TwAddVarRW(cBar, "DrawCageMode", draw_modeC, &cage_draw_mode,
           "group = 'Cage'" " keyIncr='<' keyDecr='>'");
    
  char str[100];
  sprintf(str, "group = 'Debug' min=-1 max=%d step=1", (int)(p.C.Q.rows() - 1));
    TwAddVarCB(cBar, "IDQuad", TW_TYPE_DOUBLE, setEnableCageFace, getEnableCageFace,
           NULL, strcat(str, " label='ID quad'") );
    TwAddVarRW(cBar, "show grid", TW_TYPE_BOOLCPP, &drawing.showGrid, 
        "group = 'Debug'");
    
    sprintf(str, "group = 'Debug' min=-1 max=%d step=1", (int)(p.P.getSize() - 1));
    TwAddVarCB(cBar, "IDPolychord", TW_TYPE_DOUBLE, setIDPolychord, getIDPolychord,
           NULL, strcat(str, " label='ID polychord'"));

    sprintf(str, "group = 'Debug' min=-1 max=%d step=1", (int)(p.M.F.rows() - 1));
    TwAddVarCB(cBar, "IDTriangle", TW_TYPE_DOUBLE, setTriangleView, getTriangleView,
           NULL, strcat(str, " label='ID triangle'"));


    sprintf(str, "group = 'Debug' min=-1 step=1");
    TwAddVarCB(cBar, "IDCageSubDomain", TW_TYPE_DOUBLE, setCageSubDomain, getCageSubDomain,
           NULL, strcat(str, " label='ID CageSubDomain'"));

    //sprintf(str, "group = 'Debug' ");
    //TwAddVarRO(cBar, "LabelIDCageSubDomain", TW_TYPE_FLOAT, &(drawing.IDCageSubDomain), strcat(str, "label='which sC: '"));
   



    TwAddButton(cBar, "raffinement", setRaffinementTimes, NULL, 
                "group = 'Debug' label='step 0 raffinement'");
    

    TwAddButton(cBar, "reset", resetRaffinament, NULL, 
                  "group = 'Debug' label='--> Reset'");
    
    /*TwAddVarCB(cBar, "threshold min", TW_TYPE_DOUBLE, setThresholdMin, getThresholdMin,
           NULL, "min=0.00 max=1.00 step=0.001");

    TwAddVarCB(cBar, "threshold max", TW_TYPE_DOUBLE, setThresholdMax, getThresholdMax,
           NULL, "min=0.00 max=1.00 step=0.001");*/
    glutMainLoop();
    
    exit (-1);
    #endif
}

