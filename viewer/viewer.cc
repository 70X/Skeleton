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
    if (drawing.showDebug)
        drawing.drawDebug(cage_draw_mode);

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


void TW_CALL setRaffinementTimes (void *value)
{
    int times = 1;
    if (p.numberOfRaff != -1)
        times = p.numberOfRaff;
    p.raffinementQuadLayout(times);
    char str[100];
    sprintf(str, "Camera_Rendering/raffinement label='step %d raffinement'",  (int) p.raffinementTimes);
    TwDefine(str);

    sprintf(str, "Camera_Rendering/Info1.1 label='1) Num Quad: %d'",  (int) p.C.Q.rows() - 1);
    TwDefine(str);

    sprintf(str, "Camera_Rendering/Info1.2 label='2) Last Error: %f'", p.LastError );
    TwDefine(str);

    #ifdef __MODE_DEBUG
    sprintf(str, "Camera_Rendering/IDQuad max=%d ",  (int)(p.C.Q.rows() - 1));
    TwDefine(str);
    
    sprintf(str, "Camera_Rendering/IDPolychord max=%d ",  (int)(p.P.getSize() - 1));
    TwDefine(str);
    #endif 

    glutPostRedisplay();
}

void TW_CALL setNumMax (const void *value, void *)
{
    p.numberOfRaff = *(const double *) value;
    //glutPostRedisplay();
}

void TW_CALL getNumMax (void *value, void *)
{
    *(double *) value = p.numberOfRaff;
}

void TW_CALL setQuadMax (const void *value, void *)
{
    p.QuadMax = *(const double *) value;
    //glutPostRedisplay();
}

void TW_CALL getQuadMax (void *value, void *)
{
    *(double *) value = p.QuadMax;
}

void TW_CALL setErrMax (const void *value, void *)
{
    p.ErrMax = *(const double *) value;
    //glutPostRedisplay();
}

void TW_CALL getErrMax (void *value, void *)
{
    *(double *) value = p.ErrMax;
}


// ---------------------------- MODE DEBUG ----------------------------------
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

void TW_CALL setIDPolychord (const void *value, void *)
{
    drawing.IDPolychord = *(const double *) value;
    glutPostRedisplay();
}

void TW_CALL getIDPolychord (void *value, void *)
{
    *(double *) value = drawing.IDPolychord;
}

int choiceIDSubCage = -1;
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


void TW_CALL getPartialTriangle (void *value, void *)
{
    *(double *) value = drawing.IDPartialTriangle;
}

int choiceIDTriangle = -1;
void TW_CALL setPartialTriangle (const void *value, void *)
{
    if (p.storeSubC.size() == 0)
        return;
    if (*(const double *) value > drawing.IDPartialTriangle)
        choiceIDTriangle++;
    else if (*(const double *) value < drawing.IDPartialTriangle)
        choiceIDTriangle--;
    else return;
    vector<int>::const_iterator it = p.storeSubC[0].TsQ.begin();
    advance(it, choiceIDTriangle);
    drawing.IDPartialTriangle = *it;
}

void TW_CALL getCageSubDomain (void *value, void *)
{
    *(double *) value = drawing.IDCageSubDomain;
}

// ---------------------------- END DEBUG ----------------------------------

void TW_CALL call_quit(void *clientData)
{ 
    quit();
}



