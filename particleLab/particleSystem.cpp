//-----------------------------------------------------------------------------
//           Name: particlesystem.h
//         Author: Kevin Harris
//  Last Modified: 02/01/05
//    Description: Implementation file for the CParticleSystem Class
//-----------------------------------------------------------------------------

#include <tchar.h>
#include <windows.h>
#include <mmsystem.h>
#include "stdio.h"
#include <GL/gl.h>
#include "glaux.h"

#include "particlesystem.h"

//-----------------------------------------------------------------------------
// FUNCTION POINTERS FOR OPENGL EXTENSIONS
//-----------------------------------------------------------------------------

// For convenience, this project ships with its own "glext.h" extension header 
// file. If you have trouble running this sample, it may be that this "glext.h" 
// file is defining something that your hardware doesn�t actually support. 
// Try recompiling the sample using your own local, vendor-specific "glext.h" 
// header file.

#include "glext.h"      // Sample's header file
//#include <GL/glext.h> // Your local header file

//#ifndef GL_ARB_point_parameters
PFNGLPOINTPARAMETERFARBPROC  glPointParameterfARB = NULL;
PFNGLPOINTPARAMETERFVARBPROC glPointParameterfvARB = NULL;

void setAlpha(Particle*, float);
float getSize(Particle*);

//-----------------------------------------------------------------------------
// Name: getRandomMinMax()
// Desc: Gets a random number between min/max boundaries
//-----------------------------------------------------------------------------
float getRandomMinMax(float fMin, float fMax)
{
    float fRandNum = (float)rand() / RAND_MAX;
    return fMin + (fMax - fMin) * fRandNum;
}

//-----------------------------------------------------------------------------
// Name: getRandomVector()
// Desc: Generates a random vector where X,Y, and Z components are between
//       -1.0 and 1.0
//-----------------------------------------------------------------------------
MyVector getRandomVector(void)
{
    MyVector vVector;

    // Pick a random Z between -1.0f and 1.0f.
    vVector.z = getRandomMinMax(-1.0f, 1.0f);

    // Get radius of this circle
    float radius = (float)sqrt(1 - vVector.z * vVector.z);

    // Pick a random point on a circle.
    float t = getRandomMinMax(-OGL_PI, OGL_PI);

    // Compute matching X and Y for our Z.
    vVector.x = (float)cosf(t) * radius;
    vVector.y = (float)sinf(t) * radius;

    return vVector;
}

//-----------------------------------------------------------------------------
// Name : classifyPoint()
// Desc : Classifies a point against the plane passed
//-----------------------------------------------------------------------------
int classifyPoint(MyVector* vPoint, Plane* pPlane)
{
    MyVector vDirection = pPlane->m_vPoint - *vPoint;

    float fResult = vDirection.getDotProduct(pPlane->m_vNormal);

    if (fResult < -0.001f)
        return CP_FRONT;

    if (fResult > 0.001f)
        return CP_BACK;

    return CP_ONPLANE;
}

//-----------------------------------------------------------------------------
// Name: CParticleSystem()
// Desc:
//-----------------------------------------------------------------------------
CParticleSystem::CParticleSystem()
{
    m_pActiveList = NULL; // Head node of point sprites that are active
    m_pFreeList = NULL; // Head node of point sprites that are inactive and waiting to be recycled.
    m_pPlanes = NULL;
    m_dwActiveCount = 0;
    m_fCurrentTime = 0.0f;
    m_fLastUpdate = 0.0f;
    m_chTexFile = NULL;
    m_textureID = NULL;
    m_dwMaxParticles = 1;
    m_dwNumToRelease = 1;
    m_fReleaseInterval = 1.0f;
    m_fLifeCycle = 1.0f;
    m_fSize = 1.0f;
    m_clrColor = MyVector(1.0f, 1.0f, 1.0f);
    m_vPosition = MyVector(0.0f, 0.0f, 0.0f);
    m_vVelocity = MyVector(0.0f, 0.0f, 0.0f);
    m_vGravity = MyVector(0.0f, 0.0f, 0.0f);
    m_vWind = MyVector(0.0f, 0.0f, 0.0f);
    m_bAirResistence = true;
    m_fVelocityVar = 1.0f;

    SetTexture("particle.bmp");
}

