#include "Angel.h"
#include <assert.h>
#include "ply.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

GLfloat curr_x = 0.0;       // N˙verandi x-hnit
GLfloat curr_z = -10.0;

static int spinx = 0;		// Sn˙ningur Ì x-hniti
static int spiny = 0;		// Sn˙ningur Ì y-hniti
int origx, origy;			// Upphafleg staa m˙sar

float speed = -150.0; //hradi bilaradar
float speed2 = -150.0;
float speed3 = 150.0;

float speedP = -5.0; //upphafstada planka, annars hradi
float speedP2 = 3.0;
float speedP3 = -5.0;

float speedT = 0.0; //hradi frosks a planka
float speedT2 = 0.0;
float speedT3 = 0.0;

float speedTAll = 0.0;

bool plank1 = false;
bool plank2 = false;
bool plank3 = false;

int z_counter = 0;
int lives = 3;
int score = 0;
int countModels = 0;

bool leftEdge = false; //er froskurinn i snertingu vid brunir leiksins?
bool rightEdge = false;

bool dead = false;

bool draw[4] = {false, false, false, false};
bool gameOver = false;

float flekahradi = 0.0;
float xHnitFrosks;

int groundcounter = 0;

point4 points[60];
color4 colors[63000];

GLint modelview;            // Stasetning lÌkanafylkis Ì hn˙talitara
GLint projection;           // Stasetning ofanvarpfylkis Ì hn˙talitara
int count = 0;

GLint SizeOfVertexVectorCar, SizeOfVertexVectorPlayer;
GLint NumberOfFacesCar, NumberOfFacesPlayer;

Model_PLY PLYfile;

void drawText(mat4 mv)
{
    mv *= RotateX(90);
    glUniformMatrix4fv( modelview, 1, GL_TRUE, mv );
    char lives_[] = "Lives = ";
    char score_[] = " Score = ";
    int i, len;
    
    /* Draw text */
    sprintf(&lives_[9]," %d", lives);
    sprintf(&score_[10]," %d", score);
    
    glRasterPos2i(19+xHnitFrosks, 410+(curr_z-z_counter*10));
    len = strlen(lives_);
    for (i=0; i<len; i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, lives_[i]);
    
    
    glRasterPos2i(40+xHnitFrosks, 480+(curr_z-z_counter*10));
    len = strlen(score_);
    for (i=0; i<len; i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, score_[i]);
    
    if (lives == 0)
    {
        char game_over[] = "\tGAME OVER! \n Press N to Start a New Game";
        glRasterPos2i(19+xHnitFrosks, 150+(curr_z-z_counter*10));
        len = strlen(game_over);
        for (i=0; i<len; i++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, game_over[i]);
    }
    if (countModels == 4)
    {
        char game_won[] = "\tYou won!!!\n Press N to Start a New Game";
        glRasterPos2i(19+xHnitFrosks, 150+(curr_z-z_counter*10));
        len = strlen(game_won);
        for (i=0; i<len; i++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, game_won[i]);
    }
}

//----------------------------------------------------------------------------
// OpenGL initialization

//golfid. er ekki ennta buinn ad gera neitt med tad
point4 ground[24] = {
    //gangstett
    point4( -150.0, -2.0,  -5.0, 1.0 ),
    point4( -150.0, -2.0,  -15.0, 1.0 ),
    point4( 150.0, -2.0,  -15.0, 1.0 ),
    point4( 150.0, -2.0,  -5.0, 1.0 ),
    //gata
    point4( -150.0, -2.0,  -5.0, 1.0 ),
    point4( -150.0, -2.0,  25.0, 1.0 ),
    point4( 150.0, -2.0,  25.0, 1.0 ),
    point4( 150.0, -2.0,  -5.0, 1.0 ),
    //gangstett 2
    point4( -150.0, -2.0,  45.0, 1.0 ),
    point4( -150.0, -2.0,  25.0, 1.0 ),
    point4( 150.0, -2.0,  25.0, 1.0 ),
    point4( 150.0, -2.0,  45.0, 1.0 ),
    //vatn
    point4( -150.0, -6.0,  45.0, 1.0 ),
    point4( -150.0, -6.0,  80.0, 1.0 ),
    point4( 150.0, -6.0,  80.0, 1.0 ),
    point4( 150.0, -6.0,  45.0, 1.0 ),
    //lokastada
    point4( -10.0, -6.0,  88.0, 1.0 ),
    point4( -10.0, -6.0,  80.0, 1.0 ),
    point4( 0.0, -6.0,  80.0, 1.0 ),
    point4( 0.0, -6.0,  88.0, 1.0 ),
    //girding
    point4( 82.0, -5.0,  45.0, 1.0 ),
    point4( 82.0, -5.0,  79.0, 1.0 ),
    point4( 80.0, -5.0,  79.0, 1.0 ),
    point4( 80.0, -5.0,  45.0, 1.0 )
};

