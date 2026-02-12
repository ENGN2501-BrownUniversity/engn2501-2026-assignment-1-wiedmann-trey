//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2026-01-26 17:42:17 taubin>
//------------------------------------------------------------------------
//
// LoaderStl.cpp
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

#include <stdio.h>
#include "TokenizerFile.hpp"
#include "LoaderStl.hpp"
#include "StrException.hpp"

#include "wrl/Shape.hpp"
#include "wrl/Appearance.hpp"
#include "wrl/Material.hpp"
#include "wrl/IndexedFaceSet.hpp"

// reference
// https://en.wikipedia.org/wiki/STL_(file_format)

const char* LoaderStl::_ext = "stl";

bool LoaderStl::parseFace(Tokenizer &tkn, vector<int> &coordIndex, vector<float> &coord, vector<float> &normal, int &nCoords) {
    if(!tkn.expecting("facet")) {
        if(tkn == "endsolid") return true;
        return false;
    }
    if(!tkn.expecting("normal")) return false;
    Vec3f vecNormal;
    if(!tkn.getVec3f(vecNormal)) return false;
    normal.push_back(vecNormal.x);
    normal.push_back(vecNormal.y);
    normal.push_back(vecNormal.z);
    if(!tkn.expecting("outer") || !tkn.expecting("loop")) return false;
    while(true) {
        if(!tkn.expecting("vertex")) break;
        Vec3f vecCoord;
        if(!tkn.getVec3f(vecCoord)) return false;
        coord.push_back(vecCoord.x);
        coord.push_back(vecCoord.y);
        coord.push_back(vecCoord.z);
        coordIndex.push_back(nCoords);
        nCoords++;
    }

    if(!(tkn == "endloop")) return false;
    if(!tkn.expecting("endfacet")) return false;
    coordIndex.push_back(-1);
    return true;
}

bool LoaderStl::load(const char* filename, SceneGraph& wrl) {
  bool success = false;

  // clear the scene graph
  wrl.clear();
  wrl.setUrl("");

  FILE* fp = (FILE*)0;
  try {

    // open the file
    if(filename==(char*)0) throw new StrException("filename==null");
    fp = fopen(filename,"r");
    if(fp==(FILE*)0) throw new StrException("fp==(FILE*)0");

    // use the io/Tokenizer class to parse the input ascii file

    TokenizerFile tkn(fp);
    // first token should be "solid"
    if(tkn.expecting("solid") && tkn.get()) {
      string stlName = tkn; // second token should be the solid name

      // create the scene graph structure :
      // 1) the SceneGraph should have a single Shape node a child
      Shape *shape = new Shape();
      wrl.addChild(shape);
      // 2) the Shape node should have an Appearance node in its appearance field
      Appearance *appearance = new Appearance();
      shape->setAppearance(appearance);
      shape->setName(stlName);
      // 3) the Appearance node should have a Material node in its material field
      Material *mat = new Material();
      appearance->setMaterial(mat);

      // 4) the Shape node should have an IndexedFaceSet node in its geometry node
      IndexedFaceSet *ifs = new IndexedFaceSet();
      shape->setGeometry(ifs);

      // from the IndexedFaceSet
      // 5) get references to the coordIndex, coord, and normal arrays
      vector<int> &coordIndex = ifs->getCoordIndex();
      vector<float> &coord = ifs->getCoord();
      vector<float> &normal = ifs->getNormal();

      // 6) set the normalPerVertex variable to false (i.e., normals per face)
      ifs->setNormalPerVertex(false);

      // the file should contain a list of triangles in the following format

      // facet normal ni nj nk
      //   outer loop
      //     vertex v1x v1y v1z
      //     vertex v2x v2y v2z
      //     vertex v3x v3y v3z
      //   endloop
      // endfacet

      int nCoords = 0;
      int faceIdx = 0;
      while(true) {
          if(!parseFace(tkn, coordIndex, coord, normal, nCoords)) {
              throw StrException("Failed to parse face index " + to_string(faceIdx));
          }
          faceIdx++;
          if(tkn == "endsolid") break;
      }

      // - run an infinite loop to parse all the faces
      // - write a private method to parse each face within the loop
      // - the method should return true if successful, and false if not
      // - if your method returns tru
      //     update the normal, coord, and coordIndex variables
      // - if your method returns false
      //     throw an StrException explaining why the method failed
    }

    // close the file (this statement may not be reached)
    fclose(fp);
    success = true;
  } catch(StrException* e) { 
    
    if(fp!=(FILE*)0) fclose(fp);
    fprintf(stderr,"ERROR | %s\n",e->what());
    delete e;
  }

  return success;
}

