#include "Utility.hh"

namespace Utility
{
  double areaTriangle(Vector2d A, Vector2d B, Vector2d C)
  {
      double a = ( 
              (B(0) - A(0) )*(C(1) - A(1) ) -
              (C(0) - A(0) )*(B(1) - A(1) )
          ) / 2;
  return (a > 0.0) ? a : -a;
  }


  double areaQuad(Vector3d A,Vector3d B,Vector3d C, Vector3d D)
  {  
      double ABC = ( (((C-A).cross(C-B))).norm() )/2.0;
      double ACD = ( (((C-A).cross(C-D))).norm() )/2.0;
      return ABC+ACD;
  }

  bool isLeft(Vector2d P0, Vector2d P1, Vector2d V)
  {
      Matrix2d m;
      m << P1(0)-P0(0), V(0)-P0(0),
           P1(1)-P0(1), V(1)-P0(1);
      return m.determinant() > 0;
  }

  bool is_inside(Vector2d A, Vector2d B, Vector2d C, Vector2d V)
  {
      return (  
                isLeft(A, B, V) &&
                isLeft(B, C, V) &&
                isLeft(C, A, V) 
              );
  }

  

  Vector3d getCoordBarycentricTriangle(Triangle2D ABCMapping, Triangle3D ABC, Vector2d V)
  {
        Vector2d _A = ABCMapping.row(0);
        Vector2d _B = ABCMapping.row(1);
        Vector2d _C = ABCMapping.row(2);
        
        double alpha = areaTriangle(V,_B,_C)/areaTriangle(_A,_B,_C);
        double beta  = areaTriangle(_A,V,_C)/areaTriangle(_A,_B,_C);
        double gamma = areaTriangle(_A,_B,V)/areaTriangle(_A,_B,_C);
        
        Vector3d VA = ABC.row(0);
        Vector3d VB = ABC.row(1);
        Vector3d VC = ABC.row(2);
        return alpha * VA + beta * VB + gamma * VC;
  }

  double normalizeDistance(double d, double min, double max)
  {
      return (d-min)/(max-min);
  }

  double computeDistance(Vector3d v, Vector3d w)
  {
      return (v-w).norm();
  }
};