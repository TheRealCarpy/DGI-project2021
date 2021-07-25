#include "myMatrix.h"


MyMatrix::MyMatrix(void)
{
	this->loadIdentity();
}

void MyMatrix::loadIdentity(void)
{
	static GLfloat identityMatrix[16] =
	{
		1.0,0.0,0.0,0.0,
		0.0,1.0,0.0,0.0,
		0.0,0.0,1.0,0.0,
		0.0,0.0,0.0,1.0
	};
	memcpy(this->myMatrix, identityMatrix, sizeof(identityMatrix));
}

void MyMatrix::getGLMatrix(GLenum pname) 
// Returns any matrix (used by other functions: do use directly)
{
	//remember, you should have a GLfloat myMatrix[16]; defined
	//as a member of your class (see above)
	glGetFloatv(pname,this->myMatrix);
}

void MyMatrix::getGLModelviewMatrix(void) 
// Returns the current modelview matrix
{
	getGLMatrix(GL_MODELVIEW_MATRIX);
}

void MyMatrix::getGLProjectionMatrix(void) 
// Returns the current projection matrix
{
	getGLMatrix(GL_PROJECTION_MATRIX);
}

void MyMatrix::multiplyGLMatrix(void) 
// Multiply our matrix by the current OpenGL one
//Remember to first set the current GL matrix by using the command:
// glMatrixMode()!!!
{
	glMultMatrixf(this->myMatrix);
}

void MyMatrix::setGLMatrix(void)
// Set the OpenGL matrix to that currently specified in myMatrix
//Remember to first set the current GL matrix by using the command:
// glMatrixMode()!!!
{
	glLoadMatrixf(this->myMatrix);
}

void MyMatrix::set_element(int i, float v) {
	this->myMatrix[i] = v;
}