//hornpunktar kubbsins sem eg mun nota sem spytu ofan a vatninu
point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};

// litir sem eg get notad fyrir hvad sem er
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black     0
    color4( 1.0, 0.0, 0.0, 1.0 ),  // red       1
    color4( 1.0, 1.0, 0.0, 1.0 ),  //yellow     2
    color4( 0.82, 0.71, 0.55, 1.0 ),  // brown  3
    color4( 0.0, 0.0, 1.0, 1.0 ),  // blue      4
    color4( 0.64, 0.61, 0.61, 1.0 ), // grey    5
    color4( 0.01, 0.40, 0.06, 1.0), //green     6
    color4( 0.27, 0.27, 0.27, 1.0) //steingrar 7
};

//----------------------------------------------------------------------------
// herna vel eg lit fyrir hvern hornpunkt
int Index = 0;
void
quad( int a, int b, int c, int d )
{
    colors[Index] = vertex_colors[3]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[3]; points[Index] = vertices[b]; Index++;
    colors[Index] = vertex_colors[3]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[3]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[3]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[3]; points[Index] = vertices[d]; Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
cube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

void litari(int litur)
{
    for (int i = 0; i < 4; i++)
    {
        points[Index] = ground[i+groundcounter];
        colors[Index] = vertex_colors[litur];
        Index++;
    }
    groundcounter += 4;
}

void init()
{
    //hledur inn hornpunktunum og ollum upplysingum um venjulegan kassa i listann 'points'. liturinn er einnig settur inn herna
    //og shaderinn skilur tetta tannig ad t.d. points[x] er med color[x], tannig er haegt ad hafa tetta marglita en eg valdi einn, brunan.
    cube();
    
    //yfirbordin eru geymd i points og vidhengid taknar lit i vertex_colors
    //gangstett 1 er teiknud fyrst
    litari(5);
    //sidan kemur gatan
    litari(0);
    //gangstett 2
    litari(5);
    //vatn
    litari(4);
    //lokastada
    litari(7);
    //girding
    litari(3);
    
    PLYfile.Load("Frosch.ply");
	
	SizeOfVertexVectorPlayer = 3*sizeof(vec4)*PLYfile.NumberOfFaces;
    NumberOfFacesPlayer = PLYfile.NumberOfFaces;
    
    PLYfile.Load("porsche.ply");
    
    SizeOfVertexVectorCar = 3*sizeof(vec4)*PLYfile.NumberOfFaces;
    NumberOfFacesCar = PLYfile.NumberOfFaces;
    
    //litum bilana
    for (int i = 0; i < NumberOfFacesCar*3; i++)
    {
        colors[Index] = vertex_colors[2];
        Index++;
    }
    //litum skjaldbokuna
    for (int i = 0; i < NumberOfFacesPlayer*3; i++)
    {
        colors[Index] = vertex_colors[6];
        Index++;
    }
    
    // Create a vertex array object
    GLuint vao;
    glGenVertexArraysAPPLE( 1, &vao );
    glBindVertexArrayAPPLE( vao );
	
    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    
    //herna loada eg hlutunum inna bufferinn. BufferData tekur fra plass svo staerdin er nog, subData tekur fra serstakt plass a buffer
    //tar sem annad argumentid er stadsetning a naesta lausa og tridja staerdin sem eg vil taka fra
    glBufferData( GL_ARRAY_BUFFER, sizeof(points)+sizeof(colors)+SizeOfVertexVectorCar+SizeOfVertexVectorPlayer, NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), SizeOfVertexVectorCar, PLYfile.TriangleVertices );
    PLYfile.Load("Frosch.ply");
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points)+SizeOfVertexVectorCar, SizeOfVertexVectorPlayer, PLYfile.TriangleVertices );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points)+SizeOfVertexVectorCar+SizeOfVertexVectorPlayer, sizeof(colors), colors );
    
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshaderver.glsl", "fshaderver.glsl" );
	//    GLuint program = InitShader( "vshaderfra.glsl", "fshaderctm.glsl" );
    glUseProgram( program );
	
    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	
    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    //buffer offset er stadsetningin a litabufferinum
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)+SizeOfVertexVectorCar+SizeOfVertexVectorPlayer) );
    
   	modelview = glGetUniformLocation( program, "ModelView" );
	projection = glGetUniformLocation( program, "Projection" );
	
    glEnable( GL_DEPTH_TEST );
	//	glEnable( GL_CULL_FACE );
	
	//    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); 
}
//----------------------------------------------------------------------------