//-----------------------------------------------------------------------------
// Name: ~CParticleSystem()
// Desc:
//-----------------------------------------------------------------------------
CParticleSystem::~CParticleSystem()
{
    glDeleteTextures(1, &m_textureID);

    while (m_pPlanes) // Repeat till null...
    {
        Plane* pPlane = m_pPlanes;   // Hold onto the first one
        m_pPlanes = pPlane->m_pNext; // Move down to the next one
        delete pPlane;               // Delete the one we're holding
    }

    while (m_pActiveList)
    {
        Particle* pParticle = m_pActiveList;
        m_pActiveList = pParticle->m_pNext;
        delete pParticle;
    }
    m_pActiveList = NULL;

    while (m_pFreeList)
    {
        Particle* pParticle = m_pFreeList;
        m_pFreeList = pParticle->m_pNext;
        delete pParticle;
    }
    m_pFreeList = NULL;

    if (m_chTexFile != NULL)
    {
        delete[] m_chTexFile;
        m_chTexFile = NULL;
    }
}

//-----------------------------------------------------------------------------
// Name: SetTexture()
// Desc: 
//-----------------------------------------------------------------------------
void CParticleSystem::SetTexture(char* chTexFile)
{
    // Deallocate the memory that was previously reserved for this string.
    if (m_chTexFile != NULL)
    {
        delete[] m_chTexFile;
        m_chTexFile = NULL;
    }

    // Dynamically allocate the correct amount of memory.
    m_chTexFile = new char[strlen(chTexFile) + 1];

    // If the allocation succeeds, copy the initialization string.
    if (m_chTexFile != NULL)
        strcpy(m_chTexFile, chTexFile);
}

//-----------------------------------------------------------------------------
// Name: GetTextureObject()
// Desc: 
//-----------------------------------------------------------------------------
GLuint CParticleSystem::GetTextureID()
{
    return m_textureID;
}

