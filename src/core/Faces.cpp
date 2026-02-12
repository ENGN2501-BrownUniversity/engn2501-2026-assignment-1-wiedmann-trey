//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2026-01-26 17:42:17 taubin>
//------------------------------------------------------------------------
//
// Faces.cpp
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

#include <math.h>
#include "Faces.hpp"

Faces::Faces(const int nV, const vector<int>& coordIndex) : _nV(0), _nF(0), _nC(0), _coordIndex(coordIndex) {
    _firstCornerFace.push_back(0);
    for(int i = 0; i < _coordIndex.size(); i++) {
        int idx = _coordIndex[i];
        if(idx < 0) {
            // is a -1 corner, replace with -(iF+1)
            _coordIndex[i] = -(_nF+1);

            // represents the end of a face
            _firstCornerFace.push_back(_nC+1);
            _nF++;
        } else {
            // is a vertex
            _nV++;
        }
        _nC++;
    }
}

int Faces::getNumberOfVertices() const {
    return _nV;
}

int Faces::getNumberOfFaces() const {

    return _nF;
}

int Faces::getNumberOfCorners() const {
    return _nC;
}

int Faces::getFaceSize(const int iF) const {
    if(iF >= _nF || iF < 0) return 0;

    return _firstCornerFace[iF+1] - _firstCornerFace[iF];
}

int Faces::getFaceFirstCorner(const int iF) const {
    if(iF >= _nF || iF < 0) return -1;

    return _firstCornerFace[iF];
}

int Faces::getFaceVertex(const int iF, const int j) const {
    if(iF >= _nF || iF < 0) return -1;
    if(j < 0 || j >= getFaceSize(iF) - 1) return -1;

    return _coordIndex[_firstCornerFace[iF]+j];
}

int Faces::getCornerFace(const int iC) const {
    if(iC < 0 || iC >= _nC) return -1;
    if(_coordIndex[iC] < 0) return -1;

    int i = 1;
    while(_coordIndex[iC+i] >= 0) {
        i++;
    }
    return -_coordIndex[iC+i] - 1;
}

int Faces::getNextCorner(const int iC) const {
    if(iC < 0 || iC >= _nC) return -1;
    if(_coordIndex[iC] < 0) return -1;

    if(_coordIndex[iC+1] < 0) { // need to wrap around to first corner of the face
        int f = -_coordIndex[iC+1] - 1;
        return _firstCornerFace[f];
    }
    return iC+1;
}

