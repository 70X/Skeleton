// opengl
// #include <GL/glew.h>
#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif
#define __ESC_GLUT 27

#include <AntTweakBar.h>

#include "Mesh.hh"
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
double thresholdMin = 0;
double thresholdMax = 1;
bool showMesh = true;
bool showCage = true;

DrawMesh::draw_mode_t mesh_draw_mode = DrawMesh::POINTS;
DrawMesh::draw_mode_t cage_draw_mode = DrawMesh::FLAT;


// Mesh:
Mesh m;
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
    if (showMesh)
        drawing.drawMesh(mesh_draw_mode, m);
    if (showCage)
        drawing.drawCage(cage_draw_mode, m);

   /* if (showMesh)
       drawing.drawMeshDebug(m);
    if (showCage)
        drawing.drawCageDebug(m);*/
    //m.drawMeshPar(mesh_draw_mode);
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

void TW_CALL setEnableCageFace (const void *value, void *)
{
    drawing.onlyFace = *(const double *) value;
    glutPostRedisplay();
}

void TW_CALL getEnableCageFace (void *value, void *)
{
    *(double *) value = drawing.onlyFace;
}

void TW_CALL setThresholdMin (const void *value, void *)
{
    drawing.thresholdMin = *(const double *) value;
    //glutPostRedisplay();
}

void TW_CALL getThresholdMin (void *value, void *)
{
    *(double *) value = drawing.thresholdMin;
}
void TW_CALL setThresholdMax (const void *value, void *)
{
    drawing.thresholdMax = *(const double *) value;
    //glutPostRedisplay();
}

void TW_CALL getThresholdMax (void *value, void *)
{
    *(double *) value = drawing.thresholdMax;
}

void TW_CALL call_quit(void *clientData)
{ 
    quit();
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

    m.read(argv[1]);
    m.read(strcat(argv[1],".domain.off"));
    drawing.bb(m.getMeshV(), m.getMeshF());
    m.distanceBetweenMeshCage();
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
    TwDefine("Camera_Rendering size='250 300'");
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
    
    TwAddVarRW(cBar, "show mesh", TW_TYPE_BOOLCPP, &showMesh, 
        "group = 'Mesh'");
    TwAddVarRW(cBar, "DrawMeshMode", draw_modeT, &mesh_draw_mode,
           "group = 'Mesh'" " keyIncr='<' keyDecr='>'"); 
    TwAddVarRW(cBar, "show cage", TW_TYPE_BOOLCPP, &showCage, 
        "group = 'Cage'");
    TwAddVarRW(cBar, "DrawCageMode", draw_modeC, &cage_draw_mode,
           "group = 'Cage'" " keyIncr='<' keyDecr='>'");
    
  char str[50];
  int fnum = m.getCageF().rows();
  sprintf(str, "group = 'Debug' min=-1 max=%d step=1", fnum-1);
    TwAddVarCB(cBar, "only face", TW_TYPE_DOUBLE, setEnableCageFace, getEnableCageFace,
           NULL, str);

    TwAddVarCB(cBar, "threshold min", TW_TYPE_DOUBLE, setThresholdMin, getThresholdMin,
           NULL, "min=0.00 max=1.00 step=0.001");
    TwAddVarCB(cBar, "threshold max", TW_TYPE_DOUBLE, setThresholdMax, getThresholdMax,
           NULL, "min=0.00 max=1.00 step=0.001");
    glutMainLoop();
    
    exit (-1);
    #endif
}