//-----------------------------------------------------------------------------
// Name: SetCollisionPlane()
// Desc: 
//-----------------------------------------------------------------------------
void CParticleSystem::SetCollisionPlane(MyVector vPlaneNormal, MyVector vPoint,
    float fBounceFactor, int nCollisionResult)
{
    Plane* pPlane = new Plane;

    pPlane->m_vNormal = vPlaneNormal;
    pPlane->m_vPoint = vPoint;
    pPlane->m_fBounceFactor = fBounceFactor;
    pPlane->m_nCollisionResult = nCollisionResult;

    pPlane->m_pNext = m_pPlanes; // Attach the curent list to it...
    m_pPlanes = pPlane;          // ... and make it the new head.
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: 
//-----------------------------------------------------------------------------
int CParticleSystem::Init(void)
{
    printf("\nInitialising ParticleSystem");
    //
    // If the required extensions are present, get the addresses of thier 
    // functions that we wish to use...
    //

    char* ext = (char*)glGetString(GL_EXTENSIONS);

    if (strstr(ext, "GL_ARB_point_parameters") == NULL)
    {
        printf("GL_ARB_point_parameters extension was not found");
        MessageBox(NULL, "GL_ARB_point_parameters extension was not found",
            "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return 0;
    }
    else
    {
        glPointParameterfARB = (PFNGLPOINTPARAMETERFARBPROC)wglGetProcAddress("glPointParameterfARB");
        glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)wglGetProcAddress("glPointParameterfvARB");

        if (!glPointParameterfARB || !glPointParameterfvARB)
        {
            printf("One or more GL_ARB_point_parameters functions were not found");
            MessageBox(NULL, "One or more GL_ARB_point_parameters functions were not found",
                "ERROR", MB_OK | MB_ICONEXCLAMATION);
            return 0;
        }
    }

    //
    // Load up the point sprite's texture...
    //

    AUX_RGBImageRec* pTextureImage = auxDIBImageLoad(m_chTexFile);

    if (pTextureImage != NULL)
    {
        printf("\nAssigning texture image %s", m_chTexFile);
        glGenTextures(1, &m_textureID);

        glBindTexture(GL_TEXTURE_2D, m_textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, 3, pTextureImage->sizeX, pTextureImage->sizeY, 0,
            GL_RGB, GL_UNSIGNED_BYTE, pTextureImage->data);
    }

    if (pTextureImage)
    {
        if (pTextureImage->data)
            free(pTextureImage->data);

        free(pTextureImage);
        printf("...assigned");
    }

    //
    // If you want to know the max size that a point sprite can be set 
    // to, do this.
    //

    glGetFloatv(GL_POINT_SIZE_MAX_ARB, &m_fMaxPointSize);
    glPointSize(m_fMaxPointSize);

    return 1;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc:
//-----------------------------------------------------------------------------
int CParticleSystem::Update(FLOAT fElpasedTime)
{
    Particle* pParticle;
    Particle** ppParticle;
    Plane* pPlane;
    Plane** ppPlane;
    MyVector vOldPosition;

    m_fCurrentTime += fElpasedTime;     // Update our particle system timer...

    printf("\nUpdating particle system: current time %f", m_fCurrentTime);

    ppParticle = &m_pActiveList; // Start at the head of the active list

    int i = 0;
    while (*ppParticle)
    {
        //printf("\nUpdate particle %d", i++);
        pParticle = *ppParticle; // Set a pointer to the head

        // Calculate new position
        float fTimePassed = m_fCurrentTime - pParticle->m_fInitTime;
        //printf("\nTime passed is %lf (lifecycle: %lf)", fTimePassed, m_fLifeCycle);

        if (fTimePassed >= m_fLifeCycle)
        {
            // Time is up, put the particle back on the free list...
            *ppParticle = pParticle->m_pNext;
            pParticle->m_pNext = m_pFreeList;
            m_pFreeList = pParticle;

            --m_dwActiveCount;
        }
        else
        {

            // Update particle position and velocity
            // Update velocity with respect to Gravity (Constant Accelaration)
            pParticle->m_vCurVel += m_vGravity * fElpasedTime;

            // Update velocity with respect to Wind (Accelaration based on 
            // difference of vectors)
            if (m_bAirResistence == true)
                pParticle->m_vCurVel += (m_vWind - pParticle->m_vCurVel) * fElpasedTime;

            // Finally, update position with respect to velocity
            vOldPosition = pParticle->m_vCurPos;
            pParticle->m_vCurPos += pParticle->m_vCurVel * pParticle->m_weight * fElpasedTime;
            //setAlpha(pParticle, m_fLifeCycle, fTimePassed);

            //-----------------------------------------------------------------
            // BEGIN Checking the particle against each plane that was set up

            ppPlane = &m_pPlanes; // Set a pointer to the head


            while (*ppPlane)
            {
                pPlane = *ppPlane;
                int result = classifyPoint(&pParticle->m_vCurPos, pPlane);

                if (result == CP_BACK /*|| result == CP_ONPLANE */)
                {
                    if (pPlane->m_nCollisionResult == CR_BOUNCE)
                    {
                        pParticle->m_vCurPos = vOldPosition;

                        //-----------------------------------------------------------------
                        //
                        // The new velocity vector of a particle that is bouncing off
                        // a plane is computed as follows:
                        //
                        // Vn = (N.V) * N
                        // Vt = V - Vn
                        // Vp = Vt - Kr * Vn
                        //
                        // Where:
                        // 
                        // .  = Dot product operation
                        // N  = The normal of the plane from which we bounced
                        // V  = Velocity vector prior to bounce
                        // Vn = Normal force
                        // Kr = The coefficient of restitution ( Ex. 1 = Full Bounce, 
                        //      0 = Particle Sticks )
                        // Vp = New velocity vector after bounce
                        //
                        //-----------------------------------------------------------------

                        float Kr = pPlane->m_fBounceFactor;

                        MyVector Vn = pPlane->m_vNormal.getDotProduct(pParticle->m_vCurVel) * pPlane->m_vNormal;

                        MyVector Vt = pParticle->m_vCurVel - Vn;
                        MyVector Vp = Vt - Kr * Vn;

                        pParticle->m_vCurVel = Vp;
                    }
                    else if (pPlane->m_nCollisionResult == CR_RECYCLE)
                    {
                        pParticle->m_fInitTime -= m_fLifeCycle;
                    }

                    else if (pPlane->m_nCollisionResult == CR_STICK)
                    {
                        pParticle->m_vCurPos = vOldPosition;
                        pParticle->m_vCurVel = MyVector(0.0f, 0.0f, 0.0f);
                    }
                }

                ppPlane = &pPlane->m_pNext;
            }

            // END Plane Checking
            //-----------------------------------------------------------------
            // particle color based on z position, the higher z fr�n the camera, the lower intensity
            //float dist;
            //pParticle->m_color = MyVector(1.0f, 1.0f, 1.0f);
            ppParticle = &pParticle->m_pNext;
        }
    }

    //-------------------------------------------------------------------------
    // Emit new particles in accordance to the flow rate...
    // 
    // NOTE: The system operates with a finite number of particles.
    //       New particles will be created until the max amount has
    //       been reached, after that, only old particles that have
    //       been recycled can be reintialized and used again.
    //-------------------------------------------------------------------------

    if (m_fCurrentTime - m_fLastUpdate > m_fReleaseInterval)
    {
        // Reset update timing...
        m_fLastUpdate = m_fCurrentTime;

        // Emit new particles at specified flow rate...
        for (DWORD i = 0; i < m_dwNumToRelease; ++i)
        {
            // Do we have any free particles to put back to work?
            if (m_pFreeList)
            {
                // If so, hand over the first free one to be reused.
                pParticle = m_pFreeList;
                // Then make the next free particle in the list next to go!
                m_pFreeList = pParticle->m_pNext;
            }
            else
            {
                // There are no free particles to recycle...
                // We'll have to create a new one from scratch!
                if (m_dwActiveCount < m_dwMaxParticles)
                {
                    if (NULL == (pParticle = new Particle))
                        return E_OUTOFMEMORY;
                }
            }

            if (m_dwActiveCount < m_dwMaxParticles)
            {
                pParticle->m_pNext = m_pActiveList; // Make it the new head...
                m_pActiveList = pParticle;

                // Set the attributes for our new particle...
                pParticle->m_vCurVel = m_vVelocity;
                pParticle->m_size = this->m_fSize + getRandomMinMax(-10, 10);
                pParticle->m_weight = pParticle->m_size/this->m_fSize * getRandomMinMax(0.1, 1); //Set weight, needs to go random from 0.1 to 2
                if (m_fVelocityVar != 0.0f)
                {
                    MyVector vRandomVec = getRandomVector();
                    pParticle->m_vCurVel += vRandomVec * m_fVelocityVar;
                }

                pParticle->m_fInitTime = m_fCurrentTime;
                pParticle->m_vCurPos = MyVector(getRandomMinMax(-5, 5), m_vPosition.y, getRandomMinMax(-5, 5));

                ++m_dwActiveCount;
            }
        }
    }

    // Query for the max point size supported by the hardware
    float maxSize = 0.0f;
    glGetFloatv(GL_POINT_SIZE_MAX_ARB, &m_fMaxPointSize);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RestartParticleSystem()
// Desc: 
//-----------------------------------------------------------------------------
void CParticleSystem::RestartParticleSystem(void)
{
    Particle* pParticle;
    Particle** ppParticle;

    ppParticle = &m_pActiveList; // Start at the head of the active list

    while (*ppParticle)
    {
        pParticle = *ppParticle; // Set a pointer to the head

        // Put the particle back on the free list...
        *ppParticle = pParticle->m_pNext;
        pParticle->m_pNext = m_pFreeList;
        m_pFreeList = pParticle;

        --m_dwActiveCount;
    }
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: 
//-----------------------------------------------------------------------------
void CParticleSystem::Render(void)
{
    Particle* pParticle = m_pActiveList;

    //
    // Set up the OpenGL state machine for using point sprites...
    //

    // This is how will our point sprite's size will be modified by 
    // distance from the viewer.
    float quadratic[] = { 1.0f, 0.0f, 0.01f };
    glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic);

    // The alpha of a point is calculated to allow the fading of points 
    // instead of shrinking them past a defined threshold size. The threshold 
    // is defined by GL_POINT_FADE_THRESHOLD_SIZE_ARB and is not clamped to 
    // the minimum and maximum point sizes.
    glPointParameterfARB(GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f);

    glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, 1.0f);
    glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, m_fMaxPointSize);

    // Specify point sprite texture coordinate replacement mode for each texture unit
    glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);

    //
    // Render point sprites...
    //

    bool testRender = false;

    if (testRender)//for this to work, need to disable textures in OGL
    {
        glDisable(GL_POINT_SPRITE_ARB);
    }
    else glEnable(GL_POINT_SPRITE_ARB);

    glPointSize(m_fSize);

    glColor3f(m_clrColor.x, m_clrColor.y, m_clrColor.z);

    glBegin(GL_POINTS);
    {
        // Render each particle...
        while (pParticle)
        {
            glVertex3f(pParticle->m_vCurPos.x,
                pParticle->m_vCurPos.y,
                pParticle->m_vCurPos.z);
            setAlpha(pParticle, m_fLifeCycle - m_fCurrentTime - pParticle->m_fInitTime);
            glColor4f(m_clrColor.x, m_clrColor.y, m_clrColor.z, pParticle->m_alpha);
            glPointSize(getSize(pParticle));

            pParticle = pParticle->m_pNext;
        }
    }
    glEnd();
    glDisable(GL_POINT_SPRITE_ARB);
}