void resetAll()
{
    drawing.IDPolychord = -1;
    drawing.IDQuad = -1;
    p.initAll(p.filename);
}
void TW_CALL resetRaffinement (void *value)
{
    resetAll();
    char str[100];
    TwDefine("Camera_Rendering/raffinement label='step 0 raffinement'");

    sprintf(str, "Camera_Rendering/Info1.1 label='1) Num Quad: %d'",  (int) p.C.Q.rows() - 1);
    TwDefine(str);

    sprintf(str, "Camera_Rendering/Info1.2 label='2) Last Error: %f'", p.LastError );
    TwDefine(str);
    #ifdef __MODE_DEBUG
    sprintf(str, "Camera_Rendering/IDQuad max=%d ",  (int)(p.C.Q.rows() - 1));
    TwDefine(str);
    
    sprintf(str, "Camera_Rendering/IDPolychord max=%d ",  (int)(p.P.getSize() - 1));
    TwDefine(str);
    #endif

    glutPostRedisplay();
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

    strcpy(p.filename, argv[1]);
    p.initAll(p.filename);
    drawing.setProcess(p);
    drawing.bb(p.M.V, p.M.F);
    
    //p.error_type_choice = Process::GRID_SIMPLE;
    //p.raffinementQuadLayout(30);
    //resetAll();
    //p.error_type_choice = Process::GRID_HALFEDGE;
    //p.raffinementQuadLayout(30);
    //resetAll();
    //p.error_type_choice = Process::WITH_QUEUE;
    //p.raffinementQuadLayout(30);
    
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
    
    TwEnumVal errorTypeEnum[ERROR_TYPE_NUM] = {
        { Process::WITH_QUEUE, "With queue" }, { Process::GRID_SIMPLE, "Simple Grid" }, { Process::GRID_HALFEDGE, "Half edge Grid" } }; 
    
    TwType error_type = TwDefineEnum("errorType", errorTypeEnum, ERROR_TYPE_NUM);
    
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
    
    TwAddVarRW(cBar, "DrawMeshMode", draw_modeT, &mesh_draw_mode,
           "group = 'Mesh'" " keyIncr='<' keyDecr='>'"); 
    TwAddVarRW(cBar, "show cage", TW_TYPE_BOOLCPP, &drawing.showCage, 
        "group = 'Cage'");
    TwAddVarRW(cBar, "DrawCageMode", draw_modeC, &cage_draw_mode,
           "group = 'Cage'" " keyIncr='<' keyDecr='>'");
    
    char str[100];
    sprintf(str, "group = 'Raffinement' label='1) Num Quad: %d'",  (int) p.C.Q.rows() - 1);
    TwAddButton(cBar, "Info1.1", NULL, NULL, str);

    sprintf(str, "group = 'Raffinement' label='2) Last Error: %f'", p.LastError );
    TwAddButton(cBar, "Info1.2", NULL, NULL, str);

    TwAddButton(cBar, "Blank1", NULL, NULL, "group = 'Raffinement' label=' ' ");
    
    TwAddVarRW(cBar, "ErrorType", error_type, &p.error_type_choice,
           "group = 'Raffinement'" " keyIncr='<' keyDecr='>'"); 

    sprintf(str, "group = 'Raffinement' min=-1 step=1");
    TwAddVarCB(cBar, "RaffNumMax", TW_TYPE_DOUBLE, setNumMax, getNumMax,
           NULL, strcat(str, " label='Iterations: '"));
    
    sprintf(str, "group = 'Raffinement' min=-1 step=1");
    TwAddVarCB(cBar, "RaffQuadMax", TW_TYPE_DOUBLE, setQuadMax, getQuadMax,
           NULL, strcat(str, " label='Threshold Quad: '"));

    sprintf(str, "group = 'Raffinement' min=-1 step=0.0001");
    TwAddVarCB(cBar, "RaffErrMax", TW_TYPE_DOUBLE, setErrMax, getErrMax,
           NULL, strcat(str, "min=0.00 max=1.00 label='Threshold Error: '"));

    sprintf(str, "group = 'Raffinement' label='step %d raffinement'",  (int) p.raffinementTimes);
    TwAddButton(cBar, "raffinement", setRaffinementTimes, NULL, 
                str);

    TwAddButton(cBar, "Blank2", NULL, NULL, "group = 'Raffinement' label=' ' ");
    
     TwAddSeparator(cBar, "Sep1", "group = 'Raffinement'");

    TwAddButton(cBar, "reset", resetRaffinement, NULL, 
                  "group = 'Raffinement' label='--> Reset'");
    
    #ifdef __MODE_DEBUG
    TwAddVarRW(cBar, "show lines mapping cage", TW_TYPE_BOOLCPP, &drawing.showLines, 
        "group = 'Mesh'");

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

    /*TwAddVarRW(cBar, "with queue", TW_TYPE_BOOLCPP, &p.raffinamentQueue, 
        "group = 'Debug' label='with queue'");
    */
    #endif

    glutMainLoop();
    
    exit (-1);
    #endif
}

