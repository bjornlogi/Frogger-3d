//
// S�nisforrit � innlestri og birtingu PLY skr�a.  Mj�g einf�ld �tg�fa, gerir r��
//  fyrir a� grindin s� samsett �r �r�hyrningum og innihaldi �vervektora.  Litunara�fer�
//  fer eftir �v� hva�a litaraskr�r eru nota�ar (vshaderver.glsl og fshaderver.glsl
//  gefa Gouraud litur, en vshaderfra.glsl og fshaderfra.glsl gefa Phong litun)
//
//  Hj�lmt�r Hafsteinsson, Mars 2013
//

#include "Angel.h"
#include <assert.h>
#include "ply.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

GLfloat curr_z = 5.0;       // N�verandi z-hnit
static int spinx = 0;		// Sn�ningur � x-hniti
static int spiny = 0;		// Sn�ningur � y-hniti
int origx, origy;			// Upphafleg sta�a m�sar

GLint modelview;            // Sta�setning l�kanafylkis � hn�talitara
GLint projection;           // Sta�setning ofanvarpfylkis � hn�talitara

Model_PLY PLYfile;

//----------------------------------------------------------------------------
// OpenGL initialization
void init()
{
    PLYfile.Load( "teapot-n.ply" );

	GLint SizeOfVertexVector = 3*sizeof(vec4)*PLYfile.NumberOfFaces;
	GLint SizeOfNormalsVector = 3*sizeof(vec3)*PLYfile.NumberOfFaces;

    // Create a vertex array object
    GLuint vao;
    glGenVertexArraysAPPLE( 1, &vao );
    glBindVertexArrayAPPLE( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, SizeOfVertexVector+SizeOfNormalsVector, NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, SizeOfVertexVector, PLYfile.TriangleVertices );
    glBufferSubData( GL_ARRAY_BUFFER, SizeOfVertexVector, SizeOfNormalsVector, PLYfile.TriangleNormals );

    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshaderver.glsl", "fshaderver.glsl" );
//    GLuint program = InitShader( "vshaderfra.glsl", "fshaderfra.glsl" );
    glUseProgram( program );

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SizeOfVertexVector) );

    // Initialize shader lighting parameters
    point4 light_position( 1.0, 1.0, 2.0, 0.0 );
    color4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
    color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
    color4 light_specular( 1.0, 1.0, 1.0, 1.0 );

    color4 material_ambient( 1.0, 0.0, 1.0, 1.0 );
    color4 material_diffuse( 1.0, 0.0, 1.0, 1.0 );
    color4 material_specular( 1.0, 1.0, 1.0, 1.0 );
    float  material_shininess = 10.0;

    color4 ambient_product = light_ambient * material_ambient;
    color4 diffuse_product = light_diffuse * material_diffuse;
    color4 specular_product = light_specular * material_specular;

    glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),  1, ambient_product );
    glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),  1, diffuse_product );
    glUniform4fv( glGetUniformLocation(program, "SpecularProduct"), 1, specular_product );
	
    glUniform4fv( glGetUniformLocation(program, "LightPosition"), 1, light_position );

    glUniform1f( glGetUniformLocation(program, "Shininess"), material_shininess );
		 
	modelview = glGetUniformLocation( program, "ModelView" );
	projection = glGetUniformLocation( program, "Projection" );

    glEnable( GL_DEPTH_TEST );
//	glEnable( GL_CULL_FACE );

//    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glClearColor( 1.0, 1.0, 1.0, 1.0 ); 
}

//----------------------------------------------------------------------------

void
display( void )
{
	vec4 acolor;

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	mat4 mv;
	mv *= Translate( 0.0, -1.0, -curr_z );
	mv *= RotateX( (GLfloat)spinx ) * RotateY( (GLfloat)spiny );

	mv *= RotateX( -90.0 );
    glUniformMatrix4fv( modelview, 1, GL_TRUE, mv );

    // Teikna l�kani�
    glDrawArrays( GL_TRIANGLES, 0, 3*PLYfile.NumberOfFaces );

    glFlush();
	glutSwapBuffers();
}

//----------------------------------------------------------------------------
// B�r til sn�ningshorn �t fr� hreyfingum m�sar
void mymotion( int x, int y )
{
    spiny = ( spiny + (x - origx) ) % 360 ;
    spinx = ( spinx + (y - origy) ) % 360 ;
    origx = x;
    origy = y;

    glutPostRedisplay();
}

//----------------------------------------------------------------------------
// Up-�r f�rir fj�r, en ni�ur-�r f�rir n�r
void myspecialkey ( int key, int x, int y )
{
    switch(key) {
        case GLUT_KEY_UP:
			curr_z *= 1.1;
			break;
        case GLUT_KEY_DOWN:
			curr_z *= 0.9;
            break;
    }
    glutPostRedisplay();
}

//----------------------------------------------------------------------------
// Geymir sta�setningu m�sar �egar upphaflega er smellt � m�s
void mymouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
        origx = x;
        origy = y;
    }
}

//----------------------------------------------------------------------------
// Setja ofanvarpsfylki mi�a� vi� hlutf�ll glugga
void myreshape( int width, int height )
{
    glViewport( 0, 0, width, height );

    GLfloat zNear = 0.2, zFar = 100.0;
	GLfloat fovy = 65.0;
    GLfloat aspect = GLfloat(width)/height;

    mat4  p = Perspective( fovy, aspect, zNear, zFar );
    glUniformMatrix4fv( projection, 1, GL_TRUE, p );
}

//----------------------------------------------------------------------------
int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA );
    glutInitWindowSize( 700, 700 );
    glutCreateWindow( "Loading and displaying PLY files" );

    //glewInit();

    init();

    glutDisplayFunc( display );
    glutMouseFunc( mymouse );
    glutSpecialFunc( myspecialkey );
    glutMotionFunc( mymotion );
	glutReshapeFunc( myreshape );

    glutMainLoop();
    return 0;
}