void die(int location)
{
    //ef ekki 2 ta do froskurinn, annars endadi hann a palli
    if (location != 2)
    {
        dead = true;
        score -= 5;
        if (lives > 0)
            lives--;
    }
       
    curr_z = -10;
    score += 10;
    plank1 = false;
    plank2 = false;
    plank3 = false;
    
    speedT = 0.0;
    speedT2 = 0.0;
    speedT3 = 0.0;
    
    glutPostRedisplay();
}

//froskurinn hreyfist i 'grid' og tekur z = 10 stor hopp, tannig er audvelt ad skilgreina akgreinar. speed er hreyfing fyrir hvern bil
void CollisionDetection()
{
    if (curr_z == 0.0) // Akgrein 1
    {
        //13.5 er lengd a bil, 30 er vegalengd a milli bila og 5 (dregid fra badum megin svo 10) er breidd skjaldbokunar
        for (int i = 0; i < 4; i++)
        {
            if (curr_x > speed+30*i-5 && curr_x < speed+13.5+30*i-5)
                die(0);
        }
    }
    
    if (curr_z == 10.0) //Akgrein 2
    {
        for (int i = 0; i < 3; i++)
        {
            if (curr_x > speed2+40*i-5 && curr_x < speed2+13.5+40*i-5)
                die(1);
        }
    }
    
    if (curr_z == 20.0) //Akgrein 3
    {
        for (int i = 0; i < 3; i++)
        {
            if (curr_x < speed3-53.5*i+5 && curr_x > speed3-13.5-53.5*i+5)
                die(1);
        }
    }
}

float SplashDetection(int planks1, int planks2, int planks3)
{
    speedTAll = (speedT+speedT2+speedT3)*20;
    
    if (curr_z == 50.0) //fyrsta flekaumferd
    {
        plank1 = true;
        plank2 = false;
        //hradi er margfaldadur med 20 utaf scale sem er notad utum allt, 10.5 er hversu langt a flekann leikmadurinn ma vera og 13 lengd fleka
        for (int i = 0; i < planks1; i++)
        {
            if (curr_x+speedTAll > speedP*20-10+38*i && curr_x+speedTAll < (speedP)*20+10.5+38*i)
                return speedTAll;
        }
            plank1 = false;
            die(1);
            return speedTAll;
    }
    
    else if (curr_z == 60.0)
    {
        plank1 = false;
        plank2 = true;
        plank3 = false;
        
        for (int i = 0; i < planks2; i++)
        {
            if (curr_x+speedTAll < speedP2*20+10+46*i && curr_x+speedTAll> (speedP2)*20-10+46*i)
                return speedTAll;
        }
            plank2 = false;
            die(1);
            return speedTAll;
    }
    
    else if (curr_z == 70.0)
    {
        plank2 = false;
        plank3 = true;
        
        for (int i = 0; i < planks3; i++)
        {
            if (curr_x+speedTAll > speedP3*20-10+30*i && curr_x+speedTAll < (speedP3)*20+10.5+30*i)
                return speedTAll;
        }
        //die();
        plank3 = false;
        die(1);
        return speedTAll;
    }
    
    else {
            plank1 = false;
            plank2 = false;
            plank3 = false;
            return speedTAll;
    }
        
}
//----------------------------------------------------------------------------
void drawCarLane(mat4 mv, float velocity, float space, int rotateAngle, int distance, int numberOfCars)
{
    mv *= Translate( velocity, 0.0, distance );
    mv *= RotateY(rotateAngle);
    

    int i;
    for (i = 0; i < numberOfCars; i++)
    {
        glUniformMatrix4fv( modelview, 1, GL_TRUE, mv );
        glDrawArrays( GL_TRIANGLES, sizeof(points), 3*NumberOfFacesCar );
        //glDrawArrays( GL_TRIANGLES, sizeof(points), SizeOfVertexVectorCar );
        mv *= Translate(space,0.0,0.0);
    }
    
}

void drawPlayer(mat4 mv)
{
    mv *= Scale(0.0025,0.0025,0.00125);
    mv *= Translate((xHnitFrosks)*400,400*-3,curr_z*800);
    
    glUniformMatrix4fv( modelview, 1, GL_TRUE, mv );
    glDrawArrays( GL_TRIANGLES, 3*NumberOfFacesCar+sizeof(points), 3*NumberOfFacesPlayer );
}