void CParticleSystem::RenderSimple(void)
{
    Particle* pParticle = m_pActiveList;

    //Use simple rendering system - GL_POINTS only
    glPointSize(2.2);

    glColor3f(m_clrColor.x, m_clrColor.y, m_clrColor.z);

    glBegin(GL_POINTS);
    {
        // Render each particle...
        while (pParticle)
        {
            glVertex3f(pParticle->m_vCurPos.x,
                pParticle->m_vCurPos.y,
                pParticle->m_vCurPos.z);
            glColor4f(m_clrColor.x, m_clrColor.y, m_clrColor.z, pParticle->m_alpha);
            glPointSize(getSize(pParticle));

            pParticle = pParticle->m_pNext;
        }
    }
    glEnd();
}

void CParticleSystem::Render_planes() {
    if (m_pPlanes == NULL) {
        return;
    }
    Plane* copy = m_pPlanes;
    MyVector floor, left, right, front, back, ceiling;
    MyVector lf_top, rf_top, lb_top, rb_top, lf_bot, rf_bot, lb_bot, rb_bot;

    /*ceiling = copy->m_vPoint;
    copy = m_pPlanes->m_pNext;
    back = copy->m_vPoint;
    copy = m_pPlanes->m_pNext;
    front = copy->m_vPoint;
    copy = m_pPlanes->m_pNext;
    right = copy->m_vPoint;
    copy = m_pPlanes->m_pNext;
    left = copy->m_vPoint;
    copy = m_pPlanes->m_pNext;
    floor = copy->m_vPoint;

    lf_top = ceiling.addTo(left).addTo(front);
    rf_top = right.addTo(ceiling).addTo(front);
    lb_top = left.addTo(back).addTo(ceiling);
    rb_top = right.addTo(back).addTo(ceiling);
    lf_bot = left.addTo(front).addTo(floor);
    rf_bot = right.addTo(front).addTo(floor);
    lb_bot = left.addTo(back).addTo(floor);
    rb_bot = right.addTo(back).addTo(floor);*/

    /*GLfloat vertices[] = { -1, -1, 0,
                            -1, 1, 0,
                            1, 1, 0 ,
                            1, -1, 0 };

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, vertices);
        glDrawArrays(GL_LINES, 0, 4);
        glDisableClientState(GL_VERTEX_ARRAY);*/
    glPushMatrix();
    glRotatef(1, 0, 1, 0);
    glBegin(GL_QUADS);
    glVertex3f(-3, 0, -3);
    glVertex3f(-3, 0, 3);
    glVertex3f(-3, 5, 3);
    glVertex3f(-3, 5, -3);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glRotatef(1, 0, 1, 0);
    glBegin(GL_QUADS);
    glVertex3f(3, 0, -3);
    glVertex3f(3, 0, 3);
    glVertex3f(3, 5, 3);
    glVertex3f(3, 5, -3);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glRotatef(1, 1, 0, 0);
    glBegin(GL_QUADS);
    glVertex3f(-3, 0, -3);
    glVertex3f(3, 0, -3);
    glVertex3f(3, 0, 3);
    glVertex3f(-3, 0, 3);
    glEnd();
    glPopMatrix();
}
//REAL
void setAlpha(Particle* p, float time_left)
{
    float distance = p->m_vCurPos.getDistanceXZ(MyVector(0,0,0));
    if (distance == 0)
    {
        p->m_alpha = 1.0;
    }
    else
    {
        p->m_alpha = 1.0/distance/5;
    }
}
/*
//DEBUG
void setAlpha(Particle* p, float m_fLifeCycle, float fTimePassed)
{
    if (fTimePassed > 0.5 * m_fLifeCycle) {
        p->m_alpha *= 0.5;
    }
    else {
        //printf("\n %lf", p->m_alpha);
        p->m_alpha = 1;
    }
}*/

float getSize(Particle* p) {
    float distance = p->m_vCurPos.getDistanceXZ(MyVector(0, 0, 0));
    if (distance <= 5) {
        return p->m_size - distance;
    }
    else {
        return p->m_size + distance;
    }
}

