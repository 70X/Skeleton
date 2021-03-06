// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_UPSAMPLE_H
#define IGL_UPSAMPLE_H
#include "igl_inline.h"

#include <Eigen/Core>

// History:
//  changed templates from generic matrices to PlainObjectBase Alec May 7, 2011
namespace igl
{
  // Subdivide a mesh without moving vertices: loop subdivision but odd
  // vertices stay put and even vertices are just edge midpoints
  // 
  // Templates:
  //   MatV  matrix for vertex positions, e.g. MatrixXd
  //   MatF  matrix for vertex positions, e.g. MatrixXi
  // Inputs:
  //   V  #V by dim  mesh vertices
  //   F  #F by 3  mesh triangles
  // Outputs:
  //   NV new vertex positions, V is guaranteed to be at top
  //   NF new list of face indices
  //
  // NOTE: V should not be the same as NV,
  // NOTE: F should not be the same as NF, use other proto
  template <
    typename DerivedV, 
    typename DerivedF,
    typename DerivedNV,
    typename DerivedNF>
  IGL_INLINE void upsample(
    const Eigen::PlainObjectBase<DerivedV>& V,
    const Eigen::PlainObjectBase<DerivedF>& F,
    Eigen::PlainObjectBase<DerivedNV>& NV,
    Eigen::PlainObjectBase<DerivedNF>& NF);
  // Virtually in place wrapper
  template <
    typename MatV, 
    typename MatF>
  IGL_INLINE void upsample(
    MatV& V,
    MatF& F);
}

#ifndef IGL_STATIC_LIBRARY
#  include "upsample.cpp"
#endif

#endif
