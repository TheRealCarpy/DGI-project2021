#define WIN32_LEAN_AND_MEAN

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")
#pragma comment(linker, "/subsystem:console")

#include "windows.h"

#include <gl/gl.h>            // standard OpenGL include
#include <gl/glu.h>           // OpenGL utilties
#include <glut.h>             // OpenGL utilties

#include <mmsystem.h>

#include "myvector.h"
#include "mymatrix.h"

#include "stdlib.h"
#include "stdio.h"

#include "objloader.h"
#include "particleSystem.h"

//define the particle systems
int g_nActiveSystem = 6;
CParticleSystem *g_pParticleSystems[7];
void initParticles( void );
float  g_fElpasedTime;
double g_dCurTime;
double g_dLastTime;
int rotation_value = 0;

//prototypes for our callback functions
void draw(void);    //our drawing routine
void idle(void);    //what to do when nothing is happening
void key(unsigned char k, int x, int y);  //handle key presses
void reshape(int width, int height);      //when the window is resized
void init_drawing(void);                  //drawing intialisation

//our main routine
int main(int argc, char *argv[])
{
  //Initialise Glut and create a window
  glutInit(&argc, argv);
  //sets up our display mode
  //here we've selected an RGBA display with depth testing 
  //and double buffering enabled
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  //create a window and pass through the windows title
  glutCreateWindow("Basic Glut Particle Systems");

  //run our own drawing initialisation routine
  init_drawing();

  initParticles();

  //tell glut the names of our callback functions point to our 
  //functions that we defined at the start of this file
  glutReshapeFunc(reshape);
  glutKeyboardFunc(key);
  glutIdleFunc(idle);
  glutDisplayFunc(draw);

  //request a window size of 600 x 600
  glutInitWindowSize(600,600);
  glutReshapeWindow(600,600);

  //go into the main loop
  //this loop won't terminate until the user exits the 
  //application
  glutMainLoop();

  return 0;
}

//draw callback function - this is called by glut whenever the 
//window needs to be redrawn
void draw(void)
{
  //clear the current window
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //make changes to the modelview matrix
  glMatrixMode(GL_MODELVIEW);
  //initialise the modelview matrix to the identity matrix
  glLoadIdentity();

  glPushMatrix();
  glTranslatef(0.0,0.0,-2.0); //move the camera back to view the scene
  glRotatef(rotation_value % 360 , 0.0, 1.0, 0.0); //Rotate the camera
  //
	// Enabling GL_DEPTH_TEST and setting glDepthMask to GL_FALSE makes the 
    // Z-Buffer read-only, which helps remove graphical artifacts generated 
    // from  rendering a list of particles that haven't been sorted by 
    // distance to the eye.
	//
    // Enabling GL_BLEND and setting glBlendFunc to GL_DST_ALPHA with GL_ONE 
    // allows particles, which overlap, to alpha blend with each other 
    // correctly.
	//

	glEnable( GL_DEPTH_TEST );
    glDepthMask( GL_FALSE );

    //
	// Render particle system
	//
    
	//RENDER THE PARTICLES
	//better to try a test render first - this does not use more advanced
	//features and helps verify that the basic system is working
	//It is possible that Point Sprites may not be supported by your
	//graphics card, particularly if it is an older type
	bool doTestRender = false;

	if (doTestRender)
	{
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		g_pParticleSystems[g_nActiveSystem]->RenderSimple();
	}
	else
	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);		
		
		//specify blending function here using glBlendFunc
		glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
		
		glBindTexture( GL_TEXTURE_2D, g_pParticleSystems[g_nActiveSystem]->GetTextureID() );
		g_pParticleSystems[g_nActiveSystem]->Render();
    
		if (g_nActiveSystem == 6) {
			glDisable(GL_TEXTURE_2D);
			//g_pParticleSystems[g_nActiveSystem]->Render_planes();
		}
	}

    //
    // Reset OpenGL states...
	//
    glDepthMask( GL_TRUE );
    glDisable( GL_BLEND );

  glPopMatrix();
  //flush what we've drawn to the buffer
  glFlush();
  //swap the back buffer with the front buffer
  glutSwapBuffers();
}

//idle callback function - this is called when there is nothing 
//else to do
void idle(void)
{
  //this is a good place to do animation
  //since there are no animations in this test, we can leave 
  //idle() empty
	g_dCurTime = timeGetTime();
	g_fElpasedTime = (float)((g_dCurTime - g_dLastTime) * 0.001);
	g_dLastTime = g_dCurTime;

	g_pParticleSystems[g_nActiveSystem]->Update((float)g_fElpasedTime);

	glutPostRedisplay();
}

