//Modification on Particle System code by Kevin Harris

//-----------------------------------------------------------------------------
//           Name: particlesystem.h
//         Author: Kevin Harris
//  Last Modified: 07/02/03
//    Description: Header file for the CParticleSystem Class
//-----------------------------------------------------------------------------

#ifndef CPARTICLESYSTEM_H_INCLUDED
#define CPARTICLESYSTEM_H_INCLUDED

#include <math.h>
#include <windows.h>
#include <GL/gl.h>
#include "myvector.h"
#include "mymatrix.h"

//-----------------------------------------------------------------------------
// SYMBOLIC CONSTANTS
//-----------------------------------------------------------------------------

// Classify Point
const int CP_FRONT   = 0;
const int CP_BACK    = 1;
const int CP_ONPLANE = 2;

// Collision Results
const int CR_BOUNCE  = 0;
const int CR_STICK   = 1;
const int CR_RECYCLE = 2;

const float OGL_PI = 3.141592654f;

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------

struct Plane
{
    MyVector m_vNormal;           // The plane's normal
    MyVector m_vPoint;            // A coplanar point within the plane
    float    m_fBounceFactor;     // Coefficient of restitution (or how bouncy the plane is)
    int      m_nCollisionResult;  // What will particles do when they strike the plane

    Plane   *m_pNext;             // Next plane in list
};

struct Particle
{
    MyVector  m_vCurPos;    // Current position of particle
    MyVector  m_vCurVel;    // Current velocity of particle
    float     m_weight;
    float     m_fInitTime;  // Time of creation of particle
    float m_alpha;
    float m_size;

    Particle *m_pNext;      // Next particle in list
};

struct PointVertex
{
    MyVector posit;
    MyVector color;
};

//-----------------------------------------------------------------------------
// CLASSES
//-----------------------------------------------------------------------------

class CParticleSystem
{

public:

    CParticleSystem(void);
    ~CParticleSystem(void);

    void SetMaxParticles( DWORD dwMaxParticles ) { m_dwMaxParticles = dwMaxParticles; }
	DWORD GetMaxParticles( void ) { return m_dwMaxParticles; }

    void SetNumToRelease( DWORD dwNumToRelease ) { m_dwNumToRelease = dwNumToRelease; }
	DWORD GetNumToRelease( void ) { return m_dwNumToRelease; }

    void SetReleaseInterval( float fReleaseInterval ) { m_fReleaseInterval = fReleaseInterval; }
    float GetReleaseInterval( void ) { return m_fReleaseInterval; }

    void SetLifeCycle( float fLifeCycle ) { m_fLifeCycle = fLifeCycle; }
	float GetLifeCycle( void ) { return m_fLifeCycle; }

    void SetSize( float fSize ) { m_fSize = fSize; }
	float GetSize( void ) { return m_fSize; }
	float GetMaxPointSize( void ) { return m_fMaxPointSize; }

    void SetColor( MyVector clrColor ) { m_clrColor = clrColor; }
	MyVector GetColor( void ) { return m_clrColor; }

    void SetPosition( MyVector vPosition ) { m_vPosition = vPosition; }
	MyVector GetPosition( void ) { return m_vPosition; }

    void SetVelocity( MyVector vVelocity ) { m_vVelocity = vVelocity; }
	MyVector GetVelocity( void ) { return m_vVelocity; }

    void SetGravity( MyVector vGravity ) { m_vGravity = vGravity; }
	MyVector GetGravity( void ) { return m_vGravity; }

    void SetWind( MyVector vWind ) { m_vWind = vWind; }
	MyVector GetWind( void ) { return m_vWind; }

    void SetAirResistence( bool bAirResistence ) { m_bAirResistence = bAirResistence; }
	bool GetAirResistence( void ) { return m_bAirResistence; }

    void SetVelocityVar( float fVelocityVar ) { m_fVelocityVar = fVelocityVar; }
	float GetVelocityVar( void ) { return m_fVelocityVar; }

    void SetCollisionPlane( MyVector vPlaneNormal, MyVector vPoint, 
                            float fBounceFactor = 1.0f, int nCollisionResult = CR_BOUNCE );

    int Init( void );
    int Update( float fElapsedTime );
    void Render( void );
	void RenderSimple( void );

    void SetTexture( char *chTexFile );
    GLuint GetTextureID(void);

	void RestartParticleSystem(void);

	void Render_planes(void);

private:

    Particle   *m_pActiveList;
    Particle   *m_pFreeList;
    Plane      *m_pPlanes;
	DWORD       m_dwActiveCount;
	float       m_fCurrentTime;
	float       m_fLastUpdate;
    float       m_fMaxPointSize;
    bool        m_bDeviceSupportsPSIZE;
    GLuint      m_textureID;
    
    // Particle Attributes
    DWORD       m_dwMaxParticles;
    DWORD       m_dwNumToRelease;
    float       m_fReleaseInterval;
    float       m_fLifeCycle;
    float       m_fSize;
    MyVector    m_clrColor;
    MyVector    m_vPosition;
    MyVector    m_vVelocity;
    MyVector    m_vGravity;
    MyVector    m_vWind;
    bool        m_bAirResistence;
    float       m_fVelocityVar;
    char       *m_chTexFile;
};

#endif /* CPARTICLESYSTEM_H_INCLUDED */
