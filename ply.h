//----------------------------------------------------------------------------
// Code to load PLY files
//  Based on code from openglsamples.sourceforge.net
//
// Hjálmtýr Hafsteinsson, feb. 2013
//----------------------------------------------------------------------------
#ifndef __PLY_H__
#define __PLY_H__

#include <stdio.h>
#include <string.h>
#include "Angel.h"


class Model_PLY {
public:
    Model_PLY()
    { NumberOfVertices = 0;  NumberOfFaces = 0; };
    int Load(char *filename);

    // Holds all the vertices of the mesh
    vec4* MeshVertices;

    // Holds all the normals of the mesh
    vec3* MeshNormals;

    // Holds all the triangles of the mesh (size is 3*number of triangles)
    vec4* TriangleVertices;

    // Holds all the triangles of the mesh (size is 3*number of triangles)
    vec3* TriangleNormals;

    int NumberOfVertices;
    int NumberOfFaces;
};

#endif // __PLY_H__

