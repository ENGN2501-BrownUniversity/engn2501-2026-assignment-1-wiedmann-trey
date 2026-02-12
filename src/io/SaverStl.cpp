//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2026-01-26 17:42:17 taubin>
//------------------------------------------------------------------------
//
// SaverStl.cpp
//
// Written by: <Your Name>
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2026, Gabriel Taubin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Brown University nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL GABRIEL TAUBIN BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "SaverStl.hpp"

#include "wrl/Shape.hpp"
#include "wrl/Appearance.hpp"
#include "wrl/Material.hpp"
#include "wrl/IndexedFaceSet.hpp"

#include "core/Faces.hpp"

#include <filesystem>

const char* SaverStl::_ext = "stl";

//////////////////////////////////////////////////////////////////////
bool SaverStl::save(const char* filename, SceneGraph& wrl) const {
  bool success = false;
  if(filename!=(char*)0) {

    // Check these conditions

    // 1) the SceneGraph should have a single child
    if(wrl.getChildren().size() > 1) return false;

    // 2) the child should be a Shape node
    if(!wrl.getChildren()[0]->isShape()) return false;


    Shape *shape = static_cast<Shape*>(wrl.getChildren()[0]);

    // 3) the geometry of the Shape node should be an IndexedFaceSet node
    if(!shape->hasGeometryIndexedFaceSet()) return false;

    // - construct an instance of the Faces class from the IndexedFaceSet
    IndexedFaceSet *ifs = static_cast<IndexedFaceSet*>(shape->getGeometry());

    // - remember to delete it when you are done with it (if necessary)
    //   before returning

    // 4) the IndexedFaceSet should be a triangle mesh
    if(!ifs->isTriangleMesh()) return false;

    // 5) the IndexedFaceSet should have normals per face
    if(ifs->getNormalBinding() != IndexedFaceSet::PB_PER_FACE) return false;

    Faces faces(0, ifs->getCoordIndex());

    // if (all the conditions are satisfied) {

    FILE* fp = fopen(filename,"w");
    if(	fp!=(FILE*)0) {

      // if set, use ifs->getName()
      // otherwise use filename,
      // but first remove directory and extension
      if(ifs->getName() != "") {
          filename = ifs->getName().c_str();
      } else {
          filename = std::filesystem::path(filename).stem().string().c_str();
      }

      fprintf(fp,"solid %s\n",filename);
      vector<float> normals = ifs->getNormal();
      vector<float> vertices = ifs->getCoord();

      // for each face
      for(int iF = 0; iF < faces.getNumberOfFaces(); iF++) {
          float nx = normals[iF*3], ny = normals[iF*3 + 1], nz = normals[iF*3 +2];
          fprintf(fp, "facet normal %f %f %f\n", nx, ny, nz);
          fprintf(fp, "outer loop\n");
          int faceSize = faces.getFaceSize(iF)-1;
          for(int idx = 0; idx < faceSize; idx++) {
              int iV = faces.getFaceVertex(iF, idx);
              float vx = vertices[iV*3], vy = vertices[iV*3 + 1], vz = vertices[iV*3 + 2];
              fprintf(fp, "vertex %f %f %f\n", vx, vy, vz);
          }
          fprintf(fp, "endloop\n");
          fprintf(fp, "endfacet\n");
      }

      success = true;
    }
    fprintf(fp,"endsolid %s\n",filename);
    fclose(fp);
  }
  return success;
}