void drawPlayerModel(mat4 mv, float xhnit)
{
    mv *= Scale(0.0025,0.0025,0.00125);
    mv *= Translate(xhnit*400,-6*400,81*800);
    //mv *= RotateX(-90)*RotateZ(180);;
    
    glUniformMatrix4fv( modelview, 1, GL_TRUE, mv );
    glDrawArrays( GL_TRIANGLES, 3*NumberOfFacesCar+sizeof(points), 3*NumberOfFacesPlayer );
}

void drawModels(mat4 mv)
{
    if (draw[0])
        drawPlayerModel(mv, 64.09);
    if (draw[1])
        drawPlayerModel(mv, 25.09);
    if (draw[2])
        drawPlayerModel(mv, -14.09);
    if (draw[3])
        drawPlayerModel(mv, -55.09);
}

void drawPlanks(mat4 mv, int numberOfPlanks, float adskil, float z, float hradi)
{
    mv *= Scale(20,1,5);
    mv *= Translate(hradi,-5,10+z);
    
    int i;
    for (i = 0; i < numberOfPlanks; i++)
    {
        glUniformMatrix4fv( modelview, 1, GL_TRUE, mv );
        glDrawArrays( GL_TRIANGLES, 0, 36);
        mv *= Translate(adskil,0.0,0.0);
    }
}

void drawfloor (mat4 mv)
{
    glUniformMatrix4fv( modelview, 1, GL_TRUE, mv );
    //gangstett
    glDrawArrays( GL_QUADS, 36, 4);
    //gata
    glDrawArrays( GL_QUADS, 40, 4);
    //gangstett 2
    glDrawArrays( GL_QUADS, 44, 4);
    //vatn
    glDrawArrays( GL_QUADS, 48, 4);
    //girding
    glDrawArrays( GL_QUADS, 56, 4);
    mv *= Translate(-160.0,0.0,0.0);
    glUniformMatrix4fv( modelview, 1, GL_TRUE, mv );
    glDrawArrays( GL_QUADS, 56, 4);
}
void drawLokastodur(mat4 mv)
{
    for (int i = 0; i < 4; i++)
    {
        mv *= Translate(70.0,0.0,0.0);
        glUniformMatrix4fv( modelview, 1, GL_TRUE, mv );
        glDrawArrays( GL_QUADS, 52, 4);
        mv *= Translate(-110.0,0.0,0.0);
    }
}

void lokastodur(mat4 mv)
{
    if (xHnitFrosks > 59.1 && xHnitFrosks < 69.1)
    {
        if (!draw[0])
        {
            draw[0] = true;
            countModels++;
            die(2);
        }
        else die(1);
    }
    else if (xHnitFrosks > 20.1 && xHnitFrosks < 30.1)
    {
        if (!draw[1])
        {
            draw[1] = true;
            countModels++;
            die(2);
        }
        else die(1);
    }
    else if (xHnitFrosks > -19.9 && xHnitFrosks < -7.9)
    {
        if (!draw[2])
        {
            draw[2] = true;
            countModels++;
            die(2);
        }
        else die(1);
    }
    else if (xHnitFrosks > -58.9 && xHnitFrosks < -50.9)
    {
        if (!draw[3])
        {
            draw[3] = true;
            countModels++;
            die(2);
        }
        else die(1);
    }
    else
        die(1);
    
}

void resetModels()
{
    countModels = 0;
    for (int i = 0; i < 4; i++)
        draw[i] = false;
}