//key callback function - called whenever the user presses a 
//key
void key(unsigned char k, int x, int y)
{
  switch(k)
  {
	case '1':
		g_nActiveSystem = 6;
	  break;
	/*case '2':
		g_nActiveSystem = 1;
	  break;
	case '3':
		g_nActiveSystem = 2;
	  break;
	case '4':
		g_nActiveSystem = 3;
		break;
	case '5':8
		g_nActiveSystem = 4;
		break;
	case '6':
		g_nActiveSystem = 5;
		break;
	case '7':
		g_nActiveSystem = 0;
		break;*/
    case '8':
        rotation_value -= 10;
        break;
    case '9':
        rotation_value += 10;
    case 27: //27 is the ASCII code for the ESCAPE key
      exit(0);
      break;
  }
  glutPostRedisplay();
}

//reshape callback function - called when the window size changed
void reshape(int width, int height)
{
  //set the viewport to be the same width and height as the window
  glViewport(0,0,width, height);
  //make changes to the projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  //set up our projection type
  //gluPerspective(45.0, (float) width / (float) height, 1.0, 100.0);
  
  //here we use an orthogonal projection
  glOrtho(-5.0, 5.0, -5.0, 5.0, 0.1, 5.0);
  //redraw the view during resizing
  draw();
}

//set up OpenGL before we do any drawing
//this function is only called once at the start of the program
void init_drawing(void)
{
  //blend colours across the surface of the polygons
  //another mode to try is GL_FLAT which is flat shading
  glShadeModel(GL_SMOOTH);
  //turn lighting off
  glDisable(GL_LIGHTING);
  //enable OpenGL hidden surface removal
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);

  GLfloat specular[] = {0.2,0.2,0.2,1.0};
  GLfloat ambient[] = {1.0,1.0,1.0,1.0};
  GLfloat diffuse[] = {1.0,1.0,1.0,1.0};
  GLfloat position[] = {0.0,30.0,0.0,1.0};
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, position);

  GLfloat position1[] = {10.0,30.0,0.0,1.0};
  glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
  glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT1, GL_POSITION, position1);
  glEnable(GL_LIGHT1);
  //glEnable(GL_LIGHTING);

  //glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_TEXTURE_2D);
}


