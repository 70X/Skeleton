// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2014 Daniele Panozzo <daniele.panozzo@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#include "unproject_onto_mesh.h"
#include "EmbreeIntersector.h"
#include <igl/unproject.h>
#include <igl/embree/unproject_in_mesh.h>
#include <vector>

IGL_INLINE bool igl::unproject_onto_mesh(
  const Eigen::Vector2f& pos,
  const Eigen::MatrixXi& F,
  const Eigen::Matrix4f& model,
  const Eigen::Matrix4f& proj,
  const Eigen::Vector4f& viewport,
  const igl::EmbreeIntersector & ei,
  int& fid,
  int& vid)
{
  using namespace std;
  using namespace Eigen;
  MatrixXd obj;
  vector<igl::Hit> hits;

  // This is lazy, it will find more than just the first hit
  unproject_in_mesh(pos,model,proj,viewport,ei,obj,hits);

  if (hits.size()> 0)
  {
    Vector3d bc(1.0-hits[0].u-hits[0].v, hits[0].u, hits[0].v);
    int i;
    bc.maxCoeff(&i);

    fid = hits[0].id;
    vid = F(fid,i);
    return true;
  }

  return false;
}


#ifdef IGL_STATIC_LIBRARY
// Explicit template instanciation
#endif
