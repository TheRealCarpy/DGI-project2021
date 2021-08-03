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

void MyMatrix::translate(float tx, float ty) {
	glMatrixMode(GL_MODELVIEW_MATRIX);
	getGLModelviewMatrix();
	this->myMatrix[12] += tx;
	this->myMatrix[13] += ty;
	setGLMatrix();
}

void MyMatrix::rotate(float a) {
	glMatrixMode(GL_MODELVIEW_MATRIX);
	this->myMatrix[0] = cos(a);
	this->myMatrix[1] = sin(a);
	this->myMatrix[4] = -sin(a);
	this->myMatrix[5] = cos(a);

	multiplyGLMatrix();
}

MyVector MyMatrix::multiply_vector(const MyVector v) {
	MyVector result;
	result.x = this->myMatrix[0] * v.x + this->myMatrix[1] * v.x + this->myMatrix[2] * v.x + this->myMatrix[3] * v.x;
	result.y = this->myMatrix[4] * v.y + this->myMatrix[5] * v.y + this->myMatrix[6] * v.y + this->myMatrix[7] * v.y;
	result.z = this->myMatrix[8] * v.z + this->myMatrix[9] * v.z + this->myMatrix[10] * v.z + this->myMatrix[11] * v.z;
	return result;
}
