// GLutils.hh
// Utilities to paint cylinders, cones, solid arrows, and a reference frame 
// Utilities to manipulate quaternions


////////////////////////////////////////////////////// CYLINDERS, CONES, ARROWS, FRAMES ////////////////

//#include <Eigen/Dense>
//
//using namespace Eigen;
#include "Eigentypes.h"   // redefined types (matrix and vector) from Eigen library

  //paints a cylinder with radius r between points a and b
  static void paintCylinder(const Vector3 &a, const Vector3 &b, const double radius)
  {
    glPushMatrix();
    
    GLUquadricObj *quadric=gluNewQuadric();          // Create A Pointer To The Quadric Object ( NEW )
    gluQuadricNormals(quadric, GLU_SMOOTH);   // Create Smooth Normals ( NEW )
    
    // This is the default direction for the cylinders to face in OpenGL
    Vector3 z = Vector3(0,0,1);        
    // Get diff between two points you want cylinder along
    Vector3 p = (a - b);                              
    // Get CROSS product (the axis of rotation)
    Vector3 t = z.cross(p); 
    
    // Get angle. LENGTH is magnitude of the vector
    double angle = 180 / M_PI * acos ((z.dot(p)) / p.norm());
    
    glTranslatef(b[0],b[1],b[2]);
    glRotated(angle,t[0],t[1],t[2]);
    
    gluQuadricOrientation(quadric,GLU_OUTSIDE);
    gluCylinder(quadric, radius, radius, p.norm(), 15, 15);
    
    gluDeleteQuadric(quadric);  
    glPopMatrix();
  }
  
  //paints a cone given its bottom and top points and its bottom radius
  static void paintCone(const Vector3 &bottom, const Vector3 &top, const double radius)
  {
    glPushMatrix();
    
    GLUquadricObj *quadric=gluNewQuadric();          // Create A Pointer To The Quadric Object ( NEW )
    gluQuadricNormals(quadric, GLU_SMOOTH);   // Create Smooth Normals ( NEW )
    
    // This is the default direction for the cylinders to face in OpenGL
    Vector3 z = Vector3(0,0,1);        
    // Get diff between two points you want cylinder along
    Vector3 p = (top - bottom);                              
    // Get CROSS product (the axis of rotation)
    Vector3 t = z.cross(p); 
    
    // Get angle. LENGTH is magnitude of the vector
    double angle = 180 / M_PI * acos ((z.dot(p)) / p.norm());
    
    glTranslatef(bottom[0],bottom[1],bottom[2]);
    glRotated(angle,t[0],t[1],t[2]);
    
    gluQuadricOrientation(quadric,GLU_OUTSIDE);
    //a cone is a cylinder with the one radius set to almost 0
    gluCylinder(quadric, radius, 1e-8, p.norm(), 15, 15);
    
    gluDeleteQuadric(quadric);  
    glPopMatrix();
  }
  
  
  //paints an arrow between from and to with a given radius
  static void paintArrow(const Vector3 &from, const Vector3 &to, double radius)
  {
    double length = (to - from).norm();
    Vector3 axis = (to-from).normalized();
    paintCylinder(from, to, radius);
    paintCone(to, to+0.1*length*axis, radius);
  }
  
  
  //paints a coordinate frame (3 axes x,y,z in the form of arrows colored red green and blue).
  //selected: a 3-element boolean array. selected[i] = true signifies that the i-th axes is selected, in which case the painted arrow will be thicker
static void paintCoordinateFrame(const Vector3 & point, const double length, const double radius, const bool *selected = 0)
  {
    Matrix33 I = Matrix33::Identity();
    for (int i =0; i <3; ++i)
    {
      double color[3];
      color[0] = 0.;
      color[1] = 0.;
      color[2] = 0.;
      color[i] = 1.;
      glColor3dv(color);
      
      if(selected && selected[i])
        paintArrow(point, point + length*I.col(i), radius*2);
      else
        paintArrow(point, point + length*I.col(i), radius);
    }
  }


/////////////////////////////////////////// QUATERNONS /////////////////////////////////////////////

// Routine to set a quaternion from a rotation axis and angle
// ( input axis = float[3] angle = float  output: quat = float[4] )
void SetQuaternionFromAxisAngle(const float *axis, float angle, float *quat)
{
    float sina2, norm;
    sina2 = (float)sin(0.5f * angle);
    norm = (float)sqrt(axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2]);
    quat[0] = sina2 * axis[0] / norm;
    quat[1] = sina2 * axis[1] / norm;
    quat[2] = sina2 * axis[2] / norm;
    quat[3] = (float)cos(0.5f * angle);
    
}


// Routine to convert a quaternion to a 4x4 matrix
// ( input: quat = float[4]  output: mat = float[4*4] )
void ConvertQuaternionToMatrix(const float *quat, float *mat)
{
    float yy2 = 2.0f * quat[1] * quat[1];
    float xy2 = 2.0f * quat[0] * quat[1];
    float xz2 = 2.0f * quat[0] * quat[2];
    float yz2 = 2.0f * quat[1] * quat[2];
    float zz2 = 2.0f * quat[2] * quat[2];
    float wz2 = 2.0f * quat[3] * quat[2];
    float wy2 = 2.0f * quat[3] * quat[1];
    float wx2 = 2.0f * quat[3] * quat[0];
    float xx2 = 2.0f * quat[0] * quat[0];
    mat[0*4+0] = - yy2 - zz2 + 1.0f;
    mat[0*4+1] = xy2 + wz2;
    mat[0*4+2] = xz2 - wy2;
    mat[0*4+3] = 0;
    mat[1*4+0] = xy2 - wz2;
    mat[1*4+1] = - xx2 - zz2 + 1.0f;
    mat[1*4+2] = yz2 + wx2;
    mat[1*4+3] = 0;
    mat[2*4+0] = xz2 + wy2;
    mat[2*4+1] = yz2 - wx2;
    mat[2*4+2] = - xx2 - yy2 + 1.0f;
    mat[2*4+3] = 0;
    mat[3*4+0] = mat[3*4+1] = mat[3*4+2] = 0;
    mat[3*4+3] = 1;
}


// Routine to multiply 2 quaternions (ie, compose rotations)
// ( input q1 = float[4] q2 = float[4]  output: qout = float[4] )
void MultiplyQuaternions(const float *q1, const float *q2, float *qout)
{
    float qr[4];
	qr[0] = q1[3]*q2[0] + q1[0]*q2[3] + q1[1]*q2[2] - q1[2]*q2[1];
	qr[1] = q1[3]*q2[1] + q1[1]*q2[3] + q1[2]*q2[0] - q1[0]*q2[2];
	qr[2] = q1[3]*q2[2] + q1[2]*q2[3] + q1[0]*q2[1] - q1[1]*q2[0];
	qr[3]  = q1[3]*q2[3] - (q1[0]*q2[0] + q1[1]*q2[1] + q1[2]*q2[2]);
    qout[0] = qr[0]; qout[1] = qr[1]; qout[2] = qr[2]; qout[3] = qr[3];
}



  