//----------------------------------------------------------------------------
void
display( void )
{
	vec4 acolor;
    
    mat4 mv,mv2;
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    if (curr_z >= 40)
        flekahradi = SplashDetection(3,3,3); //passa ad talan se su sama og i drawPlanks


    if (curr_z >=50 && ((leftEdge && (plank1 || plank3)) || (rightEdge && plank2)))
        //die(1);
        xHnitFrosks += 0;
    else
        {
            if (dead == false && countModels != 4 && lives > 0)
                xHnitFrosks = flekahradi + curr_x;
        }
    
    if (dead)
    {
        xHnitFrosks = 0;
        flekahradi = 0;
        curr_x = 0;
    }
    //printf("%f \n", xHnitFrosks);
    
    mv  *= LookAt( vec4(xHnitFrosks*0.5, 70.0, -30.0+curr_z*0.5, 1.0 ),
                  vec4(xHnitFrosks*0.5, 0.0, 40.0+curr_z*0.5, 1.0 ),
                  vec4( 0.0, 1.0, 1.0, 0.0 ) );
    
    //drawText(mv);
    
    //gefur hlidarsyn a leikinn, kemur ser vel til ad stadsetja hluti
   /* mv  *= LookAt( vec4( -80.0, 6.0, 50.0, 1.0 ),
                  vec4( 40.0, 6.0, 50.0, 1.0 ),
                  vec4( 0.0, 1.0, 0.0, 0.0 ) );*/
    
    drawModels(mv);
    
    drawfloor(mv);
    drawLokastodur(mv);
    //tekur inn model view, fjolda fleka, fjarlaegd a milli fleka, og fjarlaegd flekaradar fra stodludum nullpunkti    
    drawPlanks(mv, 3, 1.9, 0.3, speedP);
    drawPlanks(mv, 3, 2.3, 2.4, speedP2);
    drawPlanks(mv, 3, 1.5, 4.4, speedP3);
    
    //teiknar frosk
    if (countModels != 4 && lives > 0)
        drawPlayer(mv);
    
    //fallid tekur inn model view, speed, bil a milli bila, snuningshorn (tegar trafik fer i hina attina), fjarlaegd fra nullpunkti i z-as, og fjoldi bila
    drawCarLane(mv, speed, 30, 0, 0, 4);
    drawCarLane(mv, speed2, 40, 0, 10, 3);
    drawCarLane(mv, speed3, 53.5, 180, 20, 3);
    
    if (curr_z <= 20.0)
        CollisionDetection();
  
    if (curr_z == 80)
        lokastodur(mv);
        
    if (dead)
    {
        dead = false;
        flekahradi = 0;
    }

    glFlush();
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void myspecialkey ( int key, int x, int y )
{
    switch(key) {
        case GLUT_KEY_LEFT:
            if (!leftEdge && xHnitFrosks+10 <= 76)
                curr_x += 10.0;
            //printf("x = %f \n", xHnitFrosks);
			break;
        case GLUT_KEY_RIGHT:
            if (!rightEdge && xHnitFrosks-10 >= -76)
                curr_x -= 10.0;
            //printf("x = %f \n", xHnitFrosks);
            break;
        case GLUT_KEY_UP:
            if (lives > 0 && countModels != 4)
                curr_z += 10.0;
			break;
        case GLUT_KEY_DOWN:
            if (curr_z > -10)
                curr_z -= 10.0;
            break;
    }
    glutPostRedisplay();
}

//----------------------------------------------------------------------------
// Setja ofanvarpsfylki mia vi hlutfˆll glugga
void myreshape( int width, int height )
{
    glViewport( 0, 0, width, height );
	
    GLfloat zNear = 0.2, zFar = 100.0;
	GLfloat fovy = 65.0;
    GLfloat aspect = GLfloat(width)/height;
	
    mat4  p = Perspective( fovy, aspect, zNear, zFar );
    glUniformMatrix4fv( projection, 1, GL_TRUE, p );
}

void idle()
{
    speed += 0.04;
    if (speed >= 50) speed = -150;
    
    speed2 += 0.06;
    if (speed2 >= 100) speed2 = -150;
    
    speed3 -= 0.032;
    if (speed3 <= -100) speed3 = 150;
    
    speedP += 0.0004;
    if (speedP >= 4.7) speedP = -9.2;
    
    speedP2 -= 0.0008;
    if (speedP2 <= -9.2) speedP2 = 4.7;
    
    speedP3 += 0.001;
    if (speedP3 >= 4.7) speedP3 = -9.2;
    
    if (plank1 == true && !leftEdge) speedT += 0.0004;
    else speedT += 0;
    
    if (plank2 == true && !rightEdge) speedT2 -= 0.0008;
    else speedT2 += 0;
    
    if (plank3 == true && !leftEdge) speedT3 += 0.001;
    else speedT3 += 0;
    
    if (xHnitFrosks >= 76) leftEdge = true;
    else leftEdge = false;
    
    if (xHnitFrosks <= -74) rightEdge = true;
    else rightEdge = false;
    
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key) {
        case 'n': case 'N':
            if (countModels == 4 || lives == 0)
            {
                resetModels();
                score = 0;
                lives = 3;
            }
                break;
        case 'q': case 'Q':
            exit(0);
            break;
    }
}

//----------------------------------------------------------------------------
int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA );
    glutInitWindowSize( 700, 700 );
    glutCreateWindow( "Frogger 3D" );
	
    init();
    
	glutIdleFunc(idle);
    glutDisplayFunc( display );
    glutSpecialFunc( myspecialkey );
	glutReshapeFunc( myreshape );
    glutKeyboardFunc(keyboard);
	
    glutMainLoop();
    return 0;
}
