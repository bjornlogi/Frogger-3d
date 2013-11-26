//----------------------------------------------------------------------------
// Code to load PLY files
//  Based on code from openglsamples.sourceforge.net
//
// Hjalmtyr Hafsteinsson, feb. 2013
//----------------------------------------------------------------------------
#include "ply.h"


int Model_PLY::Load( char* filename )
{
    int i = 0;
    char buffer[1000];

    FILE* file = fopen(filename,"r");

    if( file ) {

        // ----------------------------------------
        // Find the number of vertices
        fgets( buffer, 500, file );
        while( strncmp( "element vertex", buffer, strlen("element vertex")) != 0  )
            fgets( buffer, 500, file );

        strcpy( buffer, buffer+strlen("element vertex") );
        sscanf( buffer, "%i", &NumberOfVertices );

        // Allocate space for vertices
        MeshVertices = (vec4*)malloc( NumberOfVertices*sizeof(vec4) );

        // ----------------------------------------
        // Check if the normals are given
        fseek( file, 0, SEEK_SET );
        fgets( buffer, 500, file );
        bool ReadNormals = false;
        while( strncmp( "end_header", buffer, strlen("end_header")) != 0  ) {
            if( strncmp( "property float nx", buffer, strlen("property float nx")) == 0 ) {
                ReadNormals = true;
                break;
            }
            fgets( buffer, 500, file );
        }

        // Allocate space for normals if needed
        if( ReadNormals )
            MeshNormals = (vec3*)malloc( NumberOfVertices*sizeof(vec3) );

        // ----------------------------------------
        // Find the number of faces
        fseek( file, 0, SEEK_SET );
        fgets( buffer, 500, file );
        while( strncmp( "element face", buffer, strlen("element face")) != 0  )
            fgets( buffer, 500, file );

        strcpy( buffer, buffer+strlen("element face") );
        sscanf( buffer, "%i" , &NumberOfFaces );

        // Allocate space for faces
        TriangleVertices = (vec4*)malloc( 3*NumberOfFaces*sizeof(vec4) );

        // Allocate space for normals if needed
        if( ReadNormals )
            TriangleNormals = (vec3*)malloc( 3*NumberOfFaces*sizeof(vec3) );

        // Go to the end of the header (end_header)
        while( strncmp( "end_header", buffer, strlen("end_header")) != 0  )
            fgets( buffer, 500, file );

        // ----------------------------------------
        // Read vertices (and normals)
        for( i = 0; i < NumberOfVertices; i++) {
            fgets( buffer, 500, file );
            if( ReadNormals )
                sscanf( buffer,"%f %f %f %f %f %f", &MeshVertices[i].x, &MeshVertices[i].y, &MeshVertices[i].z,
                                                    &MeshNormals[i].x, &MeshNormals[i].y, &MeshNormals[i].z );
            else
                sscanf( buffer,"%f %f %f", &MeshVertices[i].x, &MeshVertices[i].y, &MeshVertices[i].z );
            MeshVertices[i].w = 1.0;
        }

        // ----------------------------------------
        // Read faces
        int idx = 0;
        for( i = 0; i < NumberOfFaces; i++) {
            fgets( buffer, 500, file );

            if( buffer[0] == '3' ) {
                int v1, v2, v3;
                buffer[0] = ' ';
                sscanf( buffer, "%i %i %i", &v1, &v2, &v3 );

                TriangleVertices[3*idx] = MeshVertices[v1];
                TriangleVertices[3*idx+1] = MeshVertices[v2];
                TriangleVertices[3*idx+2] = MeshVertices[v3];
                TriangleNormals[3*idx] = MeshNormals[v1];
                TriangleNormals[3*idx+1] = MeshNormals[v2];
                TriangleNormals[3*idx+2] = MeshNormals[v3];
                idx++;

            } else if( buffer[0] == '4' ) {
                int v1, v2, v3, v4;
                buffer[0] = ' ';
                sscanf( buffer, "%i %i %i %i", &v1, &v2, &v3, &v4 );

                // Triangulate faces with 4 vertices (quads)
                TriangleVertices[3*i] = MeshVertices[v1];
                TriangleVertices[3*i+1] = MeshVertices[v2];
                TriangleVertices[3*i+2] = MeshVertices[v3];
                TriangleNormals[3*idx] = MeshNormals[v1];
                TriangleNormals[3*idx+1] = MeshNormals[v2];
                TriangleNormals[3*idx+2] = MeshNormals[v3];
                idx++;
                TriangleVertices[3*i] = MeshVertices[v1];
                TriangleVertices[3*i+1] = MeshVertices[v3];
                TriangleVertices[3*i+2] = MeshVertices[v4];
                TriangleNormals[3*idx] = MeshNormals[v1];
                TriangleNormals[3*idx+1] = MeshNormals[v3];
                TriangleNormals[3*idx+2] = MeshNormals[v4];
                idx++;

            }
        }
        // Now update the number of faces (after possible triangulation)
        NumberOfFaces = idx;
        fclose( file );
    }

    else {
        printf("File can't be opened\n");
    }

    return 0;
}