void initParticles( void )
{
	g_dCurTime     = timeGetTime();
	g_fElpasedTime = (float)((g_dCurTime - g_dLastTime) * 0.001);
	g_dLastTime    = g_dCurTime;


	//
    // Exploding burst
	//
    
    /*g_pParticleSystems[0] = new CParticleSystem();

    //g_pParticleSystems[0]->SetTexture( "..\\particle.bmp" );
	g_pParticleSystems[0]->SetTexture( "particle.bmp" );
    g_pParticleSystems[0]->SetMaxParticles( 100 );
    g_pParticleSystems[0]->SetNumToRelease( 100 );
    g_pParticleSystems[0]->SetReleaseInterval( 0.05f );
    g_pParticleSystems[0]->SetLifeCycle( 0.5f );
    g_pParticleSystems[0]->SetSize( 30.0f );
    g_pParticleSystems[0]->SetColor( MyVector( 1.0f, 0.0f, 0.0f ));
    g_pParticleSystems[0]->SetPosition( MyVector( 0.0f, 5.0f, 0.0f) );
    g_pParticleSystems[0]->SetVelocity( MyVector( 0.0f, 0.0f, 0.0f) );
    g_pParticleSystems[0]->SetGravity( MyVector( 0.0f, 0.0f, 0.0f) );
    g_pParticleSystems[0]->SetWind( MyVector( 0.0f, 0.0f, 0.0f) );
    g_pParticleSystems[0]->SetVelocityVar( 10.0f );

    g_pParticleSystems[0]->Init();

	//
    // Wind blown fountain
	//

	g_pParticleSystems[1] = new CParticleSystem();

//    g_pParticleSystems[1]->SetTexture( "..\\particle.bmp" );
	g_pParticleSystems[1]->SetTexture( "particle.bmp" );
    g_pParticleSystems[1]->SetMaxParticles( 500 );
    g_pParticleSystems[1]->SetNumToRelease( 5 );
    g_pParticleSystems[1]->SetReleaseInterval( 0.05f );
    g_pParticleSystems[1]->SetLifeCycle( 4.0f );
    g_pParticleSystems[1]->SetSize( 30.0f );
    g_pParticleSystems[1]->SetColor( MyVector( 1.0f, 1.0f, 1.0f ));
    g_pParticleSystems[1]->SetPosition( MyVector( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[1]->SetVelocity( MyVector( 0.0f, 5.0f, 0.0f ) );
    g_pParticleSystems[1]->SetGravity( MyVector( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[1]->SetWind( MyVector( 2.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[1]->SetVelocityVar( 1.5f );

    g_pParticleSystems[1]->Init();

	//
    // Omni-directiional emission expanding into space with no air resistence
	//

    g_pParticleSystems[2] = new CParticleSystem();

    g_pParticleSystems[2]->SetTexture( "particle.bmp" );
    g_pParticleSystems[2]->SetMaxParticles( 2048 );
    g_pParticleSystems[2]->SetNumToRelease( 10 );
    g_pParticleSystems[2]->SetReleaseInterval( 0.05f );
    g_pParticleSystems[2]->SetLifeCycle( 5.0f );
    g_pParticleSystems[2]->SetSize( 30.0f );
    g_pParticleSystems[2]->SetColor( MyVector( 1.0f, 1.0f, 1.0f ));
    g_pParticleSystems[2]->SetPosition( MyVector( 0.0f, 0.0f, 0.0f) );
    g_pParticleSystems[2]->SetVelocity( MyVector( 0.0f, 0.0f, 0.0f) );
    g_pParticleSystems[2]->SetGravity( MyVector( 0.0f, 0.0f, 0.0f) );

    g_pParticleSystems[2]->SetWind( MyVector( 0.0f, 0.0f, 0.0f) );
    g_pParticleSystems[2]->SetAirResistence( false );

    g_pParticleSystems[2]->SetVelocityVar(2.0f);

    g_pParticleSystems[2]->Init();
 
	//
    // Fountain particles behave like paint spots when striking a plane as 
	// the wind blowing them towards us
	//

    g_pParticleSystems[3] = new CParticleSystem();

    g_pParticleSystems[3]->SetTexture( "particle.bmp" );
    g_pParticleSystems[3]->SetMaxParticles( 100 );
    g_pParticleSystems[3]->SetNumToRelease( 10 );
    g_pParticleSystems[3]->SetReleaseInterval( 0.05f );
    g_pParticleSystems[3]->SetLifeCycle( 3.0f );
    g_pParticleSystems[3]->SetSize( 30.0f );
    g_pParticleSystems[3]->SetColor( MyVector( 1.0f, 1.0f, 1.0f ));
    g_pParticleSystems[3]->SetPosition( MyVector( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[3]->SetVelocity( MyVector( 0.0f, 5.0f, 0.0f ) );
    g_pParticleSystems[3]->SetGravity( MyVector( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[3]->SetWind( MyVector( 0.0f, 0.0f, -20.0f ) );
    g_pParticleSystems[3]->SetVelocityVar( 2.5f );

    g_pParticleSystems[3]->SetCollisionPlane( MyVector( 0.0f, 0.0f,1.0f ), 
                                          MyVector( 0.0f, 0.0f, -5.0f ),
                                          1.0f, CR_STICK );

    g_pParticleSystems[3]->Init();

	//
    // Fountain using a single collision plane acting as a floor
	//

    g_pParticleSystems[4] = new CParticleSystem();

    g_pParticleSystems[4]->SetTexture( "particle.bmp" );
    g_pParticleSystems[4]->SetMaxParticles( 200 );
    g_pParticleSystems[4]->SetNumToRelease( 10 );
    g_pParticleSystems[4]->SetReleaseInterval( 0.05f );
    g_pParticleSystems[4]->SetLifeCycle( 5.0f );
    g_pParticleSystems[4]->SetSize( 30.0f );
    g_pParticleSystems[4]->SetColor( MyVector( 1.0f, 1.0f, 1.0f ));
    g_pParticleSystems[4]->SetPosition( MyVector( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[4]->SetVelocity( MyVector( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[4]->SetGravity( MyVector( 0.0f, -9.8f, 0.0f ) );
    g_pParticleSystems[4]->SetWind( MyVector( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[4]->SetVelocityVar( 20.0f );

    g_pParticleSystems[4]->SetCollisionPlane( MyVector( 0.0f, 1.0f, 0.0f ), 
                                          MyVector( 0.0f, 0.0f, 0.0f ) );

    g_pParticleSystems[4]->Init();

	//
    // Fountain boxed-in by 6 collision planes
	//

    g_pParticleSystems[5] = new CParticleSystem();

    g_pParticleSystems[5]->SetTexture( "particle.bmp" );
    g_pParticleSystems[5]->SetMaxParticles( 100 );
    g_pParticleSystems[5]->SetNumToRelease( 5 );
    g_pParticleSystems[5]->SetReleaseInterval( 0.05f );
    g_pParticleSystems[5]->SetLifeCycle( 5.0f );
    g_pParticleSystems[5]->SetSize( 30.0f );
    g_pParticleSystems[5]->SetColor( MyVector( 1.0f, 1.0f, 1.0f ));
    g_pParticleSystems[5]->SetPosition( MyVector( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[5]->SetVelocity( MyVector( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[5]->SetGravity( MyVector( 0.0f, -9.8f, 0.0f ) );
    g_pParticleSystems[5]->SetWind( MyVector( 0.0f, 0.0f, 0.0f ) );
    g_pParticleSystems[5]->SetVelocityVar( 20.0f );

    // Create a series of planes to collide with
    g_pParticleSystems[5]->SetCollisionPlane( MyVector( 0.0f, 1.0f, 0.0f ), 
                                          MyVector( 0.0f, 0.0f, 0.0f ) ); // Floor

    g_pParticleSystems[5]->SetCollisionPlane( MyVector( 1.0f, 0.0f, 0.0f ), 
                                          MyVector(-3.0f, 0.0f, 0.0f ) ); // Left Wall

    g_pParticleSystems[5]->SetCollisionPlane( MyVector(-1.0f, 0.0f, 0.0f ), 
                                          MyVector( 3.0f, 0.0f, 0.0f ) ); // Right Wall

    g_pParticleSystems[5]->SetCollisionPlane( MyVector( 0.0f, 0.0f, 1.0f ), 
                                          MyVector( 0.0f, 0.0f,-3.0f ) ); // Front Wall

    g_pParticleSystems[5]->SetCollisionPlane( MyVector( 0.0f, 0.0f,-1.0f ), 
                                          MyVector( 0.0f, 0.0f, 3.0f ) ); // Back Wall

    g_pParticleSystems[5]->SetCollisionPlane( MyVector( 0.0f,-1.0f, 0.0f ), 
                                          MyVector( 0.0f, 5.0f, 0.0f ) ); // Ceiling

    g_pParticleSystems[5]->Init();*/

	g_pParticleSystems[6] = new CParticleSystem();
	g_pParticleSystems[6]->SetTexture("particle.bmp");
	g_pParticleSystems[6]->SetMaxParticles(2000);
	g_pParticleSystems[6]->SetNumToRelease(5);
	g_pParticleSystems[6]->SetReleaseInterval(0.02f);
	g_pParticleSystems[6]->SetLifeCycle(3.0f);
	g_pParticleSystems[6]->SetSize(5.0f);
	g_pParticleSystems[6]->SetColor(MyVector(1.0f, 1.0f, 1.0f));

	g_pParticleSystems[6]->SetPosition(MyVector(0.0f, -5.0f, 0.0f));
	g_pParticleSystems[6]->SetVelocity(MyVector(0.0f, 0.0f, 0.0f));
	g_pParticleSystems[6]->SetGravity(MyVector(0.0f, 0.0f, 0.0f));

	g_pParticleSystems[6]->SetWind(MyVector(0.0f, 0.0f, 0.0f));
	g_pParticleSystems[6]->SetAirResistence(false);

	g_pParticleSystems[6]->SetVelocityVar(2.0f);

	g_pParticleSystems[6]->SetCollisionPlane(MyVector(0.0f, -4.0f, 0.0f),
		MyVector(0.0f, 0.0f, 0.0f)); // Floor

	/*g_pParticleSystems[6]->SetCollisionPlane(MyVector(1.0f, 0.0f, 0.0f),
		MyVector(-3.0f, 0.0f, 0.0f)); // Left Wall

	g_pParticleSystems[6]->SetCollisionPlane(MyVector(-1.0f, 0.0f, 0.0f),
		MyVector(3.0f, 0.0f, 0.0f)); // Right Wall

	g_pParticleSystems[6]->SetCollisionPlane(MyVector(0.0f, 0.0f, 1.0f),
		MyVector(0.0f, 0.0f, -3.0f)); // Front Wall

	g_pParticleSystems[6]->SetCollisionPlane(MyVector(0.0f, 0.0f, -1.0f),
		MyVector(0.0f, 0.0f, 3.0f)); // Back Wall

	g_pParticleSystems[6]->SetCollisionPlane(MyVector(0.0f, -1.0f, 0.0f),
		MyVector(0.0f, 5.0f, 0.0f)); // Ceiling*/

	g_pParticleSystems[6]->Init();
}
