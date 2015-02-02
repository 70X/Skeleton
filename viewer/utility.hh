
//determine whether (x,y) is inside/outside of a polyline
//1=inside, 0=outside
// from http://www.visibone.com/inpoly/
int inpoly ( const std::vector<std::vector<unsigned int > >&poly, //polygon points, [0]=x, [1]=y. Polyline need not be closed (i.e. first point != last point), the line segment between last and first selected points is constructed within this function.
            const unsigned int xt, //x (horizontal) of target point
            const unsigned int yt) //y (vertical) of target point
{
  int npoints= poly.size();
  unsigned int xnew,ynew;
  unsigned int xold,yold;
  unsigned int x1,y1;
  unsigned int x2,y2;
  int i;
  int inside=0;
  
  if (npoints < 3) {
    return(0);
  }
  xold=poly[npoints-1][0];
  yold=poly[npoints-1][1];
  for (i=0 ; i < npoints ; i++) {
    xnew=poly[i][0];
    ynew=poly[i][1];
    if (xnew > xold) {
      x1=xold;
      x2=xnew;
      y1=yold;
      y2=ynew;
    }
    else {
      x1=xnew;
      x2=xold;
      y1=ynew;
      y2=yold;
    }
    if ((xnew < xt) == (xt <= xold)          /* edge "open" at one end */
        && ((long)yt-(long)y1)*(long)(x2-x1)
        < ((long)y2-(long)y1)*(long)(xt-x1)) {
      inside=!inside;
    }
    xold=xnew;
    yold=ynew;
  }
  return(inside);
}

